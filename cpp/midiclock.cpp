// midiout.cpp
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "rtmidi/RtMidi.h"
#include <unistd.h>
#include <sys/time.h>
#include <cmath>
#include <csignal>

enum class MidiClockState {stop, stopped, start, running};

class MidiClock
{
    public:
        MidiClock(double the_tempo = 120.0)
        {
            tempo = the_tempo;
            midi_clock.push_back(248);
            midi_start.push_back(250);
            midi_stop.push_back(252);
            beat_period = (60.0/tempo);
            clock_period = beat_period/24.0;
            midiout = new RtMidiOut();
        }

        void cleanup()
        {
            std::cout << "Cleaning up" << std::endl;
            midiout->sendMessage(&midi_stop);
            delete midiout;
        }

        void run()
        {
            double start_time_in_sec;
            double now_in_sec ;

            // Check available ports.
            unsigned int nPorts = midiout->getPortCount();
            if ( nPorts == 0 ) {
                std::cout << "No ports available!\n";
            }
            // Open port b 
            midiout->openPort( 2 );
            midiout->sendMessage(&midi_stop);
            uint64_t i = 1;
            bool initialized = false;
            bool start_sent = false;
            double threshold = 0.0001;
            int sleep_period_in_micros = 50;

            now_in_sec = this->now(); 
            double midi_start_time = (floor(now_in_sec / (beat_period * 4)) + 1) * (beat_period * 4);

            while(true)
            {
                if (!initialized)
                {
                    start_time_in_sec = this->now(); 
                    initialized = true;
                }

                now_in_sec = this->now(); 

                if (now_in_sec > midi_start_time - threshold && !start_sent)
                {
                    midiout->sendMessage(&midi_start);
                    start_sent = true;
                }
                if (now_in_sec > start_time_in_sec + (i * clock_period) - threshold)
                {
                    printf("%f\n",now_in_sec);
                    midiout->sendMessage(&midi_clock);
                    while(now_in_sec > start_time_in_sec + (i * clock_period) - threshold) {i++;;}
                }
                usleep(sleep_period_in_micros);
            }
        }

        double tempo;
        MidiClockState state = MidiClockState::stopped;

    private:

        double now()
        {
            struct timeval now;
            gettimeofday(&now,NULL);
            return now.tv_sec + now.tv_usec / 1000000.0;
        }

        RtMidiOut *midiout = new RtMidiOut();
        double beat_period;
        double clock_period;
        std::vector<unsigned char> midi_clock;         
        std::vector<unsigned char> midi_start;         
        std::vector<unsigned char> midi_stop;         
};

///////////////////////////////////////////////////

// this must be global to allow for proper cleanup
MidiClock * midiclock = new MidiClock();

void cleanup(int s)
{
    printf("Caught signal %d\n", s);
    if (s == 2)
    {
        midiclock->cleanup();
        exit(s);
    }
}

int main( int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: midiclock <tempo>" << std::endl;
        return 0;
    }
    double tempo = atof(argv[1]);
    signal(SIGINT,cleanup);
    midiclock->run();
    return 0;
}
