//
// PingSender.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include "PingSender.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: client <host> <local ip>" << std::endl;
        return 1;
    }

    std::string remote_ip = argv[1];
    std::string my_ip = argv[2];
    PingSender pingsender(my_ip, remote_ip);
    pingsender.send_ping();
    return 0;
}
