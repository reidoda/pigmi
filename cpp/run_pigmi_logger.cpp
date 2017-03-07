//
// run_pigmi_logger.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2017 Reid Oda
// Based on the Boost server example by Christopher M. Kohlhoff (chris at kohlhoff dot com)
//

#include "PigmiLogger.hpp"
//#include <ctime>
#include <iostream>
#include <string>
//#include <fstream>
//#include <boost/array.hpp>
#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/format.hpp>
#include <boost/thread.hpp>



int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    if (argc != 2)
    {
        std::cerr << "Usage: server <public ip>" << std::endl;
        return 1;
    }

    PigmiLogger server(io_service, argv[1]);
    boost::thread bt(boost::bind(&boost::asio::io_service::run, &io_service));

    std::cout << "Press enter to stop\n" << std::endl;
    std::string request;
    std::getline(std::cin, request);

    io_service.stop(); 
    return 0;
}

