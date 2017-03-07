#ifndef MIDICLOCK_H
#define MIDICLOCK_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "rtmidi/RtMidi.h"
#include <unistd.h>
#include <sys/time.h>
#include <cmath>
#include <csignal>
#include <wiringPi.h>

enum class MidiClockState {stop, stopped, start, running};

class MidiClock
{
    public:
        MidiClock(double the_tempo = 120.0);
        void cleanup();
        void run();
        double get_tempo();
        void set_tempo(double new_tempo);
        MidiClockState get_clock_state();
        void set_clock_state(MidiClockState new_state); 

    private:

        double tempo;
        MidiClockState state;
        double now();
        RtMidiOut *midiout = new RtMidiOut();
        double beat_period;
        double clock_period;
        std::vector<unsigned char> midi_clock;         
        std::vector<unsigned char> midi_start;         
        std::vector<unsigned char> midi_stop;         
};

#endif
