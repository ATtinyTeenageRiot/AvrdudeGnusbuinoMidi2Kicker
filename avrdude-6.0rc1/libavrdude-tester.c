

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
#include "term.h"
#include "safemode.h"
#include "update.h"
#include "usbasp.h"


/* Get VERSION from ac_cfg.h */
char * version      = VERSION;

char * progname;
char   progbuf[PATH_MAX]; /* temporary buffer of spaces the same
                             length as progname; used for lining up
                             multiline messages */


static PROGRAMMER * pgm_usbasp;

/*
 * global options
 */
int    do_cycles;   /* track erase-rewrite cycles */
int    verbose;     /* verbose output */
int    quell_progress; /* un-verebose output */
int    ovsigck;     /* 1=override sig check, 0=don't */


/*
 * main routine
 */
int main(int argc, char * argv [])
{
    int     rc;          /* general return code checking */
    int     exitrc;      /* exit code for main() */
    int     init_ok;     /* Device initialization worked well */
    int     is_open;     /* Device open succeeded */


    struct avrpart    * attiny_85;

    is_open       = 0;

    progname = argv[0];
    pgm_usbasp = pgm_new();

    pgm_usbasp->initpgm = usbasp_initpgm;
    ladd(pgm_usbasp->id, dup_string("usbasp"));
    strcpy(pgm_usbasp->config_file, "./avrdude.conf");
    strcpy(pgm_usbasp->type, "usbasp");
    strcpy(pgm_usbasp->usbproduct, "USBasp");
    strcpy(pgm_usbasp->usbvendor, "www.fischl.de");
    strcpy(pgm_usbasp->desc, "USBasp, http://www.fischl.de/usbasp/");
    pgm_usbasp->exit_datahigh = EXIT_DATAHIGH_UNSPEC;
    pgm_usbasp->exit_reset = EXIT_RESET_UNSPEC;
    pgm_usbasp->exit_vcc = EXIT_VCC_UNSPEC;
    pgm_usbasp->conntype = CONNTYPE_USB;
    pgm_usbasp->baudrate = 0;
    pgm_usbasp->bitclock = 0;
    pgm_usbasp->usbpid = 1500;
    pgm_usbasp->usbvid = 5824;
    pgm_usbasp->lineno = 697;
    pgm_usbasp->ppictrl = 0;
    pgm_usbasp->ppidata = 0;
    pgm_usbasp->page_size = 0;
    pgm_usbasp->ispdelay = 0;
    pgm_usbasp->flag = '0';


    attiny_85 = avr_new_part();
    strcpy(attiny_85->config_file, "./avrdude.conf");
    attiny_85->lineno = 9005;

    attiny_85->avr910_devcode = 32;
    //attiny_85->bs2 = '\0';
    attiny_85->bytedelay = 0;
    attiny_85->chip_erase_delay = 4500;
    attiny_85->chiperasepolltimeout = 40;
    attiny_85->chiperasepulsewidth = 0;
    attiny_85->chiperasetime = 0;
    attiny_85->cmdexedelay = 25;
    //attiny_85->config_file = ./avrdude.conf";
    attiny_85->ctl_stack_type  = CTL_STACK_HVSP;
    strcpy(attiny_85->desc, "Attiny85");
    //attiny_85->controlstack = @0x7fd7ca8652ac;
    attiny_85->eecr = 0;
    //attiny_85->eeprom_instr = @0x7fd7ca8652cf;
    attiny_85->flags = 99;
    //attiny_85->flash_instr = @0x7fd7ca8652cc;
    attiny_85->hventerstabdelay = 100;
    attiny_85->hvleavestabdelay = 100;
    attiny_85->hvspcmdexedelay = 0;
    //attiny_85->id = t85;
    //attiny_85->idr = '\0';
    attiny_85->latchcycles = 1;
    attiny_85->lineno = 9005;
    attiny_85->mcu_base = 0;
    //attiny_85->mem =;
    attiny_85->nvm_base = 0;
    attiny_85->ocdrev = 1;
    //attiny_85->op = @0x7fd7ca865340;
    //attiny_85->pagel = '\0';
    attiny_85->pollindex = 3;
    attiny_85->pollmethod = 1;
    attiny_85->pollvalue = 'S';
    attiny_85->postdelay = 1;
    attiny_85->poweroffdelay = 25;
    attiny_85->predelay = 1;
    attiny_85->progmodedelay = 0;
    attiny_85->programfusepolltimeout = 25;
    attiny_85->programfusepulsewidth = 0;
    attiny_85->programlockpolltimeout = 25;
    attiny_85->programlockpulsewidth = 0;
    //attiny_85->rampz = '\0';
    attiny_85->reset_disposition = 1;
    attiny_85->resetdelay = 25;
    attiny_85->resetdelayms = 1;
    attiny_85->resetdelayus = 0;
    attiny_85->retry_pulse = 4;
    //attiny_85->signature = @0x7fd7ca86526e;
    //attiny_85->spmcr = '\0';
    attiny_85->stabdelay = 100;
    attiny_85->stk500_devcode = 20;
    attiny_85->synchcycles = 6;
    attiny_85->synchloops = 32;
    attiny_85->timeout = 200;
    attiny_85->togglevtg = 1;



    AVRMEM * mem_flash;
    mem_flash = avr_new_memtype();

    mem_flash->blocksize = 32;
    mem_flash->delay = 6;

    strcpy(mem_flash->desc, "flash");

    mem_flash->max_write_delay =	4500;//	int
    mem_flash->min_write_delay =	4500;//	int
    mem_flash->mode = 65;//	int
    mem_flash->num_pages = 128;//	int
    mem_flash->offset =	0;//	unsigned int
    mem_flash->page_size = 64;//	int
    mem_flash->paged = 1;//	int
    mem_flash->pollindex = 0;//	int
    mem_flash->pwroff_after_write =	0;//	int
    mem_flash->readsize	= 256;//	int
    mem_flash->size	= 8192;//	int
    mem_flash->readback[0] = 255;
    mem_flash->readback[1] = 255;

    ladd(attiny_85->mem, mem_flash);

    avr_initmem(attiny_85);



    if (pgm_usbasp->initpgm)
        {
            pgm_usbasp->initpgm(pgm_usbasp);
        }
    else
        {
            fprintf(stderr,
                    "\n%s: Can't initialize the programmer.\n\n",
                    progname);
            exit(1);
        }

    if (pgm_usbasp->setup)
        {
            pgm_usbasp->setup(pgm_usbasp);
        }

    rc = pgm_usbasp->open(pgm_usbasp, "usb");
    pgm_usbasp->enable(pgm_usbasp);

    init_ok = (rc = pgm_usbasp->initialize(pgm_usbasp, attiny_85)) >= 0;
    if (!init_ok)
        {
            fprintf(stderr, "%s: initialization failed, rc=%d\n", progname, rc);
            if (!ovsigck)
                {
                    fprintf(stderr, "%sDouble check connections and try again, "
                            "or use -F to override\n"
                            "%sthis check.\n\n",
                            progbuf, progbuf);
                    exitrc = 1;
                }
        }

    fprintf(stderr,
            "%s: AVR device initialized and ready to accept instructions\n",
            progname);



    fprintf(stderr,
            "%s: UPLOADD\n",
            progname);

    UPDATE write_avr;

    write_avr.filename = "/var/folders/x6/17svhgds0x7889jbwj0rb4y80000gp/T/build5373350735204642116.tmp/sendNotes.cpp.hex";
    write_avr.memtype  = "flash";
    write_avr.format = 2;
    write_avr.op = 1;

    //write_avr.filename = "/var/folders/x6/17svhgds0x7889jbwj0rb4y80000gp/T/build5373350735204642116.tmp/sendNotes.cpp.hex";
    //write_avr.memtype  = "flash";
    //write_avr.format = 2;
    //write_avr.op = 2; //verify

    do_op(pgm_usbasp,attiny_85, &write_avr, UF_NONE);

    pgm_usbasp->powerdown(pgm_usbasp);
    pgm_usbasp->disable(pgm_usbasp);
    pgm_usbasp->close(pgm_usbasp);

//  pgm->enable(pgm);





}

