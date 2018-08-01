// Working Radar Code

#include <GL\freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void Radar_Pattern(void);
void initGL();
void display();
void Keyboard(unsigned char, int, int);

float sw = 0;


int main(int argc, char* argv[])
{
	printf("\n   Hold 's' key down to sweep radar pattern.\n");

	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(400, 100);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("Radar Pattern - MaxH - March 2011");
	glutDisplayFunc(display);			// Main display function
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 1;
}


void display()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, 0, 3, 0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glRotatef(sw, 0, 0, 1);
	Radar_Pattern();
	// keep scanning
	sw += 3;  
	glutPostRedisplay();

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int q, int s)
{
	switch (key) {
	case 's':  sw += 3;  glutPostRedisplay();   break;
	case  27:  exit(0);
	default: printf("   Keyboard %c == %d\n", key, key);
	}
}

void Radar_Pattern(void)
{
	int i, n = 120;
	float da, ga, gb, a, b;
	const float TWOPI = 3.1415926535 * 2.0;

	da = TWOPI / (float)n;

	glBegin(GL_QUADS);
	for (i = 0; i < n; i++) {
		a = (float)i * da;
		b = a + da;
		ga = 0.90 * (float)(i) / (float)n;
		gb = 0.90 * (float)(i + 1) / (float)n;
		glColor3f(0, ga, 0);
		glVertex2i(0, 0); glVertex2f(cos(a), sin(a));
		glColor3f(0, gb, 0);
		glVertex2f(cos(b), sin(b)); glVertex2i(0, 0);
	}
	glEnd();
}

void initGL()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);		// Set the matrix mode to object modeling
	glPushMatrix();
	glLoadIdentity();
}