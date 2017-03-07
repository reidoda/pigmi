//
// PigmiLogger.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2017 Reid Oda
// Based on the Boost server example by Christopher M. Kohlhoff (chris at kohlhoff dot com)
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

class PigmiLogger
{
    public:
        PigmiLogger(boost::asio::io_service& io_service, std::string the_local_ip);

    private:
        /* This function returns a timestamp string in either seconds or nanoseconds */
        std::string make_timestamp(Precision precision = Precision::nano);

        /* This function runs a shell command <cmd> and returns the 
           output as a standard string (from stackoverflow) */
        std::string exec(const char* cmd);
        void start_receive();

        void handle_receive(const boost::system::error_code& error,
                std::size_t /*bytes_transferred*/);

        void handle_send(boost::shared_ptr<std::string> /*message*/,
                const boost::system::error_code& /*error*/,
                std::size_t /*bytes_transferred*/);

        udp::socket socket_;
        udp::endpoint remote_endpoint_;
        boost::array<char, 128> recv_buffer_;
        std::ofstream log_file;
        std::string local_ip;
};
