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

    std::cerr << ">> sending kick sysex message!\n";

    message.push_back( 0xF0 );
    message.push_back( 0x08 );
    message.push_back( 0xF7 );

    midiout->sendMessage( &message );
    std::cerr << ">> message sent, wait 1.5 secs\n";
    SLEEP(1500);

}

BabyGnusbSysexCommander::~BabyGnusbSysexCommander()
{
    delete(midiout);
}

bool BabyGnusbSysexCommander::detect()
{
   std::cerr << ">> sysex detect\n";

   midiout = new RtMidiOut();
   std::string portName;

   unsigned int nPorts = midiout->getPortCount();

    for ( unsigned int i=0; i<nPorts; i++ ) {

      portName = midiout->getPortName(i);
     std::cerr << "  Midi port #" << i << ": " << portName << '\n';

      if (portName == "CocoMidi")
      {
        std::cout << ">> CocoMidi detected.. connecting to midi port!\n";
        midiout->openPort( i );
        return true;
      }
    }
    return false;
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

EXPORT_C int babygnusbuino_sysex_detect(BabyGnusbSysexCommander* _this)
{
    if (_this->detect())
    {
        return 1;
    }else{
        return 0;
    }
}

EXPORT_C void babygnusbuino_sysex_send_reset(BabyGnusbSysexCommander* _this)
{
    _this->reset();
}
