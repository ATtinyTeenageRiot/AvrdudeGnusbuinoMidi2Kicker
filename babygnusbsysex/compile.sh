rm -fr *.o
g++ -O3 -Wall -Wextra -I.. -I../include -D__MACOSX_CORE__ -c sysextest.cpp 
gcc -c -o test.o test.c
gcc -o test test.o sysextest.o Release/RtMidi.o -framework CoreMIDI -framework CoreFoundation -framework CoreAudio -lstdc++
ar -cvq libgnusb.a *.o ./Release/RtMidi.o