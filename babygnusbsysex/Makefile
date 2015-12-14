
# g++ -O3 -Wall -Wextra -I.. -I../include -D__MACOSX_CORE__ -c sysextest.cpp 
# gcc -c -o test.o test.c
# gcc -o test test.o sysextest.o Release/RtMidi.o -framework CoreMIDI -framework CoreFoundation -framework CoreAudio -lstdc++



PROGRAMS = babygnusbsysex

CC	   = g++

CFLAGS   = -O3 -Wall -Wextra
CFLAGS  += -I$(INCLUDE) -I$(INCLUDE)/include
RM = /bin/rm
SRC_PATH = ../rtmidi-2.1.0
INCLUDE = ../rtmidi-2.1.0
OBJECT_PATH = Release
OBJECTS =   RtMidi.o

vpath %.o $(OBJECT_PATH)

ifeq ($(OS),Windows_NT)
	CCFLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CCFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CCFLAGS += -D IA32
		DEFS= -D__WINDOWS_MM__	  
		LIBRARY  = -lwinmm																																			  
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CCFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		DEFS	 =   -D__MACOSX_CORE__
		LIBRARY  = -framework CoreMIDI -framework CoreFoundation -framework CoreAudio
	endif
endif

%.o : $(SRC_PATH)/%.cpp
	$(CC) $(CFLAGS) $(DEFS) -c $(<) -o $(OBJECT_PATH)/$@

all : $(PROGRAMS)

babygnusbsysex : babygnusbsysex.cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(DEFS) -c babygnusbsysex.cpp

clean : 
	$(RM) -f $(OBJECT_PATH)/*.o
	$(RM) -f $(PROGRAMS) *.exe
	$(RM) -f *~
	$(RM) -fR *.dSYM

distclean: clean
	$(RM) -f Makefile

strip : 
	strip $(PROGRAMS)