CXX := g++
CXX_FLAGS := -lGL -lGLU -lglut -std=c++11
all:
	$(CXX) server.cpp -o serverPort.o $(CXX_FLAGS)
	$(CXX) client.cpp -o client.o $(CXX_FLAGS)

runserver:
	./serverPort.o 2018 > log1 &

killserver:
	killall -r serverPort

testClient: 
	./client.o 127.0.0.1 2018 sample.txt

.PHONY: clean
clean:
	rm -f server.o client.o

