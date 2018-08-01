CC:= g++
SRC_DIR = Linux
CFLAGS= -c -Wall -I. -lGL -lGLU -lglut -std=gnu++0x
all:
	$(CC) prac.cpp $(CFLAGS) -o prac.o  && ./prac.o

.PHONY: clean
clean:
	rm -rf *.o *.exe