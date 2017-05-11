//
// Created by user on 5/7/2017.
//


#ifndef __PARSER_H__
#define __PARSER_H__
#include <vector>
#include <fstream>
#include <set>
#include <map>
#include <list>


using namespace std;

//----------------------------------------------------------------------------------------------------------------------
struct node_S{
public:
    node_S(int id) : _id(id) {}
    void addConnection(int);
    int _id;
    list<int> connectTo;
};
//----------------------------------------------------------------------------------------------------------------------
struct edge_S{
public:
    edge_S(int fromNode, int toNode,int weight) : _fromNode(fromNode), _toNode(toNode), _weight(weight) {}
    int _fromNode;
    int _toNode;
    int _weight;
};
//----------------------------------------------------------------------------------------------------------------------
struct path{
public:
    path(){};
    void addpath();
    vector<edge_S*> walked;
};

//----------------------------------------------------------------------------------------------------------------------
struct parser_S{
public:
    parser_S(const char* p_caCase);
    parser_S(const char* p_caCase, const char* p_caSol);
    ~parser_S() {}
    bool parse();
    void dump();
    void solution();
    void Dijkstra();
    void findCycle();
    edge_S* doesEdgeExist(int start, int end);
    void FloydWarshallWithPathReconstruction(int, int);


    //data
    vector<edge_S*>     _vEdge;
    vector<node_S*>     _vNode;

    //mapping
    map<int,int> _startIndex;
private:
    void parseInput();
    bool parseSol();
    bool _getLine();
    bool _isFileExist(const char*);

    //data
    ifstream    _input;
    const char* _caCase;
    const char* _caSol;
    char        _buffer[512];
    double      _dMMC;
    vector<int> _vCycle;
};
//----------------------------------------------------------------------------------------------------------------------
struct less_than_key
{
    inline bool operator() (const edge_S* edge1, const edge_S* edge2)
    {
        return (edge1->_fromNode < edge2->_fromNode);
    }
};
#endif

