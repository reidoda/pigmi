midiclock:
	g++-4.8 -std=c++11 midiclock.cpp -lrtmidi -lrt -o ../bin/midiclock 

testgps:
	g++-4.8 -std=c++11 testgps.cpp -o ../bin/testgps

the_server:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/server -L /usr/lib server.cpp -lboost_system -lpthread

the_client:
	g++-4.8 -std=c++11 -lstdc++ -I /usr/include/boost -o ../bin/client -L /usr/lib client.cpp -lboost_system -lpthread 

clean:
	rm -f ../bin/* 
