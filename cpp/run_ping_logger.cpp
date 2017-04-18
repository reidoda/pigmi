#include <iostream>
#include <cstdio>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "PigmiLogger.hpp"
#include "PingSender.hpp"

// Must be global to cleanup in case of ctrl-c
boost::asio::io_service io_service;

/* This function cleans stuff up */
void cleanup(int s)
{   
    printf("Caught signal %d\n", s);
    if (s == 2)
    {   
        io_service.stop();
        exit(s);
    }
}

int main( int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: run_ping_logger <my public ip> <remote ip>" << std::endl;
        std::cout << "To determine your public ip go to https://www.whatismyip.com/" << std::endl;
        return 0;
    }

    std::string public_ip = argv[1];
    std::string remote_ip = argv[2];
    signal(SIGINT,cleanup);

    PigmiLogger server(io_service, public_ip);
    boost::thread bt(boost::bind(&boost::asio::io_service::run, &io_service));

    PingSender ping_sender(public_ip, remote_ip);
    boost::thread ping_thread(boost::bind(&PingSender::run, &ping_sender));
    
    int i;
    std::cout << "Hit return to stop\n" << std::endl;
    std::cin >> i;

    return 0;
}
