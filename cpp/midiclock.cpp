// midiout.cpp
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "rtmidi/RtMidi.h"
#include <unistd.h>
#include <sys/time.h>
#include <cmath>

/*void cleanup(int s, RtMidiOut * midiout)
{
    printf("Caught signal %d\n", s);
    if (s == 2)
    {

}*/

int main()
{
  RtMidiOut *midiout = new RtMidiOut();
  std::vector<unsigned char> clock;
  std::vector<unsigned char> start;
  std::vector<unsigned char> stop;
  clock.push_back(248);
  start.push_back(250);
  stop.push_back(252);
  double tempo = 120.0;
  double beat_period = (60.0/tempo);
  double clock_period = beat_period/24.0;
  //printf("period: %f\n",period);

  struct timeval start_time;
  double start_time_in_sec;
  struct timeval now;
  double now_in_sec ;

  // Check available ports.
  unsigned int nPorts = midiout->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    //goto cleanup;
  }
  // Open port b 
  midiout->openPort( 2 );
  //midiout->sendMessage( &start );
  uint64_t i = 1;
  bool initialized = false;
  bool start_sent = false;
  double threshold = 0.0001;
  int sleep_period_in_micros = 50;


  gettimeofday(&now,NULL);
  now_in_sec = now.tv_sec + now.tv_usec / 1000000.0;
  double midi_start_time = (floor(now_in_sec / (beat_period * 4)) + 1) * (beat_period * 4);
 
  while(true)
  {
    if (!initialized)
    {
        gettimeofday(&start_time,NULL);
        start_time_in_sec = start_time.tv_sec + start_time.tv_usec / 1000000.0;
        initialized = true;
    }

    gettimeofday(&now,NULL);
    now_in_sec = now.tv_sec + now.tv_usec / 1000000.0;

    if (now_in_sec > midi_start_time - threshold && !start_sent)
    {
        midiout->sendMessage(&start);
        start_sent = true;
    }
    if (now_in_sec > start_time_in_sec + (i * clock_period) - threshold)
    {
        printf("%f\n",now_in_sec);
        midiout->sendMessage(&clock);
        while(now_in_sec > start_time_in_sec + (i * clock_period) - threshold) {i++;;}
    }
    usleep(sleep_period_in_micros);
  }
  // Clean up
  //cleanup:
  //std::cout << "Cleaning up" << std::endl;
  // delete midiout;
 return 0;
}
