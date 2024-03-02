CC=g++

all: main.o am clean

main.o: main.cc
	$(CC) -c -o $@ $< -I/usr/include/SDL2 -std=c++20 -Wall
    
am: main.o
	$(CC) -o $@ $< -lSDL2 -lSDL2_mixer -lncurses -lpthread

clean:
	rm -f main.o