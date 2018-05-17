CXX := g++
CXX_FLAGS := -lhiredis -std=c++11
all:
	$(CXX) server.cpp -o serverPort $(CXX_FLAGS)
	$(CXX) client.cpp -o client $(CXX_FLAGS)
runserver:
	./serverPort 2015 > log1 &
	./serverPort 2016 > log2 &
	./serverPort 2017 > log3 &

killserver:
	killall -r serverPort

testClient: 
	./client 127.0.0.1 2015 2016 2017 sample.txt

.PHONY: clean
clean:
	rm -f server client

