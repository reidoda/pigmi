//
// PingSender.hpp
// ~~~~~~~~~~
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp> 
#include <boost/thread.hpp>

using boost::asio::ip::udp;

class PingSender
{
    public:
        //PingSender(std::string my_ip, std::Vector<std::string> remote_ips)
        PingSender(std::string new_my_ip, std::string new_remote_ip);
        std::string make_timestamp();
        std::string pad_ip(std::string ip);
        void send_ping();
        void run();

    private:
        double now();
        std::string my_ip;
        std::string remote_ip;
        std::string port_num = "4694";
};
