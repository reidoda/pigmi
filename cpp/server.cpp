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
#include <fstream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>


using boost::asio::ip::udp;

enum class Precision {nano, sec};

/* creates a timestamp string in either seconds or nanoseconds */
std::string make_timestamp(Precision precision = Precision::nano)
{
    std::string return_string;
    struct timeval now;
    long double now_in_seconds;
    gettimeofday(&now, NULL);
    now_in_seconds = now.tv_sec;
    if (precision == Precision::nano)
    {
        now_in_seconds += now.tv_usec / 1000000.0;
        return_string = std::to_string(now_in_seconds);
    } else
    {
        return_string = std::to_string(int(now_in_seconds));
    }
    return return_string;
}

/* runs a shell command and returns the output as a standard string (from stackoverflow) */
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

/* This class waits for UDP messages from other PIGMIS. Each message contains a timestamp 
   in Unix seconds and the IP of the remote PIGMI. Each message is 33 chars long. The 
   timestamp is 17 characters long (e.g. 487368056.197201), followed by a space, followed 
   by optional spaces (i.e. to pad an IP that is shorter than 15 characters), followed by 
   the remote ip. Here's are some example messages:

   1487368056.197201 192.168.205.101
   1487368533.585372  192.168.55.201
   1487369915.484713       45.32.1.4

   When a message is received it is appended to the file log_file. 

   It also periodically checks the GPS status of and writes the appropriate line of 'ntpq -p' 
   to the log file.
*/

class pigme_logger
{
    public:
        pigme_logger(boost::asio::io_service& io_service, std::string the_local_ip)
            : socket_(io_service, udp::endpoint(udp::v4(), 4694))
        {
            start_receive();
            std::string log_path = "../log/" + make_timestamp(Precision::sec) + ".log";
            log_file.open(log_path.c_str());
            local_ip = the_local_ip;
        }

    private:
        void start_receive()
        {
            std::cout << "start_receive" << std::endl;
            socket_.async_receive_from(
                    boost::asio::buffer(recv_buffer_), remote_endpoint_,
                    boost::bind(&pigme_logger::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        void handle_receive(const boost::system::error_code& error,
                std::size_t /*bytes_transferred*/)
        {
            std::cout << "handle_receive" << std::endl;
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
                log_file << boost::str(
                        boost::format("PING, %f, %s, %s, %d\n") % local_time % local_ip % remote_ip % transit_time);
                log_file.flush();
                boost::shared_ptr<std::string> message(
                        new std::string(make_timestamp()));

                socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                        boost::bind(&pigme_logger::handle_send, this, message,
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
        std::ofstream log_file;
        std::string local_ip;
};

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    if (argc != 2)
    {
        std::cerr << "Usage: server <public ip>" << std::endl;
        return 1;
    }

    pigme_logger server(io_service, argv[1]);
    boost::thread bt(boost::bind(&boost::asio::io_service::run, &io_service));

    std::cout << "Press enter to stop\n" << std::endl;
    std::string request;
    std::getline(std::cin, request);
 
    io_service.stop(); 
    return 0;
}

