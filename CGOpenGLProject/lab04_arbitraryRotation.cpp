#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
/***freeglut***/
#include <.\GL\freeglut.h>
#include <math.h>
using namespace std;
#define PI 3.14159265

void SetupRC();
void ChangeSize(int, int);
void RenderScene(void);
void myKeyboard(unsigned char, int, int);
void mySepcialKey(int, int, int);
void do_rotateX(GLdouble);
void do_rotateY(GLdouble);
void do_rotateZ(GLdouble);
void do_arbitraryRotate(GLdouble);
void do_translate(float, float, float);
void do_scale(float);
void my_Mouse(GLint, GLint, GLint, GLint);

float tx = 0, ty = 0, tz = 0;
double thetaX = 0, thetaY = 0, thetaZ = 0, thetaArb = 0;
float scale = 1;
float mouseX = 0, mouseY = 0, mouseZ = 0;
double mouseWX = 1, mouseWY = 1, mouseWZ = 0;
GLdouble zNear = 0, zFar = 0;

GLfloat TranslateMatrix[16] =
{ 
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};
GLfloat ScaleMatrix[16] =
{ 
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

GLfloat MultiMatrix[16] =
{
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};
GLdouble projectionMatrix[16];
GLdouble modelViewMatrix[16];
int viewport[4];

int main(int argc, char** argv)
{
	tx = ty = tz = 0;
	thetaX = thetaY = thetaZ  = thetaArb = 0;
	mouseWX = 1, mouseWY = 0, mouseWZ = 0;
	cout << "110598009 胡伯源" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(600, 80);
	glutCreateWindow("Moving Cube");
	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySepcialKey);
	glutMouseFunc(my_Mouse);

	glutMainLoop();
	return 0;
}

void SetupRC()
{
	// Light values and coordinates
	GLfloat whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { 0.f, 25.0f, 20.0f, 0.0f };
	// Enable lighting
	glEnable(GL_LIGHTING);
	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); //load the projection matrix
	glLoadIdentity();
	// Viewing volume
	// left:-10; right:10; bottom:-10; top:10
	// 右手定則: -z 朝自己; zNear:-10; zFar:20
	glOrtho(-10, 10, -10, 10, -10, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	zNear = -10;
	zFar = 20;
}

void RenderScene(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); // load the modelview matrix
	glLoadIdentity();
	// 前三個為 eye(x,y,z) 攝影機處的位置
	gluLookAt(10.0f, 10.0f, 10.0f, 0, 0, 0, 0, 1, 0);

	// draw X-axis, Y-axis, Z-axis lines
	// X-axis
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-10, 0, 0);
	glVertex3f(10, 0, 0);
	glEnd();
	//X word
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(11, 1, 0);
	glVertex3f(13, -1, 0);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(11, -1, 0);
	glVertex3f(13, 1, 0);
	glEnd();

	// Y-axis
	glColor3f(0.f, 1.0f, 0.f);
	glBegin(GL_LINES);
	glVertex3f(0, -10, 0);
	glVertex3f(0, 10, 0);
	glEnd();
	//Y word
	glColor3f(0.f, 1.0f, 0.f);
	glBegin(GL_LINES);
	glVertex3f(0, 11, 0);
	glVertex3f(0, 12, 0);
	glEnd();
	glColor3f(0.f, 1.0f, 0.f);
	glBegin(GL_LINES);
	glVertex3f(0, 12, 0);
	glVertex3f(-1, 13, 1);
	glEnd();
	glColor3f(0.f, 1.0f, 0.f);
	glBegin(GL_LINES);
	glVertex3f(0, 12, 0);
	glVertex3f(1, 13, 0);
	glEnd();

	// Z-axis
	glColor3f(0.f, 0.f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, 0, -10);
	glVertex3f(0, 0, 10);
	glEnd();
	//Z word
	glColor3f(0.f, 0.f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 12);
	glVertex3f(0, 0, 11);
	glEnd();
	glColor3f(0.f, 0.f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 11);
	glVertex3f(0, -1, 12);
	glEnd();
	glColor3f(0.f, 0.f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, -1, 12);
	glVertex3f(0, -1, 11);
	glEnd();

	// draw point
	glColor3f(1.0f, 0.0f, 1.0f);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3f((float)mouseWX + 1, (float)mouseWY + 1, (float)mouseWZ + 1);
	glEnd();
	glFlush();

	//glVertex3f(mouseX, mouseY, 0);
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f((float)mouseWX, (float)mouseWY, (float)mouseWZ);
	glEnd();

	// perform transformation for the cube
	
	do_rotateX(thetaX);
	do_rotateY(thetaY);
	do_rotateZ(thetaZ);
	// perform arbitrary rotation for the cube
	do_arbitraryRotate(thetaArb);

	do_translate(tx, ty, tz);

	do_scale(scale);

	// cube
	glColor3f(1, 1, 0);
	glutSolidCube(6);
	glutSwapBuffers();

}

void do_rotateX(GLdouble angle)
{
	// Rotate angle Degrees around Y-Achsis
	GLdouble cosA = cos(angle * PI / 180);
	GLdouble sinA = sin(angle * PI / 180);
	// populate matrix in column major order
	GLdouble m[4][4] = {
	  { 1.0,	0.0,	0.0,  0.0}, // <- X column
	  { 0.0,	cosA,	-sinA,  0.0}, // <- Y column
	  { 0.0,	sinA,	cosA,  0.0}, // <- Z column
	  { 0.0,	0.0,	0.0,  1.0}  // <- W column
	};
	glMultMatrixd(&m[0][0]);
}

void do_rotateY(GLdouble angle)
{
	//rotate around y axis
	// Rotate angle Degrees around Y-Achsis
	GLdouble cosA = cos(angle * PI / 180);
	GLdouble sinA = sin(angle * PI / 180);
	// populate matrix in column major order
	GLdouble m[4][4] = {
	  { cosA, 0.0, -sinA,  0.0}, // <- X column
	  { 0.0,  1.0,   0.0,  0.0}, // <- Y column
	  { sinA, 0.0,  cosA,  0.0}, // <- Z column
	  { 0.0,  0.0,   0.0,  1.0}  // <- W column
	};
	glMultMatrixd(&m[0][0]);
}

void do_rotateZ(GLdouble angle)
{
	// Rotate angle Degrees around Z-Achsis
	GLdouble cosA = cos(angle * PI / 180);
	GLdouble sinA = sin(angle * PI / 180);
	// populate matrix in column major order
	GLdouble m[4][4] = {
	  { cosA,	-sinA,	0.0,  0.0}, // <- X column
	  { sinA,	cosA,   0.0,  0.0}, // <- Y column
	  { 0.0,	0.0,	1.0,  0.0}, // <- Z column
	  { 0.0,	0.0,	0.0,  1.0}  // <- W column
	};
	glMultMatrixd(&m[0][0]);
}

void do_translate(float tx, float ty, float tz)
{
	TranslateMatrix[12] = tx;
	TranslateMatrix[13] = ty;
	TranslateMatrix[14] = tz;
	glMultMatrixf(TranslateMatrix);
}

void do_scale(float scale)
{
	ScaleMatrix[0] = scale;
	ScaleMatrix[5] = scale;
	ScaleMatrix[10] = scale;
	glMultMatrixf(ScaleMatrix);
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		// reset translation & rotation
		tx = ty = tz = 0;
		thetaX = thetaY = thetaZ = thetaArb = 0;
		mouseWX = 1; mouseWY = mouseWZ = 0;
		scale = 1;
		break;
		// change the rotation angle thetaX along X-axis
	case 'a':
		thetaX += 3;
		break;
	case 'd':
		thetaX -= 3;
		break;
		// change the rotation angle thetaY along Y-axis
	case 'w':
		thetaY += 3;
		break;
	case 's':
		thetaY -= 3;
		break;
		// change the rotation angle thetaZ along Z-axis
	case 'z':
		thetaZ += 3;
		break;
	case 'x':
		thetaZ -= 3;
		break;
	case '+':
		scale *= 1.2;
		break;
	case '-':
		scale *= 0.8;
		break;
	case 'q':
		thetaArb += 3;
		break;
	case 'e':
		thetaArb -= 3;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void mySepcialKey(int key, int x, int y)
{
	switch (key)
	{
		// change the translation along X-axis
	case GLUT_KEY_LEFT:
		tx -= 0.2f;
		break;
	case GLUT_KEY_RIGHT:
		tx += 0.2f;
		break;
		// change the translation along Y-axis
	case GLUT_KEY_UP:
		ty += 0.2f;
		break;
	case GLUT_KEY_DOWN:
		ty -= 0.2f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void my_Mouse(GLint button, GLint state, GLint x, GLint y)
{
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			
			//std::cout << "(" << x << "," << y << ")" << std::endl;
			mouseWX = 1; mouseWY = mouseWZ = 0;
			float winX = (float)x;
			float winY = (float)y;
			float screenZ = 1;
			glReadPixels(winX, viewport[3] - winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screenZ);
			gluUnProject(winX, viewport[3] - winY, screenZ, modelViewMatrix, projectionMatrix, viewport
				, &mouseWX, &mouseWY, &mouseWZ);
			std::cout << "(" << mouseWX << "," << mouseWY << "," << mouseWZ << ")" << std::endl;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void do_arbitraryRotate(GLdouble angle)
{
	// Rotate angle Degrees around arbitaty-Achsis
	GLfloat Cos = cos(angle * PI / 180);
	GLfloat Sin = sin(angle * PI / 180);
	GLfloat x = mouseWX, y = mouseWY, z = mouseWZ;
	double magnitude = sqrt(x * x + y * y + z * z);
	if (magnitude == 0) throw "ERROR";
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;

	
	MultiMatrix[0] = Cos + (1 - Cos) * x * x;
	MultiMatrix[1] = (1 - Cos) * y * x + Sin * z;
	MultiMatrix[2] = (1 - Cos) * z * x - Sin * y; // Should be minus between two numbers
	MultiMatrix[4] = (1 - Cos) * x * y - Sin * z;
	MultiMatrix[5] = Cos + (1 - Cos) * y * y;
	MultiMatrix[6] = (1 - Cos) * z * y + Sin * x;
	MultiMatrix[8] = (1 - Cos) * x * z + Sin * y;
	MultiMatrix[9] = (1 - Cos) * y * z - Sin * x; // Should be minus between two numbers
	MultiMatrix[10] = Cos + (1 - Cos) * z * z;
	glMultMatrixf(MultiMatrix);
}