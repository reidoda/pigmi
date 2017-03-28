//
// PigmiLogger.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2017 Reid Oda
// Based on the Boost server example by Christopher M. Kohlhoff (chris at kohlhoff dot com)
//

#include "PigmiLogger.hpp"


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

PigmiLogger::PigmiLogger(boost::asio::io_service& io_service, std::string the_local_ip)
    : socket_(io_service, udp::endpoint(udp::v4(), 4694)) 
{
    start_receive();
    std::string log_path = "/home/pi/pigmi/log/" + make_timestamp(Precision::sec) + ".log";
    log_file.open(log_path.c_str());
    local_ip = the_local_ip;
}


/* This function returns a timestamp string in either seconds or nanoseconds */
std::string PigmiLogger::make_timestamp(Precision precision)
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

/* This function runs a shell command <cmd> and returns the 
   output as a standard string (from stackoverflow) */
std::string PigmiLogger::exec(const char* cmd) 
{
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
void PigmiLogger::start_receive()
{
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&PigmiLogger::handle_receive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void PigmiLogger::handle_receive(const boost::system::error_code& error,
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
        std::printf("transit time from remote_ip to me: %f", transit_time);
        std::cout << ", remote_ip: " << remote_ip << std::endl;
        log_file << boost::str(
                boost::format("PING, %f, %s, %s, %d\n") % local_time % local_ip % remote_ip % transit_time);
        if (ping_counter++ % ntpq_freq == 0)
            log_file << boost::str(
                    boost::format("NTPQ, %f, %s") % local_time % exec("ntpq -p | sed -n 3p"));
        log_file.flush();
        boost::shared_ptr<std::string> message(
                new std::string(make_timestamp()));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                boost::bind(&PigmiLogger::handle_send, this, message,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
}

void PigmiLogger::handle_send(boost::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& /*error*/,
        std::size_t /*bytes_transferred*/)
{
}

