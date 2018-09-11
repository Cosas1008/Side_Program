#include <Point.h>



float radius, degree;

float rad2deg = 2*PI / 360;
// Constructor Define
Point::Point(void){
    srand( time(NULL) );
    radius = 1;
    degree = rand()%180; // from 0 to 180
    x = radius*cos(rad2deg * degree);
    y = radius*sin(rad2deg * degree);
};

Point::Point(int r, int deg){
    x = r*cos(rad2deg * degree);
    y = r*sin(rad2deg * degree);
};


float Point::getX(){
    return x;
};

float Point::getY(){
    return y;
};