CXX = g++
CXXFLAGS = -g -std=c++11
TOPDIR = ~/Cleareye
SRCDIR = $(TOPDIR)/src

cleareye: cleareye.o server.o client.o
		$(CXX) $(CXXFLAGS) cleareye.o server.o client.o -o cleareye -lws2_32

cleareye.o: src/cleareye.cpp
		$(CXX) $(CXXFLAGS) -c src/cleareye.cpp

server.o: src/packet.h src/server.h src/server.cpp 
		$(CXX) $(CXXFLAGS) -c src/server.cpp

client.o: src/server.h src/client.h src/client.cpp 
		$(CXX) $(CXXFLAGS) -c src/client.cpp

run:
		cleareye

clean:
		del *.o
