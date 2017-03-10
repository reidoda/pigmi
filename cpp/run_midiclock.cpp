#include <iostream>
#include <cstdio>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "MidiClock.hpp"
#include "PigmiLogger.hpp"
#include "PingSender.hpp"

/* These are global to allow for proper cleanup */
MidiClock * midiclock = new MidiClock();
boost::asio::io_service io_service;

/* This function cleans stuff up */
void cleanup(int s)
{
    printf("Caught signal %d\n", s);
    if (s == 2)
    {
        midiclock->cleanup();
        io_service.stop(); 
        exit(s);
    }
}

int main( int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: run_midiclock <tempo> <my public ip> <remote ip>" << std::endl;
        std::cout << "To determine your public ip go to https://www.whatismyip.com/" << std::endl;
        return 0;
    }

    double tempo = atof(argv[1]);
    std::string public_ip = argv[2];
    std::string remote_ip = argv[3];
    signal(SIGINT,cleanup);

    PigmiLogger server(io_service, public_ip);
    boost::thread bt(boost::bind(&boost::asio::io_service::run, &io_service));

    PingSender ping_sender(public_ip, remote_ip);
    boost::thread ping_thread(boost::bind(&PingSender::run, &ping_sender));

    midiclock->set_tempo(tempo);
    midiclock->run();
    return 0;
}
