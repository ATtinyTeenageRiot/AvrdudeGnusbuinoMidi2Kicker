//*****************************************//
//  sysextest.cpp
//  by Gary Scavone, 2003-2005.
//
//  Simple program to test MIDI sysex sending and receiving.
//
//*****************************************//

#include "sysextest.h"


void BabyGnusbSysexCommander::reset()
{
    std::vector<unsigned char> message;

  bool detected = false;

  midiout = new RtMidiOut();

  std::string portName;
  unsigned int i = 0, 

  nPorts = midiout->getPortCount();
  
  for ( i=0; i<nPorts; i++ ) {
    std::cout << "wewkokvisa\n";
    portName = midiout->getPortName(i);
    std::cout << "  Output port #" << i << ": " << portName << '\n';



    if (portName == "babygnusbuino")
    {
      std::cout << "babygnusbuino detected.. connect to midi port!\n";

      midiout->openPort( i );
      detected = true;
      std::cout << "gnusbuino\n";
    } 
  }


  if (detected)
  {
    std::cout << "babygnusbuino detected.. send sysex message kick!\n";

    message.push_back( 0xF0 );
    message.push_back( 0x08 );
    message.push_back( 0xF7 );

    midiout->sendMessage( &message );
    SLEEP(1000);

  }else{
    std::cout << "babygnusbuino not detected\n";
  }

  delete(midiout);
}

  // access functions
EXPORT_C BabyGnusbSysexCommander* some_class_new(void)
{
    return new BabyGnusbSysexCommander();
}

EXPORT_C void some_class_delete(BabyGnusbSysexCommander* _this)
{
    delete _this;
}

EXPORT_C void some_class_some_method(BabyGnusbSysexCommander* _this)
{
    _this->reset();
}

// void usage( void ) {
//   std::cout << "\nuseage: sysextest N\n";
//   std::cout << "    where N = length of sysex message to send / receive.\n\n";
//   exit( 0 );
// }



// // This function should be embedded in a try/catch block in case of
// // an exception.  It offers the user a choice of MIDI ports to open.
// // It returns false if there are no ports available.
// bool chooseMidiPort( RtMidi *rtmidi );

// void mycallback( double deltatime, std::vector< unsigned char > *message, void * /*userData*/ )
// {
//   unsigned int nBytes = message->size();
//   for ( unsigned int i=0; i<nBytes; i++ )
//     std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
//   if ( nBytes > 0 )
//     std::cout << "stamp = " << deltatime << std::endl;
// }

// int main( int argc, char *argv[] )
// {
//   RtMidiOut *midiout = 0;
//   Rtmidiout *midiout = 0;
//   std::vector<unsigned char> message;
//   unsigned int i, nBytes;

//   // Minimal command-line check.
//   if ( argc != 2 ) usage();
//   nBytes = (unsigned int) atoi( argv[1] );

//   // RtMidiOut and Rtmidiout constructors
//   try {
//     midiout = new RtMidiOut();
//     midiout = new Rtmidiout();
//   }
//   catch ( RtMidiError &error ) {
//     error.printMessage();
//     goto cleanup;
//   }

//   // Don't ignore sysex, timing, or active sensing messages.
//   midiout->ignoreTypes( false, true, true );

//   try {
//     if ( chooseMidiPort( midiout ) == false ) goto cleanup;
//     if ( chooseMidiPort( midiout ) == false ) goto cleanup;
//   }
//   catch ( RtMidiError &error ) {
//     error.printMessage();
//     goto cleanup;
//   }

//   midiout->setCallback( &mycallback );

//   message.push_back( 0xF6 );
//   midiout->sendMessage( &message );
//   SLEEP( 500 ); // pause a little

//   // Create a long sysex message of numbered bytes and send it out ... twice.
//   for ( int n=0; n<2; n++ ) {
//     message.clear();
//     message.push_back( 240 );
//     for ( i=0; i<nBytes; i++ )
//       message.push_back( i % 128 );
//     message.push_back( 247 );
//     midiout->sendMessage( &message );

//     SLEEP( 500 ); // pause a little
//   }

//   // Clean up
//  cleanup:
//   delete midiout;
//   delete midiout;

//   return 0;
// }

// bool chooseMidiPort( RtMidi *rtmidi )
// {
//   bool isInput = false;
//   if ( typeid( *rtmidi ) == typeid( Rtmidiout ) )
//     isInput = true;

//   if ( isInput )
//     std::cout << "\nWould you like to open a virtual input port? [y/N] ";
//   else
//     std::cout << "\nWould you like to open a virtual output port? [y/N] ";

//   std::string keyHit;
//   std::getline( std::cin, keyHit );
//   if ( keyHit == "y" ) {
//     rtmidi->openVirtualPort();
//     return true;
//   }

//   std::string portName;
//   unsigned int i = 0, nPorts = rtmidi->getPortCount();
//   if ( nPorts == 0 ) {
//     if ( isInput )
//       std::cout << "No input ports available!" << std::endl;
//     else
//       std::cout << "No output ports available!" << std::endl;
//     return false;
//   }

//   if ( nPorts == 1 ) {
//     std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
//   }
//   else {
//     for ( i=0; i<nPorts; i++ ) {
//       portName = rtmidi->getPortName(i);
//       if ( isInput )
//         std::cout << "  Input port #" << i << ": " << portName << '\n';
//       else
//         std::cout << "  Output port #" << i << ": " << portName << '\n';
//     }

//     do {
//       std::cout << "\nChoose a port number: ";
//       std::cin >> i;
//     } while ( i >= nPorts );
//   }

//   std::cout << std::endl;
//   rtmidi->openPort( i );

//   return true;
// }
