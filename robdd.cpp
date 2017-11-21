#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>


using namespace std;

typedef vector<vector<int>> IntContainer;
typedef IntContainer::iterator IntIterator;

class ITE
{
private:
    int nNodes, nInputs, nLatches, nOutputs, nAnds;
    FILE* fin;
    std::vector<int> gateNode; // default 0
    std::vector<int*> uniqueTable; // 0 {1,1,1} {2,2,2}
    map<vector<int>,int> hashUniqueTable; // hash table
    map<vector<int>,int> hashComputeTable; // compute table
public:
    ITE(char * filename);
    ~ITE();
    void readFile(char * filename);
    int ite(int i, int j, int k);
    bool terminal(int i,int t,int e,int& terminalResult);
    bool isExists(int f,int g,int h,int& index);
    int findorAddUniqueTable(int top, int t, int e);
    int topVariable(int i, int t, int e);
    int function(int head, int top, bool tf);
    void getBDD(void);
    void toString(std::string &ostr);
};

ITE::ITE(char *filename) {
    // initialize uniqueTable
    uniqueTable.push_back(0);
    uniqueTable.push_back(new int[3]);
    // 1 represent 0 in boolean
    // uniqueTable {2 1 0} === value left right
    uniqueTable[1][2] = 1; // 0 I
    uniqueTable[1][1] = 1; // 0 T
    uniqueTable[1][0] = 1; // 0 E
    // 2 represent 1 in boolean
    uniqueTable.push_back( new int[3]);
    uniqueTable[2][2] = 2; // 1 I
    uniqueTable[2][1] = 2; // 1 T
    uniqueTable[2][0] = 2; // 1 E

    //
    std::vector<int> key1(3,1); // NOT gate
    std::vector<int> key2(3,2); // TRUE gate
    hashUniqueTable[key1] = 1; // index = 1
    hashUniqueTable[key2] = 2; // index = 2

    //readFile
    readFile(filename);
}

ITE::~ITE() {
    for(int i = 0; i < uniqueTable.size(); i++){
        delete uniqueTable[i];
    }

}

int ITE::ite(int i, int t, int e)
{
    int terminalResult = 0;
    // Check if the ITE is the terminal case
    if(terminal(i,t,e,terminalResult))
        return terminalResult;
    else{
        if(isExists(i,t,e,terminalResult)){
            return terminalResult;
        }else{
            int top = topVariable(i,t,e);
            int t = ite(function(i,top,true), function(t,top,true),function(e,top,true));
            int e = ite(function(i,top,false), function(t,top,false), function(e,top,true));
            if(t == e)
                return t;
            int r = findorAddUniqueTable(top,t,e);
            int keys[] = {i,t,e};
            std::vector<int> key (keys, keys+sizeof(keys)/sizeof(int));
            hashComputeTable[key] = r;
            return r;
        }
    }
}

// The Terminal Case
bool ITE::terminal(int i, int t, int e, int& terminalResult)
{
    if(i == 2) // if  I == 1
        terminalResult = t; // return true
    else if(i == 1) // if I == 0
        terminalResult = e;
    else if(t == 2 && e == 1) // T == 1 and E == 0
        terminalResult = i;
    else if(t == e) // T == E
        terminalResult = t;
    else if(t == 1 && e == 2)
        terminalResult = -1 * i;
    else
        return false;

    return true;
}

// Check the computed table has entry {(f,g,h),r}
bool ITE::isExists(int f, int g, int h, int& computedTableIndex)
{
    // Key
    int keys[] = {f,g,h};
    std::vector<int> key(keys, keys+sizeof(keys)/sizeof(int));

    // Check Computed Table
    if(hashComputeTable.find(key) == hashComputeTable.end()) // can't find the index for ITE
        return false;
    else{
        computedTableIndex = hashComputeTable[key];
        return true;
    }
}

int ITE::topVariable(int i, int t, int e)
{
    int min = i;

    if(t < min)
        min = t;

    if(e < min)
        min = e;

    return min;	// the minimal value is the top Node
}

// function
int ITE::function(int head, int top, bool tf)
{
    // Check the head is possitive or negetive
    if(head%2 == 0){
        // Terminal Nodes
        if(head == 1 || head == 2)
            return head;

        //Return node
        if(uniqueTable[head][2] == top && tf)
            return uniqueTable[head][1];
        else if(uniqueTable[head][2] == top && !tf)
            return uniqueTable[head][0];

        return head;
    }else{
        if(head == 2)
            return 1;
        else if(head == 1)
            return 2;

        if(uniqueTable[head][2] == top && tf)
            return uniqueTable[head][0];
        else if(uniqueTable[head][2]== top && !tf)
            return uniqueTable[head][1];

        return head -1;
    }
}

// find / insert uniqueTable
int ITE::findorAddUniqueTable(int top, int t, int e)
{
    int keys[] = {top, t, e};
    std::vector<int> key ( keys, keys+ sizeof(keys)/sizeof(int));

    // Check if the uniqueTable exist already
    if(hashUniqueTable.find(key) != hashUniqueTable.end())
        return hashUniqueTable[key];

    // Add ite into hashUniqueTable
    int index = uniqueTable.size();

    uniqueTable.push_back(new int[3]);
    uniqueTable[index][2] = top;
    uniqueTable[index][1] = t;
    uniqueTable[index][0] = e;

    hashUniqueTable[key] = index;

    return index;
}

void ITE::readFile(char *filename)
{
    char fname[40], aigs[4];
    string line;
    strcpy(fname,filename);	// copy filename to fname
    fin = fopen(fname,"r");
    if (fin != NULL) {
        printf("Reading in AIGERs information...\n");
        fscanf(fin,"%s",aigs);
        if(strcmp("aag", aigs) != 0)
        {
            printf("The file is not in AAG format!");
            exit(-1);
        }
        // start reading parameters
        fscanf(fin,"%d %d %d %d %d", &nNodes, &nInputs, &nLatches, &nOutputs, &nAnds);
        cout << nNodes << nInputs << nLatches << nOutputs << nAnds << endl; //debug
        int node;
        for(int i =3; i < 3+nNodes; i++)
        {
            if(fin != NULL){
                fscanf(fin,"%d",&node);
                cout << node << endl;
                uniqueTable.push_back(new int[3]);
                uniqueTable[i][2] = node;   //ID
                uniqueTable[i][1] = 2;      //R
                uniqueTable[i][0] = 1;      //L
                gateNode.push_back(node); // push the input/output node into gateNode vector
                // if the node number is odd, output should go through not before it throw back value
            }else{
                perror ("Error opening file");
                exit(-1);
            }

        }
        getBDD();


    }else{
        perror ("Error opening file");
        exit(-1);
    }


}

void ITE::getBDD() {
    for(int i =0; i < nAnds; i++){
        int index,right,left;
        fscanf(fin, "%d %d %d", &index, &right, &left);
        //debug: cout << index << t << e << endl;
        //ite(t,e,1);
        if(index%2 == 0){
            // AND node
            //ite(right, left, 1);
        }else{
            // NAND node
            //ite(right,1,left);
            index -= 1;
        }
        gateNode.push_back(index);
    }

}

void ITE::toString(std::string &ostr) {
    stringstream ss;
    //Extract BDD Table
    for(int i = 1; i < uniqueTable.size(); i++){
        ss << i << " ";

        for(int j = 2; j >=0; j--)
            ss << uniqueTable[i][j] << " ";

        ss << "\n";
    }
    ostr = ss.str();
}

int robdd(int argc, char *argv[])
{
    ITE* f, *g;
    if(argc != 5)
    {
        printf("The run method is ./%s [input1] [input2] [input3] [output]", argv[0] );
        exit(-1);
    }else{
        char cktName[20];
        strcpy(cktName,argv[1]);
        f = new ITE(cktName); // f
        strcpy(cktName,argv[2]);
        g = new ITE(cktName); // g
        string functionF, functionG;
        f->toString(functionF);
        g->toString(functionG);

        char outputFile[20];
        strcpy(outputFile,argv[4]);
        ofstream ostream;
        ostream.open(outputFile,ios::out);
        ostream << functionF;
        ostream << functionG;

        ostream.close();

        delete f, g;
    }
}