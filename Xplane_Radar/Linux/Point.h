#include <stdlib.h>
#include <time.h>

#define PI 3.14159265

using namespace std;

class Point{
public:
    float getX();
    float getY();
    Point();
    Point(int r, int deg);
private:
    float x;
    float y;
}