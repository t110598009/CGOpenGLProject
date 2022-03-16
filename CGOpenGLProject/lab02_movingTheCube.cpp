#include <stdio.h> 
#include <stdlib.h>
/***freeglut***/
#include <.\GL\freeglut.h>

void SetupRC();
void ChangeSize(int, int);
void RenderScene(void);
void myKeyboard(unsigned char, int, int);
void mySepcialKey(int, int, int);

float tx = 0, ty = 0, tz = 0;
float thetaX = 0, thetaY = 0, thetaZ = 0;

int main(int argc, char** argv)
{
	// These are variable that you will need
	// to move your cube
	// about translation and rotation
	tx = ty = tz = 0;
	thetaX = thetaY = thetaZ = 0;

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

	// perform transformation for the cube
	glRotatef(thetaX, 1, 0, 0);
	glRotatef(thetaY, 0, 1, 0);
	glRotatef(thetaZ, 0, 0, 1);

	glTranslatef(tx, ty, tz);

	// cube
	glColor3f(1, 1, 0);
	glutSolidCube(6);
	glutSwapBuffers();

}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		// reset translation & rotation
		tx = ty = tz = 0;
		thetaX = thetaY = thetaZ = 0;
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