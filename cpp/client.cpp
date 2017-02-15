//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_timestamp()
{
  // using namespace std; // For time_t, time and ctime;
  // time_t now = time(0);
  // return ctime(&now);
  struct timeval now;
  long double now_in_seconds;
  gettimeofday(&now, NULL);
  now_in_seconds = now.tv_sec + now.tv_usec / 1000000.0;
  return std::to_string(now_in_seconds);
}

std:: string pad_ip(std::string ip)
{
    std::string padding = " ";
    for (int i=0; i < 15 - ip.length(); i++)
    {
        padding = padding + " ";
    }
    std::cout << "\"" << padding + ip << "\"" << std::endl;
    return padding + ip;
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <local ip>" << std::endl;
      return 1;
    }
    
    std::string my_ip = argv[2];
    std::string outgoing_msg;

    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), argv[1], "4694");
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    udp::socket socket(io_service);
    socket.open(udp::v4());

    // boost::array<char, 3> send_buf  = {{ 0 }};
    outgoing_msg = make_timestamp() + pad_ip(my_ip); 
    socket.send_to(boost::asio::buffer(outgoing_msg), receiver_endpoint);

    boost::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(
        boost::asio::buffer(recv_buf), sender_endpoint);

    std::cout.write(recv_buf.data(), len);
    std::cout << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
