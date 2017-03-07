#include "MidiClock.hpp"

MidiClock::MidiClock(double the_tempo)
{
    set_tempo(the_tempo);
    midi_clock.push_back(248);
    midi_start.push_back(250);
    midi_stop.push_back(252);
    beat_period = (60.0/tempo);
    clock_period = beat_period/24.0;
    midiout = new RtMidiOut();
    wiringPiSetup();
    pinMode(5, OUTPUT);
}

void MidiClock::cleanup()
{
    std::cout << "Cleaning up" << std::endl;
    midiout->sendMessage(&midi_stop);
    delete midiout;
}

void MidiClock::run()
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
    int sleep_period_in_micros = 1;
    double next_tick_time;

    now_in_sec = this->now(); 
    double midi_start_time = (floor(now_in_sec / (beat_period * 4)) + 1) * (beat_period * 4);

    while(true)
    {
        if (!initialized)
        {
            start_time_in_sec = this->now(); 
            midiout->sendMessage(&midi_clock);
            midiout->sendMessage(&midi_stop);
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
            midiout->sendMessage(&midi_clock);
            i++;
            // while(now_in_sec > start_time_in_sec + (i * clock_period) - threshold) {i++;;}
        }

        next_tick_time = (floor(now_in_sec / beat_period ) + 1) * beat_period;
        if (now_in_sec > next_tick_time - threshold)
        {
            digitalWrite(5,HIGH);
            digitalWrite(5,LOW);
        }

        usleep(sleep_period_in_micros);
    }
}

double MidiClock::get_tempo() { return tempo; }

void MidiClock::set_tempo(double new_tempo) 
{ 
    tempo = new_tempo; 
    beat_period = (60.0/tempo);
    clock_period = beat_period/24.0;
}

MidiClockState MidiClock::get_clock_state() { return state; }

void MidiClock::set_clock_state(MidiClockState new_state) { state = new_state; }



double MidiClock::now()
{
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec + now.tv_usec / 1000000.0;
}

