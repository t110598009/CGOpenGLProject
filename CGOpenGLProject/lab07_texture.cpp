#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <.\GL\freeglut.h>

using namespace std;
using namespace cv;

void ChangeSize(int, int);
void RenderScene(void);
void SetupRC();

// Keep track of effects step
int nStep = 0;

// Lighting data
GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat lightSpecular[] = { 0.9f, 0.9f, 0.9f };
GLfloat materialColor[] = { 0.8f, 0.0f, 0.0f };
GLfloat vLightPos[] = { -80.0f, 120.0f, 100.0f, 0.0f };
GLfloat ground[3][3] = { { 0.0f, -25.0f, 0.0f },
						{ 10.0f, -25.0f, 0.0f },
						{ 10.0f, -25.0f, -10.0f } };

GLuint textures[4];

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(600, 80);
	glutCreateWindow("Loading texture from img");

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	SetupRC();

	std::cout << "110598009 ­J§B·½" << std::endl;
	glutMainLoop();

	return 0;
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the clipping volume
	glOrtho(-50, 50, -50, 50, -50 ,50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-5.0f, 15.0f, 30.0f, 0, 0, 0, 0, 1, 0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-25.0f, -25.0f, -10.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(00.0f, -25.0f, 10.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(00.0f, 25.0f, 10.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-25.0f, 25.0f, -10.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -25.0f, 10.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(25.0f, -25.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(25.0f, 25.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 25.0f, 10.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-25.0f, 25.0f, -10.0f); //cube top copy by textures[3] fourth vertex
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 25.0f, 10.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(25.0f, 25.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 25.0f, -20.0f);
	glEnd();

	glEnd();
	glutSwapBuffers();
}

void SetupRC() {
	GLbyte* pBytes;
	GLint nWidth, nHeight, nComponents;
	GLenum format;

	Mat image;
	String ImgPath = "lab07_img\\floor.jpg";
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(4, textures);

	// Load the texture objects
	ImgPath = "lab07_img\\floor.jpg";
	image = imread(ImgPath, CV_LOAD_IMAGE_COLOR);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);


	ImgPath = "lab07_img\\Block4.jpg";
	image = imread(ImgPath, CV_LOAD_IMAGE_COLOR);
	cv::flip(image, image, 0); // make picture flipping.
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);
	//free(pBytes);

	ImgPath = "lab07_img\\Block5.jpg";
	image = imread(ImgPath, CV_LOAD_IMAGE_COLOR);
	cv::flip(image, image, 0); // make picture flipping.
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);

	ImgPath = "lab07_img\\Block6.jpg";
	image = imread(ImgPath, CV_LOAD_IMAGE_COLOR);
	cv::flip(image, image, 0); // make picture flipping.
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);

}