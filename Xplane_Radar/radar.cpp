#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
// #include <unistd.h> // usleep function

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLUT
#include <GL/glut.h>

// windows
#ifdef WIN32
#include <windows.h>
#endif

#define WIDTH 400
#define HEIGHT 400
const int n = 20;
const GLfloat R = 1.0f;
const GLfloat Pi = 3.1415926536f;
static int day = 200;
double FPS, FPS_old;
char fpsChar[255];
// Function Declair
double CalFrequency(void);
void myDisplay(void);
void myIdle(void);
void print(int x, int y,int z, char *string);

void myDisplay(void)
{
    /* Clear the window. */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    int n = 120;

    // float da, ga, gb, a, b;
    // da = 2*Pi / (float)n;
    // glBegin(GL_QUADS);
	// for (int i = 0; i < n; i++) {
	// 	a = (float)i * da;
	// 	b = a + da;
	// 	ga = 0.90 * (float)(i) / (float)n;
	// 	gb = 0.90 * (float)(i + 1) / (float)n;
	// 	glColor3f(0, ga, 0);
	// 	glVertex2i(0, 0); glVertex2f(cos(a), sin(a));
	// 	glColor3f(0, gb, 0);
	// 	glVertex2f(cos(b), sin(b)); glVertex2i(0, 0);
	// }
	// glEnd();

    glLineWidth(1.0f);
    glViewport(0,-HEIGHT/2,WIDTH,HEIGHT);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f,1.0f,0.0f);
    for(float l=0.2; l <= 1; l += 0.2){
        for(int i=0; i<=n; i++){
            glVertex2f(R*l*cos(Pi*(float)i/(float)n),R*l*sin(Pi*(float)i/(float)n));
        }
    }
    glEnd();
    glFlush();
    glutSwapBuffers();
}

void printingOut(char name[])
{
    int i = 0;
    while(i <= strlen(name) ){
        std::cout << name[i];
        i++;
    }

}

void myIdle(void)
{
    myDisplay();
    FPS = CalFrequency();
    char header[] = "FPS = ";
    char buffer[64];
    int ret = snprintf(buffer, sizeof buffer, "%f", FPS);
    strcpy(fpsChar,header);
    if(FPS != FPS_old){
        strcat(fpsChar,buffer);
        printingOut(fpsChar);
        FPS_old = FPS;
    }
    strcat(fpsChar,buffer);
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
     count = 0;
    //  usleep(1000);
     last = current;
     current = clock();
     timegap = (current-last)/(double)CLOCKS_PER_SEC;
     save = 50.0/timegap;
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
};
void print_bitmap_string(char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *s);
         s++;
      }
   }
}
int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("First OpenGL Example");
    glutDisplayFunc(&myDisplay);
    glutIdleFunc(&myIdle);
    glutMainLoop();
    glViewport(0,0,WIDTH,HEIGHT);
    float x = -225.0,y = 70.0;
    glRasterPos2f(x, y);
    print_bitmap_string(fpsChar);
    return 0;

}