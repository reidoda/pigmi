// midiout.cpp
#include <thread>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "rtmidi/RtMidi.h"
#include <unistd.h>
int main()
{
  int note_numbers[] = {36, 38, 43, 50, 42, 46, 39};
  RtMidiOut *midiout = new RtMidiOut();
  std::vector<unsigned char> message;
  // Check available ports.
  unsigned int nPorts = midiout->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }
  // Open first available port.
  midiout->openPort( 2 );
  // Send out a series of MIDI messages.
  // Program change: 192, 5
//  message.push_back( 192 );
//  message.push_back( 5 );
//  midiout->sendMessage( &message );
  // Control Change: 176, 7, 100 (volume)
//  message[0] = 176;
//  message[1] = 7;
//  message.push_back( 100 );
//  midiout->sendMessage( &message );
  // Note On: 144, 36, 90
  message.push_back(0);
  message.push_back(0);
  message.push_back(0);
  for (int i = 0; i < 7; i++)
  {

  message[0] = 153;
  message[1] = note_numbers[i];
  message[2] = 90;
  midiout->sendMessage( &message );
  printf("Sending note number %i\n",note_numbers[i]);
  usleep(50000); // Platform-dependent ... see example in tests directory.
  // Note Off: 128, 36, 40
  //message[0] = 128;
  //message[1] = 36;
  //message[2] = 40;
  //midiout->sendMessage( &message );
  //std::cout << "Sending note off" << std::endl;
  }
  // Clean up
 cleanup:
  std::cout << "Cleaning up" << std::endl;
  delete midiout;
  return 0;
}
