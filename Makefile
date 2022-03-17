# -*- MakeFile -*-
cpp := -std=c++2a
# -lws2_32 (for windows)
client: main.o client.o
	g++ $(cpp) main.o client.o -o client -lpthread

main.o: main.cpp
	g++ $(cpp) -c main.cpp

client.o: client.cpp client.hpp utils/utility.h
	g++ $(cpp) -c -g client.cpp 

clean:
	rm *.o client
