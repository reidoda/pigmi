//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

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

class udp_server
{
public:
  udp_server(boost::asio::io_service& io_service)
    : socket_(io_service, udp::endpoint(udp::v4(), 4694))
  {
    start_receive();
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
      struct timeval now;
      double local_time;
      gettimeofday(&now, NULL);
      local_time = now.tv_sec + now.tv_usec / 1000000.0;
    if (!error || error == boost::asio::error::message_size)
    {
      recv_buffer_[17] = '\0';
      recv_buffer_[33] = '\0';
      
      double remote_time = std::atof(recv_buffer_.data()+0);
      std::string remote_ip = recv_buffer_.data()+18;
      boost::algorithm::erase_all(remote_ip, " ");
      double transit_time = local_time - remote_time;
      std::printf("transit_time: %f", transit_time);
      std::cout << ", remote_ip: " << remote_ip << std::endl;
      boost::shared_ptr<std::string> message(
          new std::string(make_timestamp()));

      socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
          boost::bind(&udp_server::handle_send, this, message,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      start_receive();
    }
  }

  void handle_send(boost::shared_ptr<std::string> /*message*/,
      const boost::system::error_code& /*error*/,
      std::size_t /*bytes_transferred*/)
  {
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 128> recv_buffer_;
};

int main()
{
  try
  {
    boost::asio::io_service io_service;
    udp_server server(io_service);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
