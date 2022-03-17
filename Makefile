# -*- MakeFile -*-
cpp := -std=c++2a

client: main.o client.o
	g++ $(cpp) main.o client.o -o client -lpthread -lws2_32

main.o: main.cpp
	g++ $(cpp) -c main.cpp

client.o: client.cpp client.hpp utils/utility.h
	g++ $(cpp) -c -g client.cpp 

clean:
	rm *.o client
