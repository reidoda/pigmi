
run_midiclock:
	g++-4.8 -O3 -std=c++11 run_midiclock.cpp MidiClock.cpp -lrtmidi -lrt -lwiringPi -o ../bin/run_midiclock 

midiclock-old:
	g++-4.8 -O3 -std=c++11 midiclock-old.cpp -lrtmidi -lrt -lwiringPi -o ../bin/midiclock-old

testgps:
	g++-4.8 -std=c++11 testgps.cpp -o ../bin/testgps

run_pigmi_logger:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/run_pigmi_logger -L /usr/lib PigmiLogger.cpp run_pigmi_logger.cpp -lboost_system -lpthread -lboost_thread

run_ping_sender:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/run_ping_sender -L /usr/lib run_ping_sender.cpp PingSender.cpp -lboost_system -lpthread 

blink:
	gcc -Wall -o ../bin/blink blink.c -lwiringPi

clean:
	rm -f ../bin/* 
