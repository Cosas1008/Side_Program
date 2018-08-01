#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
// #include <unistd.h> // usleep function
#include <circle.h>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLUT
#include <GL/glut.h>

const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
const GLfloat factor = 0.1f;
static int day = 200;
double FPS, FPS_old;
// Function Declair
double CalFrequency(void);
void myDisplay(void);
void myIdle(void);
void print(int x, int y,int z, char *string);

/*
//This will draw a circle
void myDisplay(void)
{
     int i;
     glClear(GL_COLOR_BUFFER_BIT);
     glBegin(GL_POLYGON);
     for(i=0; i<n; ++i){
        glVertex2f(R*cos(2*Pi/n*i), R*sin(2*Pi/n*i));
     }
     glEnd();
     glFlush();
}
void myDisplay(void)
{
     GLfloat x;
     glClear(GL_COLOR_BUFFER_BIT);
     glBegin(GL_LINES);
         glVertex2f(-1.0f, 0.0f);
         glVertex2f(1.0f, 0.0f);         // 以上两个点可以画x轴
         glVertex2f(0.0f, -1.0f);
         glVertex2f(0.0f, 1.0f);         // 以上两个点可以画y轴
     glEnd();
     glBegin(GL_LINE_STRIP);
     for(x=-1.0f/factor; x<1.0f/factor; x+=0.01f)
     {
         glVertex2f(x*factor, sin(x)*factor);
     }
     glEnd();
     glPointSize(5.0f);

     glBegin(GL_POINTS);

         glVertex2f(0.0f, 0.0f);

         glVertex2f(0.5f, 0.5f);

     glEnd();
     glFlush();

}
*/
void myDisplay(void)
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, 1, 0, 1);
    gluLookAt(0, 0, 1       // camera
            , 0, 0, 0,      // look at
             0, 1, 0);      // Z axis

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Radar line
    glColor3f(0.0f, 0.0f, 1.0f);
    /*
    glBegin(GL_POINT);
    for(int i=0; i<20; ++i){
        glVertex3f(0.1*cos(2*Pi/20*i), 0.1*sin(2*Pi/20*i),0);
    }
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(10,10,0);
    for(int i = 0; i < 100; ++i){
        glVertex3f(0,0,0);
        glVertex3f(10.0*cos(2*Pi/i), 10.0*sin(2*Pi/i),0);
    }
    glVertex3f(0,0,0);
    glVertex3f(-10,10,0);
    glEnd();
    
    glBegin(GL_POINTS);
    for(int i=0;i<1000;++i)
    {
        glVertex3f(cos(2*3.14159*i/1000.0),sin(2*3.14159*i/1000.0),0);
    }
    glEnd();
    */
    circle c;
    c.createcircle(0,10,0);
   //glColor3f(1.0f, 0.0f, 0.0f);
    //glutSolidSphere(0.2, 20, 10);
    glFlush();
    glutSwapBuffers();
}
void myIdle(void)
{
    /* 新的函数，在空闲时调用，作用是把日期往后移动一天并重新绘制，达到动画效果 */
    ++day;
    if( day >= 360 ){
        day = 0;
    }
    myDisplay();
    FPS = CalFrequency();
    if(FPS != FPS_old){
        //printf("FPS = %f\n", FPS);
        FPS_old = FPS;
    }
    
}

double CalFrequency(void)
{
    static int count;
    static double save;
    static clock_t last, current;
    double timegap;

    ++count;
    if( count <= 50 )
        return save;
    //  usleep(1000);
    last = current;
    current = clock();
    timegap = (current-last)/(double)CLOCKS_PER_SEC;
    save = count/timegap;
    count = 0;
    return save;
}

void print(int x, int y,int z, char *string)
{
    //set the position of the text in the window using the x and y coordinates
    glRasterPos2f(x,y);
    //get the length of the string to display
    int len = (int) strlen(string);

    //loop to display character by character
    for (int i = 0; i < len; i++) 
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,string[i]);
    }
}

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("First OpenGL Example");
    glutDisplayFunc(&myDisplay);
    //glutIdleFunc(&myIdle);                // 新加入了这句
    glutMainLoop();
    //glViewport(0,0,400,400);
    //print(-400,-400,0,fpsChar);
    return 0;

}