#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <cassert>
#include <sys/time.h>


using namespace std;

// read file 

struct path
{
	int weight;
	std::vector<int> vertex;
};

int main(int argc, char** argv)
{
	struct timeval start, end;

    gettimeofday(&start,NULL);

	// main.cpp + <.tsp>
	if(argc < 2){
        printf("Usage: ./tsp <.tsp>\n");
        return 0;
    }
	if (!isFileExist(argv[1])) { assert(0); }



	gettimeofday(&end, NULL);

    long msec = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
    printf("Elapsed: %.2f sec\n", msec/1000.0);
	return 0;
}