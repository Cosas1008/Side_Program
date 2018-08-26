#include <vector>


using namespace std;

// points in linked-list form

class Points {
public:
    Points(int,int*);
    void createPnt(int);
    //~Points();
private:
    int num;
    vector<float> points;
};