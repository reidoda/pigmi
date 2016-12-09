#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/time.h>
#include <regex>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>

std::string exec(const char* cmd, time_t wait_time=20) 
{
    struct timeval start_time;
    struct timeval now;
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    gettimeofday(&start_time,NULL);
    gettimeofday(&now,NULL);
    while (!feof(pipe.get()) && now.tv_sec < (start_time.tv_sec + wait_time))
    {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
        gettimeofday(&now,NULL);
    }
    return result;
}

int main()
{
    if(0 != geteuid()) 
    { 
        std::cerr << "This utility must be run as root. Type 'sudo ./testgps' (assuming you're calling testgps from the current directory).\n"; 
        exit(0);
    }

    std::string version = "1.0";
    printf("testgps version: %s\n", version.c_str());

    int num_problems = 0;
    std::string pps_source, pps_test_result, ntp_result;
    int ntp_reach;
    float ntp_offset, ntp_jitter;
    std::string pps_command = "ppstest /dev/";
    pps_source = exec("dmesg | grep -i pps | grep -i irq | awk '{print $4}' | sed 's/\\://'");
    printf("### Testing communication between operating system and GPS sensor. ###\n");
    printf("PPS source at %s",pps_source.c_str());

    pps_command += pps_source;
    pps_test_result = exec(pps_command.c_str(),3);
    printf("%s\n",pps_test_result.c_str());
    std::regex unable_to_open("unable to open",std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::regex timed_out("connection timed out",std::regex_constants::ECMAScript | std::regex_constants::icase);
    if (std::regex_search(pps_test_result, unable_to_open))
    {
        //Output something, but what? 
        exit(0);
    }
    else if (std::regex_search(pps_test_result, timed_out))
    {
        std::cout << "Make sure you have the proper pin designated in /boot/config.txt\n";
        exit(0);
    }
    else
    {
        std::cout << "Check the above output for error messages. If you see a series of 'sequence' messages and no obvious errors we're most likely fine.\n\n";
    }

    ntp_result = exec("ntpq -p");
    printf("### Testing communication between NTP and GPS PPS signal ###\n");
    printf("%s\n", ntp_result.c_str());
    std::string::size_type sz;

    ntp_reach = std::stoi(exec("ntpq -p | grep -i PPS\\( | awk '{print $7}'"));
    if (ntp_reach < 377)
    {
        printf("GPS strength is %.2f, which is not enough. Check the connectivity light on the Raspberry Pi.\n", 
                float(ntp_reach) / 377.0 * 100);
        num_problems++;
    } else {
        printf("GPS NTP reach at full strength\n");
    }

    ntp_offset = std::stof(exec("ntpq -p | grep -i PPS\\( | awk '{print $9}'"));
    if (abs(ntp_offset) > 1.0)
    {
        printf("The system clock is %f ms off from the GPS signal. Wait a little while and see if it improves.\n",
                ntp_offset);
        num_problems++;
    } else {
        printf("System clock properly disciplined by GPS/PPS signal\n");
    }

    ntp_jitter = std::stof(exec("ntpq -p | grep -i PPS\\( | awk '{print $10}'"));
    if(abs(ntp_jitter) > 1.0)
    {
        printf("The system time is experiencing high jitter of %f. This probably doesn't matter, but keep an eye on it.\n",
                ntp_jitter);
        num_problems++;
    } else {
        printf("Jitter is low (this is good).\n");
    }

    printf("There were %i problems in total (that this utility could detect).\n", num_problems);

    return 0;
}

