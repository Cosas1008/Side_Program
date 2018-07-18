#pragma warning(disable 4996)

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\freeglut_ext.h>
#include <stdio.h>
#include <stdlib.h>

//#pragma comment( lib, "glew32.lib" )

unsigned int width = 500, height = 500;
GLuint texture;
char* filename = NULL;

void initGL();
void display();
void reshape(GLsizei newwidth, GLsizei newheight);
GLuint loadTexture();
void keyboard(unsigned char key, int x, int y);

int main(int argc, char* argv[])
{
	printf("[main]initialize the code here...\n");

	/* GLUT init */
	glutInit(&argc, argv);				// GLUT initialized
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);	// Set windows size
	glutInitWindowPosition(100, 100);
	glutCreateWindow("BMP Image");		// Set windows name
	initGL();
	glutDisplayFunc(display);			// Main display function
	glutReshapeFunc(reshape);			// Optional
	glutKeyboardFunc(keyboard);			// Optional

	/* OpenGL 2D generic initialization*/
	

	filename = argv[1];
	/* OpenGL main loop*/
	glutMainLoop();

	printf("[main]finish here.\n");
	system("pause");
	return 0;
}

/* Handler for window-repaint event. Called back when the window first appears and
whenever the window needs to be re-painted. */
void display()
{
	/*
	
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix
	// load image with loadTexture function
	
	// Clear color and depth buffers
	// glClearDepth(0.0f);				// Set default	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the window
	// Refresh the texture
	
	loadTexture();
	glMatrixMode(GL_MODELVIEW);
	// Draw a quad
	// bind texture
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(0.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(width, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(width, height);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.0f, height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/
	glMatrixMode(GL_MODELVIEW);		// Set the matrix mode to object modeling
	glPushMatrix();
	glLoadIdentity();
	// glDisable(GL_LIGHTING);
	loadTexture();
	// Draw a textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(width, height, 0);
	glTexCoord2f(0, 1); glVertex3f(width, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(0 ,0 , 0);
	glTexCoord2f(1, 0); glVertex3f(0, height, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

/* Handler for window re-size event. Called back when the window first appears and
whenever the window is re-sized with its new width and height */
void reshape(GLsizei newwidth, GLsizei newheight)
{
	// Set the viewport to cover the new window
	glViewport(0, 0, width = newwidth, height = newheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, 0.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// glutPostRedisplay();
}

//loadTexture .bmp 24bit RGB image function
GLuint loadTexture()
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // width*height*3
	// Read file
	FILE * file;
	errno_t err;

	err = fopen_s(&file, (const char*) filename, "rb");

	if (file == NULL) {
		printf("Image could not be opened\n");
		return false;
	}
	// check the bmp file
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return false;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return false;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	//data = (unsigned char *)malloc(imageSize);// Create a buffer
	unsigned char* data = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	// fread ( void * ptr, size_t size, size_t count, FILE * stream );
	fread(data, sizeof(unsigned char), imageSize, file);
	
	// Convert (B, G, R) to (R, G, B)
	unsigned char tmp;
	for (int j = 0; j < imageSize; j += 3)
	{
		tmp = data[j];
		data[j] = data[j + 2];
		data[j + 2] = tmp;
	}
	// Set glMatrixMode to GL_TEXTURE
	glMatrixMode(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// printf("Data size %d",imageSize);
	// Give the image to OpenGL
	if (data != nullptr) 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else 
	{
		printf("No data");
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Release memory

	fclose(file);
	free(data);

	// glMatrixMode(GL_MODELVIEW);

	return texture;
}


void initGL()
{
	/*
	glViewport(0, 0, width, height); // use a screen size of width by height
	// glEnable(GL_TEXTURE_2D);     // Enable 2D texturing

	glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
	glLoadIdentity();
	
	//glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
	//glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
	glOrtho(0.0, width, 0.0, height, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling
	*/
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);		// Set the matrix mode to object modeling
	glPushMatrix();
	glLoadIdentity();
	// glDisable(GL_LIGHTING);
	// glColor3f(1, 1, 1);
}