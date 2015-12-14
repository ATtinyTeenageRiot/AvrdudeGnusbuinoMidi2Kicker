//*****************************************//
//  sysextest.cpp
//  by Gary Scavone, 2003-2005.
//
//  Simple program to test MIDI sysex sending and receiving.
//
//*****************************************//

#include "babygnusbsysex.h"


void BabyGnusbSysexCommander::reset()
{
    std::vector<unsigned char> message;

  bool detected = false;

  midiout = new RtMidiOut();

  std::string portName;
  unsigned int i = 0, 

  nPorts = midiout->getPortCount();
  
  for ( i=0; i<nPorts; i++ ) {

    portName = midiout->getPortName(i);
    std::cout << "  Output port #" << i << ": " << portName << '\n';



    if (portName == "babygnusbuino")
    {
      std::cout << "babygnusbuino detected.. connecting to midi port!\n";

      midiout->openPort( i );
      detected = true;
      std::cout << "gnusbuino\n";
    } 
  }


  if (detected)
  {
    std::cout << "sending kick sysex message!\n";

    message.push_back( 0xF0 );
    message.push_back( 0x08 );
    message.push_back( 0xF7 );

    midiout->sendMessage( &message );
    std::cout << "message sent, wait 1.5 secs\n";
    SLEEP(1500);

  }else{
    std::cout << "babygnusbuino not detected\n";
  }

}

BabyGnusbSysexCommander::~BabyGnusbSysexCommander()
{
    delete(midiout);
}


  // access functions
EXPORT_C BabyGnusbSysexCommander* babygnusbuino_sysex_create(void)
{
    return new BabyGnusbSysexCommander();
}

EXPORT_C void babygnusbuino_sysex_delete(BabyGnusbSysexCommander* _this)
{
    delete _this;
}

EXPORT_C void babygnusbuino_sysex_send_reset(BabyGnusbSysexCommander* _this)
{
    _this->reset();
}
