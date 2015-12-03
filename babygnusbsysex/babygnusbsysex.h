

#ifdef __cplusplus // only actually define the class if this is C++

#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif



class BabyGnusbSysexCommander
{
private:
  RtMidiOut *midiout;
public:
  BabyGnusbSysexCommander()
  {

  }
  void reset();
};


#else

// C doesn't know about classes, just say it's a struct
typedef struct BabyGnusbSysexCommander BabyGnusbSysexCommander; 

#endif


// access functions
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif


EXPORT_C BabyGnusbSysexCommander* some_class_new(void);
EXPORT_C void some_class_delete(BabyGnusbSysexCommander*);
EXPORT_C void some_class_some_method(BabyGnusbSysexCommander*);
