midiclock:
	g++-4.8 -std=c++11 midiclock.cpp -lrtmidi -lrt -o ../bin/midiclock 

midiclock-old:
	g++-4.8 -O3 -std=c++11 midiclock-old.cpp -lrtmidi -lrt -lwiringPi -o ../bin/midiclock-old

testgps:
	g++-4.8 -std=c++11 testgps.cpp -o ../bin/testgps

server:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/server -L /usr/lib server.cpp -lboost_system -lpthread -lboost_thread

client:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/client -L /usr/lib client.cpp -lboost_system -lpthread 

blink:
	gcc -Wall -o ../bin/blink blink.c -lwiringPi

clean:
	rm -f ../bin/* 
