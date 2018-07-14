//#pragma warning(disable 4996)
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>


void DrawImage(GLuint in_texture) {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glLoadIdentity();
	glDisable(GL_LIGHTING);


	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, in_texture);


	// Draw a textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2i(1, -1);
	glTexCoord2f(1, 1);
	glVertex2i(1, 1);
	glTexCoord2f(0, 1);
	glVertex2i(-1, 1);
	glEnd();


	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

}


GLuint texture = 0;
void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2, 2, -2, 2, 2, -2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2i(1, -1);
	glTexCoord2f(1, 1);
	glVertex2i(1, 1);
	glTexCoord2f(0, 1);
	glVertex2i(-1, 1);
	glEnd();

	glutSwapBuffers();
}

GLuint loadTexture(const char * filename)
{

	GLuint texture;
	unsigned int width, height;
	unsigned char * data;
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // width*height*3
	// Read file
	FILE * file;
	errno_t err;

	err = fopen_s(&file, filename, "rb");

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
	
	// Create a buffer
	data = (unsigned char *)malloc(imageSize);

	// Read the actual data from the file into the buffer
	fread(data, width * height * 3, 1, file);

	fclose(file);
	glGenTextures(1, &texture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texture);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	free(data);
	return texture;
}

int main(int argc, char* argv[])
{
	printf("[main]initialize the code here...\n");

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, &data);
	/*
	GLuint texture;
	texture = loadTexture(argv[1]);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("glutTest09");

	DrawImage(texture);
	*/
	// Poor filtering. Needed !
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("GLUT");
	glewInit();
	texture = loadTexture(argv[1]);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;

	printf("[main]finish here.\n");
	system("pause");
	return 0;
}