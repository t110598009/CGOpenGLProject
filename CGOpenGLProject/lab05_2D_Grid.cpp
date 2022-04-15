#include <iostream>
#include <GL/freeglut.h>
#include <math.h>
#include <vector>
using namespace std;
#define PI 3.1415926

void SetupRC();
void ChangeSize(int, int);
void RenderScene(void);
void drawGrid(float, int);
void createGLUTMenus();
void Menu(int);
void ChangeSizeMenu(int);
void my_Mouse(int, int, int, int);
int draw = 0;

int x_min;
int y_min;
int x_max;
int y_max;

int x_one[20];
int y_one[20];
int x_two[20];
int y_two[20];
int x_count = 0;
int y_count = 0;

float gridSize = 10;
float mouse_X = 0;
float mouse_Y = 0;

int window_width = 400;
int window_height = 400;
float mouse_offset_x;
float mouse_offset_y;

bool isClicked = false;
double mouseWX = 0, mouseWY = 0, mouseWZ = 0;
GLdouble projectionMatrix[16];
GLdouble modelViewMatrix[16];
int viewport[4];

int main(int argc, char** argv)
{
	// These are variable that you will need
	// to move your cube
	// about translation and rotation
	/*tx = ty = tz = 0;
	thetaX = thetaY = thetaZ = 0;*/

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(600, 80);
	glutCreateWindow("Draw 2D Grid");
	SetupRC();

	createGLUTMenus();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMouseFunc(my_Mouse);
	/*glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySepcialKey);*/

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
	//glOrtho(-10, 10, -10, 10, -10, 20);
	glOrtho(-20, 20, -20, 20, -10, 20);

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
	gluLookAt(0.0f, 0.0f, 5.0f, 0, 0, 0, 0, 1, 0);

	//
	drawGrid(gridSize, gridSize * 2 );
	

	//
	if (isClicked) {
		if (abs(x_min) > gridSize || abs(x_max) > gridSize ||
			abs(y_min) > gridSize || abs(y_max) > gridSize)
			return; 
		
		glColor3f(1.0f, 0.0f, 1.0f);
		glBegin(GL_POLYGON);
		glVertex3f(x_min, y_min, 0);
		glVertex3f(x_max, y_min, 0);
		glVertex3f(x_max, y_max, 0);
		glVertex3f(x_min, y_max, 0);
		glEnd();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		isClicked = false;
	}

	glFlush();
	glutSwapBuffers();
}

void drawGrid(float size, int nbSubdivisions) {
	GLboolean lighting;
	glGetBooleanv(GL_LIGHTING, &lighting);
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	for (int i = 0; i <= nbSubdivisions; ++i) {
		const float pos = size * (2.0 * i / nbSubdivisions - 1.0);
		glVertex2d(pos, -size);
		glVertex2d(pos, size);
		glVertex2d(-size, pos);
		glVertex2d(size, pos);
	}
	glEnd();
	if (lighting)
		glEnable(GL_LIGHTING);
}

void createGLUTMenus() {
	int Change_size;

	Change_size = glutCreateMenu(ChangeSizeMenu);
	glutAddMenuEntry("10 x 10", 1);
	glutAddMenuEntry("15 x 15", 2);
	glutAddMenuEntry("20 x 20", 3);
}

void ChangeSizeMenu(int option) {
	switch (option)
	{
	case 1:
		gridSize = (float)10;
		//drawGrid(10, 20);
		break;
	case 2:
		gridSize = (float)15;
		//drawGrid(16, 256);
		break;
	case 3:
		gridSize = (float)20;
		//drawGrid(20, 400);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void my_Mouse(int button, int state, int x, int y) {
	mouse_offset_x = (400 / 2) / gridSize;
	mouse_offset_y = (400 / 2) / gridSize;

	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {

			//std::cout << "(" << x << "," << y << ")" << std::endl;
			//mouseWX = 1; mouseWY = mouseWZ = 0;
			float winX = (float)x;
			float winY = (float)y;
			float screenZ = 1;

			float mouseX = ((float)x / window_width) * 2 - 1;
			float mouseY = -( ((float)y / window_height) * 2 - 1 );
			/*mouseX = float(x - (window_width / 2)) / mouse_offset_x;
			mouseY = float((window_height / 2) - y) / mouse_offset_y;*/
			mouseX *= 10 ;
			mouseY *= 10 ;
			glReadPixels(winX, viewport[3] - winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screenZ);
			gluUnProject(winX, viewport[3] - winY, screenZ, modelViewMatrix, projectionMatrix, viewport
				, &mouseWX, &mouseWY, &mouseWZ);
			//std::cout << "(" << mouseX << "," << mouseY << ")" << std::endl;
			//std::cout << "(" << mouseWX << "," << mouseWY << ")" << std::endl;
			x_min = floor(mouseWX);
			y_min = floor(mouseWY);
			x_max = ceil(mouseWX);
			y_max = ceil(mouseWY);

			if (abs(x_min) > gridSize || abs(x_max) > gridSize ||
				abs(y_min) > gridSize || abs(y_max) > gridSize)
				return;
			/*
			if (x_min % 2 == 0) {
				x_min -= 1;
			}
			if (y_min % 2 == 0) {
				y_min -= 1;
			}
			if (x_max % 2 == 0) {
				x_max += 1;
			}
			if (y_max % 2 == 0) {
				y_max += 1;
			}*/
			//cout << "Grid座標 : (" << x_max << " " << x_min << " , " << y_max << " " << y_min << endl;
			//cout << "Grid座標 : (" << ((x_max + x_min) / 4) << "," << ((y_max + y_min) / 4) << ")" << endl;
			cout << "Grid座標 : (" << (int)ceil((float)(x_max + x_min) / 2.0)   << "," << (int)ceil((float)(y_max + y_min) / 2.0)  << ")" << endl;
			isClicked = true;
			/*draw++;

			mouse_X = float(x - (window_width / 2)) / mouse_offset_x;
			mouse_Y = float((window_height / 2) - y) / mouse_offset_y;

			x_min = floor(mouse_X);
			y_min = floor(mouse_Y);
			x_max = ceil(mouse_X);
			y_max = ceil(mouse_Y);

			if (x_min % 2 == 0) {
				x_min -= 1;
			}
			if (y_min % 2 == 0) {
				y_min -= 1;
			}
			if (x_max % 2 == 0) {
				x_max += 1;
			}
			if (y_max % 2 == 0) {
				y_max += 1;
			}

			cout << "Grid座標 : (" << (x_max + x_min) / 4 << "," << (y_max + y_min) / 4 << ")" << endl;
			if (draw == 1) {
				x_one[x_count / 2] = (x_max + x_min) / 4;
				y_one[y_count / 2] = (y_max + y_min) / 4;
				x_count++;
				y_count++;
				cout << "x_one" << endl;
				cout << draw << endl;
			}
			else if (draw == 2) {
				x_two[(x_count / 2)] = (x_max + x_min) / 4;
				y_two[(y_count / 2)] = (y_max + y_min) / 4;
				x_count++;
				y_count++;
				cout << "x_two" << endl;
				cout << draw << endl;
			}*/
		}
	}
}