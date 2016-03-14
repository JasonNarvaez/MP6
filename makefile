# makefile

all: dataserver client client2

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -c -g -std=c++11 NetworkRequestChannel.cpp

semaphore.o: semaphore.h semaphore.cpp
	g++ -c -g -std=c++11 semaphore.cpp
	
BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -c -g -std=c++11 BoundedBuffer.cpp

dataserver: dataserver.cpp NetworkRequestChannel.o
	g++ -g -std=c++11 -o dataserver dataserver.cpp NetworkRequestChannel.o -lpthread

client: client.cpp semaphore.o BoundedBuffer.o NetworkRequestChannel.o
	g++ -g -std=c++11 -pthread -o client client.cpp semaphore.o BoundedBuffer.o NetworkRequestChannel.o
	
