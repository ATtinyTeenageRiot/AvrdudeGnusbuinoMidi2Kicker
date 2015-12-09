/*
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2000-2005  Brian S. Dean <bsd@bsdhome.com>
 * Copyright 2007-2009 Joerg Wunsch <j@uriah.heep.sax.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* $Id: main.c 1161 2013-05-05 13:35:35Z rliebscher $ */

/*
 * Code to program an Atmel AVR device through one of the supported
 * programmers.
 *
 * For parallel port connected programmers, the pin definitions can be
 * changed via a config file.  See the config file for instructions on
 * how to add a programmer definition.
 *  
 */

#include "ac_cfg.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "avr.h"
#include "config.h"
#include "confwin.h"
#include "fileio.h"
#include "lists.h"
#include "par.h"
#include "pindefs.h"
#include "term.h"
#include "safemode.h"
#include "update.h"
#include "pgm_type.h"


/* Get VERSION from ac_cfg.h */
char * version      = VERSION;

char * progname;
char   progbuf[PATH_MAX]; /* temporary buffer of spaces the same
                             length as progname; used for lining up
                             multiline messages */

struct list_walk_cookie
{
    FILE *f;
    const char *prefix;
};

static LISTID updates = NULL;

static LISTID extended_params = NULL;

static LISTID additional_config_files = NULL;

static PROGRAMMER * pgm;

/*
 * global options
 */
int    do_cycles;   /* track erase-rewrite cycles */
int    verbose;     /* verbose output */
int    quell_progress; /* un-verebose output */
int    ovsigck;     /* 1=override sig check, 0=don't */


/*
 * usage message
 */
static void usage(void)
{
  fprintf(stderr,
 "Usage: %s [options]\n"
 "Options:\n"
 "  -p <partno>                Required. Specify AVR device.\n"
 "  -b <baudrate>              Override RS-232 baud rate.\n"
 "  -B <bitclock>              Specify JTAG/STK500v2 bit clock period (us).\n"
 "  -C <config-file>           Specify location of configuration file.\n"
 "  -c <programmer>            Specify programmer type.\n"
 "  -D                         Disable auto erase for flash memory\n"
 "  -i <delay>                 ISP Clock Delay [in microseconds]\n"
 "  -P <port>                  Specify connection port.\n"
 "  -F                         Override invalid signature check.\n"
 "  -e                         Perform a chip erase.\n"
 "  -O                         Perform RC oscillator calibration (see AVR053). \n"
 "  -U <memtype>:r|w|v:<filename>[:format]\n"
 "                             Memory operation specification.\n"
 "                             Multiple -U options are allowed, each request\n"
 "                             is performed in the order specified.\n"
 "  -n                         Do not write anything to the device.\n"
 "  -V                         Do not verify.\n"
 "  -u                         Disable safemode, default when running from a script.\n"
 "  -s                         Silent safemode operation, will not ask you if\n"
 "                             fuses should be changed back.\n"
 "  -t                         Enter terminal mode.\n"
 "  -E <exitspec>[,<exitspec>] List programmer exit specifications.\n"
 "  -x <extended_param>        Pass <extended_param> to programmer.\n"
 "  -y                         Count # erase cycles in EEPROM.\n"
 "  -Y <number>                Initialize erase cycle # in EEPROM.\n"
 "  -v                         Verbose output. -v -v for more.\n"
 "  -q                         Quell progress output. -q -q for less.\n"
 "  -?                         Display this usage.\n"
 "\navrdude version %s, URL: <http://savannah.nongnu.org/projects/avrdude/>\n"
          ,progname, version);
}


static void update_progress_tty (int percent, double etime, char *hdr)
{
  static char hashes[51];
  static char *header;
  static int last = 0;
  int i;

  setvbuf(stderr, (char*)NULL, _IONBF, 0);

  hashes[50] = 0;

  memset (hashes, ' ', 50);
  for (i=0; i<percent; i+=2) {
    hashes[i/2] = '#';
  }

  if (hdr) {
    fprintf (stderr, "\n");
    last = 0;
    header = hdr;
  }

  if (last == 0) {
    fprintf(stderr, "\r%s | %s | %d%% %0.2fs", 
            header, hashes, percent, etime);
  }

  if (percent == 100) {
    if (!last) fprintf (stderr, "\n\n");
    last = 1;
  }

  setvbuf(stderr, (char*)NULL, _IOLBF, 0);
}

static void update_progress_no_tty (int percent, double etime, char *hdr)
{
  static int done = 0;
  static int last = 0;
  int cnt = (percent>>1)*2;

  setvbuf(stderr, (char*)NULL, _IONBF, 0);

  if (hdr) {
    fprintf (stderr, "\n%s | ", hdr);
    last = 0;
    done = 0;
  }
  else {
    while ((cnt > last) && (done == 0)) {
      fprintf (stderr, "#");
      cnt -=  2;
    }
  }

  if ((percent == 100) && (done == 0)) {
    fprintf (stderr, " | 100%% %0.2fs\n\n", etime);
    last = 0;
    done = 1;
  }
  else
    last = (percent>>1)*2;    /* Make last a multiple of 2. */

  setvbuf(stderr, (char*)NULL, _IOLBF, 0);
}

static void list_programmers_callback(const char *name, const char *desc,
                                      const char *cfgname, int cfglineno,
                                      void *cookie)
{
    struct list_walk_cookie *c = (struct list_walk_cookie *)cookie;
    if (verbose){
        fprintf(c->f, "%s%-16s = %-30s [%s:%d]\n",
                c->prefix, name, desc, cfgname, cfglineno);
    } else {
        fprintf(c->f, "%s%-16s = %-s\n",
                c->prefix, name, desc);
    }
}

static void list_programmers(FILE * f, const char *prefix, LISTID programmers)
{
    struct list_walk_cookie c;

    c.f = f;
    c.prefix = prefix;

    sort_programmers(programmers);

    walk_programmers(programmers, list_programmers_callback, &c);
}

static void list_programmer_types_callback(const char *name, const char *desc,
                                      void *cookie)
{
    struct list_walk_cookie *c = (struct list_walk_cookie *)cookie;
    fprintf(c->f, "%s%-16s = %-s\n",
                c->prefix, name, desc);
}

static void list_programmer_types(FILE * f, const char *prefix)
{
    struct list_walk_cookie c;

    c.f = f;
    c.prefix = prefix;

    walk_programmer_types(list_programmer_types_callback, &c);
}

static void list_avrparts_callback(const char *name, const char *desc,
                                   const char *cfgname, int cfglineno,
                                   void *cookie)
{
    struct list_walk_cookie *c = (struct list_walk_cookie *)cookie;

    /* hide ids starting with '.' */
    if ((verbose < 2) && (name[0] == '.'))
        return;

    if (verbose) {
        fprintf(c->f, "%s%-8s = %-18s [%s:%d]\n",
                c->prefix, name, desc, cfgname, cfglineno);
    } else {
        fprintf(c->f, "%s%-8s = %s\n",
                c->prefix, name, desc);
    }
}

static void list_parts(FILE * f, const char *prefix, LISTID avrparts)
{
    struct list_walk_cookie c;

    c.f = f;
    c.prefix = prefix;

    sort_avrparts(avrparts);

    walk_avrparts(avrparts, list_avrparts_callback, &c);
}

static void exithook(void)
{
    if (pgm->teardown)
        pgm->teardown(pgm);
}

static void cleanup_main(void)
{
    if (updates) {
        ldestroy_cb(updates, (void(*)(void*))free_update);
        updates = NULL;
    }
    if (extended_params) {
        ldestroy(extended_params);
        extended_params = NULL;
    }
    if (additional_config_files) {
        ldestroy(additional_config_files);
        additional_config_files = NULL;
    }

    cleanup_config();
}

/*
 * main routine
 */
int main(int argc, char * argv [])
{
  int              rc;          /* general return code checking */
  int              exitrc;      /* exit code for main() */
  int              i;           /* general loop counter */
  int              ch;          /* options flag */
  int              len;         /* length for various strings */
  struct avrpart * p;           /* which avr part we are programming */
  AVRMEM         * sig;         /* signature data */
  struct stat      sb;
  UPDATE         * upd;
  LNODEID        * ln;


  /* options / operating mode variables */
  int     erase;       /* 1=erase chip, 0=don't */
  int     calibrate;   /* 1=calibrate RC oscillator, 0=don't */
  char  * port;        /* device port (/dev/xxx) */
  int     terminal;    /* 1=enter terminal mode, 0=don't */
  int     verify;      /* perform a verify operation */
  char  * exitspecs;   /* exit specs string from command line */
  char  * programmer;  /* programmer id */
  char  * partdesc;    /* part id */
  char    sys_config[PATH_MAX]; /* system wide config file */
  char    usr_config[PATH_MAX]; /* per-user config file */
  int     cycles;      /* erase-rewrite cycles */
  int     set_cycles;  /* value to set the erase-rewrite cycles to */
  char  * e;           /* for strtol() error checking */
  int     baudrate;    /* override default programmer baud rate */
  double  bitclock;    /* Specify programmer bit clock (JTAG ICE) */
  int     ispdelay;    /* Specify the delay for ISP clock */
  int     safemode;    /* Enable safemode, 1=safemode on, 0=normal */
  int     silentsafe;  /* Don't ask about fuses, 1=silent, 0=normal */
  int     init_ok;     /* Device initialization worked well */
  int     is_open;     /* Device open succeeded */
  enum updateflags uflags = UF_AUTO_ERASE; /* Flags for do_op() */
  unsigned char safemode_lfuse = 0xff;
  unsigned char safemode_hfuse = 0xff;
  unsigned char safemode_efuse = 0xff;
  unsigned char safemode_fuse = 0xff;

  char * safemode_response;
  int fuses_specified = 0;
  int fuses_updated = 0;
#if !defined(WIN32NATIVE)
  char  * homedir;
#endif


  partdesc      = NULL;
  port          = NULL;
  erase         = 0;
  calibrate     = 0;
  p             = NULL;
  ovsigck       = 0;
  terminal      = 0;
  verify        = 1;        /* on by default */
  quell_progress = 0;
  exitspecs     = NULL;
  pgm           = NULL;
  programmer    = default_programmer;
  verbose       = 0;
  do_cycles     = 0;
  set_cycles    = -1;
  baudrate      = 0;
  bitclock      = 0.0;
  ispdelay      = 0;
  safemode      = 1;       /* Safemode on by default */
  silentsafe    = 0;       /* Ask by default */
  is_open       = 0;

  pgm = pgm_new();
  usbasp_initpgm(pgm);

}

