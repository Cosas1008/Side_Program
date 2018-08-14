CC:= g++
SRC_DIR = Linux
CFLAGS := -Wall -I. -lGL -lGLU -lglut 
all:
	$(CC) radar.cpp $(CFLAGS) -o radar.o -std=gnu++0x && ./radar.o

	
.PHONY: clean
clean:
	rm -rf *.o *.exe