#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <math.h>
#include <sys/time.h>

#include "parser.h"

using namespace std;

struct Cycle
{
    int weight;
    vector <int> path;
};
bool hasCycle = false;

void solution(float result_weight,vector <int> result_path);

int main(int argc, char** argv)
{
    struct timeval start, end;

    gettimeofday(&start,NULL);

    if(argc < 2){
        printf("Usage: ./Lab2 <.in>\n");
        return 0;
    }

    parser_S* pParser;
    if (argc == 2) {
        pParser = new parser_S(argv[1]);
        pParser->parse();
        pParser->dump();
    } else if (argc == 3) { //verifier
        pParser = new parser_S(argv[1], argv[2]);
        if (!pParser->parse()) {
            return 0;
        }
        pParser->dump();
    }

    /* Start your code */
    // 1. find minimum mean cycle
    // 2. dump minimum mean weight and minimum mean cycle in "solution.txt"
	// contributor: noob SimonBack
	
    int numNode = pParser->_vNode.size();
    float **distance = new float*[numNode + 1];
    float **predecessor = new float*[numNode + 1];

    for(int i = 0; i < numNode + 1; ++i) {
        distance[i] = new float[numNode + 1];
        predecessor[i] = new float[numNode + 1];
    }

    // initialize distance and predecessor array
    for(int u = 1; u <= numNode; u++) {
        for(int v = 1; v <= numNode; v++) {
            distance[u][v] = INFINITY;
            predecessor[u][v] = NAN;
        }
    }

    for(auto edge : pParser->_vEdge) {
        distance[edge->_fromNode][edge->_toNode] = edge->_weight;
        predecessor[edge->_fromNode][edge->_toNode] = edge->_fromNode;
    }

    // Floyd-Warshall
    for(int k = 1; k <= numNode; k++) {
        printf("\rIn progess: %.1f%%", ((float)k/numNode) * 100);
        fflush(stdout);
        for(int u = 1; u <= numNode; u++) {
            for(int v = 1; v <= numNode; v++) {
                if(distance[u][v] > distance[u][k] + distance[k][v]){
                    distance[u][v] = distance[u][k] + distance[k][v];
                    predecessor[u][v] = predecessor[k][v];
                }
            }
        }
    }

    cout << endl;

    vector <Cycle> cycles;

    for(int n = 1; n <= numNode; n++)
    {
        if(distance[n][n] != INFINITY)
        {
            Cycle cycle;
            int u = n, v = n;

            hasCycle = true;
            cycle.weight = distance[n][n];

            cycle.path.push_back(n);
            while(predecessor[u][v] != n) {
                cycle.path.push_back((int)predecessor[u][v]);
                v = predecessor[u][v];
            }
            cycle.path.push_back(n);
            reverse(cycle.path.begin(), cycle.path.end());

            cycles.push_back(cycle);
        }
    }

    if(hasCycle)
    {
        float result_weight = INFINITY;
        vector <int> result_path;

        for (auto cycle : cycles) {
            float mean_weight = (float) cycle.weight / (cycle.path.size() - 1);
            if (mean_weight < result_weight) {
                result_weight = mean_weight;
                result_path = cycle.path;
            }
        }

        cout << "Minimum Mean Weight: " << fixed << setprecision(2) << result_weight << endl;
        cout << "Path: ";
        for (auto point : result_path)
            cout << point << " ";
        cout << endl;

        solution(result_weight,result_path);
    }
    else
        cout << "No cycle" << endl;

    gettimeofday(&end, NULL);

    long msec = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
    printf("Elapsed: %.2f sec\n", msec/1000.0);

    delete pParser;
    return 0;
}

void solution(float result_weight,vector <int> result_path)
{
    FILE* pfile;
    pfile = fopen("solution.txt","w");
    if(hasCycle){
        //has cycle
        fprintf(pfile,"%.2f\n",result_weight);
        for (auto point : result_path)
            fprintf(pfile,"%d ",point);
        fprintf(pfile,"\n");
    }else
        fprintf(pfile,"No cycle\n");


    fclose(pfile);
}
