CC = g++-4.8
CFLAGS = -O3 -std=c++11


run_midiclock: PigmiLogger.o MidiClock.o PingSender.o run_midiclock.cpp
	 $(CC) $(CFLAGS) -I /usr/include/boost -L /usr/lib -lwiringPi -lrtmidi -lrt -lpthread -lboost_system -lboost_thread -o ../bin/run_midiclock MidiClock.o PigmiLogger.o PingSender.o run_midiclock.cpp

PingSender.o: PingSender.cpp PingSender.hpp
	$(CC) $(CFLAGS) -c PingSender.cpp -I /usr/include/boost -L /usr/lib -lboost_system -lpthread -lboost_thread

MidiClock.o: MidiClock.cpp MidiClock.hpp
	$(CC) $(CFLAGS) -c MidiClock.cpp -lrtmidi -lrt -lwiringPi

PigmiLogger.o: PigmiLogger.cpp PigmiLogger.hpp
	$(CC) $(CFLAGS) -c PigmiLogger.cpp -I /usr/include/boost -L /usr/lib -lboost_system -lpthread -lboost_thread

##### Older Stuff ######

midiclock-old:
	g++-4.8 -O3 -std=c++11 midiclock-old.cpp -lrtmidi -lrt -lwiringPi -o ../bin/midiclock-old

testgps:
	g++-4.8 -std=c++11 testgps.cpp -o ../bin/testgps

run_pigmi_logger:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/run_pigmi_logger -L /usr/lib PigmiLogger.cpp run_pigmi_logger.cpp -lboost_system -lpthread -lboost_thread

run_ping_sender:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/run_ping_sender -L /usr/lib run_ping_sender.cpp PingSender.cpp -lboost_system -lpthread -lboost_thread

blink:
	gcc -Wall -o ../bin/blink blink.c -lwiringPi

clean:
	rm -f ../bin/* 
	rm ./*.o
