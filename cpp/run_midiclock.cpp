#include <iostream>
#include "MidiClock.hpp"
#include <cstdio>

/* This must be global to allow for proper cleanup*/
MidiClock * midiclock = new MidiClock();

/* This function cleans stuff up */
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
    midiclock->set_tempo(tempo);
    midiclock->run();
    return 0;
}
