CC:= g++
SRC_DIR = Linux
CFLAGS := -Wall -I../Linux -lGL -lGLU -lglut 
all:
	$(CC) radar.cpp Point.cpp $(CFLAGS) -o radar.o -std=gnu++0x && ./radar.o

	
.PHONY: clean
clean:
	rm -rf *.o *.exe