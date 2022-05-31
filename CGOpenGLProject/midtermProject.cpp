#include <iostream>
#include <GL/freeglut.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;
#define PI 3.1415926

struct vertex {
	float x, y;
	float r, g, b;
	bool isNull = true;
	bool isScan = false;
};

float randomValue() {
	/* 產生亂數 */
	return (float)(rand() % 11) / 10;
}

void SetupRC();
void ChangeSize(int, int);
void RenderScene(void);
void drawGrid(float, int);
void createGLUTMenus();
void Menu(int);
void ChangeSizeMenu(int);
void DrawLineMenu(int);
void mainMenu(int);
void my_Mouse(int, int, int, int);
void MidPointLine(int x1, int y1, int x2, int y2);
void AntiAliasingLine(int x1, int y1, int x2, int y2);
void DrawPixel(int, int, float);
void DrawPixelByVertex(vertex);
void setPixel(int, int);
void crow(vertex[], int);
void ScanY(vertex[], int, int);
void ScanX(vertex*, vertex*, int);
void differenceX(vertex*, vertex*, vertex*, vertex*, int);
void differenceY(vertex*, vertex*, vertex*, vertex*, int);
void difference(vertex*, vertex*, vertex*, vertex*, float, float);
void increment(vertex*, vertex*);
bool midPoint = 0;
bool AntiPoint = 0;
bool crowRasterizing = 0;

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

bool isClicked = false;
double mouseWX = 0, mouseWY = 0, mouseWZ = 0;
GLdouble projectionMatrix[16];
GLdouble modelViewMatrix[16];
int viewport[4];
vertex vlist[5];

int main(int argc, char** argv)
{
	/*
	* Start from following lab:
	* lab05: Create 2D grid and draw point by mouse click.
	* lab06: Draw Line: Midpoint algorithm 
	* and anti-aliasing algorithm.
	*/

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(550, 550);
	glutInitWindowPosition(600, 80);
	glutCreateWindow("Midterm project");
	SetupRC();

	createGLUTMenus();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMouseFunc(my_Mouse);

	/* 設定亂數種子 */
	srand(time(NULL));

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
	// Enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); //load the projection matrix
	glLoadIdentity();
	// Viewing volume
	// left:-10; right:10; bottom:-10; top:10
	// 右手定則: -z 朝自己; zNear:-10; zFar:20
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

	// 劃出 2D 框框
	drawGrid(gridSize, gridSize * 2);


	// 滑鼠是否點擊
	if (isClicked && !crowRasterizing) { // no need to do here, when crow rasterizing.
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

		isClicked = false;
	}

	if (midPoint == true && y_count / 2 > 0) {
		//cout << "Enter midPoint" << endl;
		for (int i = 0; i < y_count / 2; i++) {
			MidPointLine(x_one[i], y_one[i], x_two[i], y_two[i]);
		}
	}

	if (AntiPoint == true && y_count / 2 > 0) {
		for (int i = 0; i < y_count / 2; i++) {
			AntiAliasingLine(x_one[i], y_one[i], x_two[i], y_two[i]);
		}
	}
	const int listSize = sizeof(vlist) / sizeof(vlist[0]);
	for (int i = 0; i < listSize; i++) {
		if (!vlist[i].isNull) {
			DrawPixelByVertex(vlist[i]); //有值就畫點出來的頂點

		}
	}
	if (!vlist[listSize - 1].isNull) { //擁有5個點就畫
		cout << "Doing Crow's Algorithm..." << endl;
		crow(vlist, listSize);
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
	int Change_size, Draw_Line;

	Change_size = glutCreateMenu(ChangeSizeMenu);
	glutAddMenuEntry("10 x 10", 1);
	glutAddMenuEntry("15 x 15", 2);
	glutAddMenuEntry("20 x 20", 3);
	Draw_Line = glutCreateMenu(DrawLineMenu);
	glutAddMenuEntry("MidPoint", 1);
	glutAddMenuEntry("MidPoint and Anti-aliasing", 2);
	glutAddMenuEntry("Anti-aliasing", 3);

	int memu = glutCreateMenu(mainMenu);
	glutAddSubMenu("Change_size", Change_size);
	glutAddSubMenu("Line Algorithm", Draw_Line);
	glutAddMenuEntry("Polygon", 1);
	glutAddMenuEntry("reset", 2);
	glutAddMenuEntry("exit", 3);

}

void ChangeSizeMenu(int option) {
	switch (option)
	{
	case 1:
		gridSize = (float)10;
		break;
	case 2:
		gridSize = (float)15;
		break;
	case 3:
		gridSize = (float)20;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void DrawLineMenu(int option) {
	switch (option)
	{
	case 1:
		midPoint = true;
		AntiPoint = false;
		x_count = 0;
		y_count = 0;
		break;
	case 2:
		midPoint = true;
		AntiPoint = true;
		x_count = 0;
		y_count = 0;
		break;
	case 3:
		midPoint = false;
		AntiPoint = true;
		x_count = 0;
		y_count = 0;
		break;
	}
	glutPostRedisplay();
}

void mainMenu(int option) {
	switch (option)
	{
	case 1:
		crowRasterizing = true;
		x_count = 0;
		y_count = 0;
		if (!vlist[0].isNull) {
			int i = 5;
			while (i--)
				vlist[i].isNull = true;
		}
		break;
	case 2:// reset
		if (!vlist[0].isNull) {
			int i = 5;
			while (i--)
				vlist[i].isNull = true;
		}
		crowRasterizing = false;
		midPoint = false;
		AntiPoint = false;
		x_count = 0;
		y_count = 0;
		break;
	case 3:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void my_Mouse(int button, int state, int x, int y) {

	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {

			float winX = (float)x;
			float winY = (float)y;
			float screenZ = 1;
			const int listSize = sizeof(vlist) / sizeof(vlist[0]);
			cout << listSize << endl;

			glReadPixels(winX, viewport[3] - winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screenZ);
			gluUnProject(winX, viewport[3] - winY, screenZ, modelViewMatrix, projectionMatrix, viewport
				, &mouseWX, &mouseWY, &mouseWZ);

			x_min = floor(mouseWX);
			y_min = floor(mouseWY);
			x_max = ceil(mouseWX);
			y_max = ceil(mouseWY);

			// 超出目前設定的邊框
			if (abs(x_min) > gridSize || abs(x_max) > gridSize ||
				abs(y_min) > gridSize || abs(y_max) > gridSize)
				return;

			cout << "Grid座標 : (" << (int)ceil((float)(x_max + x_min) / 2.0) << "," << (int)ceil((float)(y_max + y_min) / 2.0) << ")" << endl;
			isClicked = true;

			if (crowRasterizing && x_count < 5) {
				vlist[x_count].x = x_min;
				vlist[x_count].y = y_min;
				vlist[x_count].r = randomValue();
				vlist[x_count].g = randomValue();
				vlist[x_count].b = randomValue();
				vlist[x_count].isNull = false;
				cout << x_count << " - r:" << vlist[x_count].r << " g:" << vlist[x_count].g << " b:" << vlist[x_count].b << endl;
			}

			if (x_count % 2 == 0) {
				x_one[x_count / 2] = x_min;
				y_one[y_count / 2] = y_min;
				x_count++;
				y_count++;
				cout << "x_one" << endl;
				//cout << draw << endl;
			}
			else if (x_count % 2 == 1) {
				x_two[x_count / 2] = x_min;
				y_two[y_count / 2] = y_min;
				x_count++;
				y_count++;
				cout << "x_two" << endl;
				//cout << draw << endl;
			}
			
		}
	}
}
void MidPointLine(int x0, int y0, int x1, int y1) {

	if (x0 > x1)
	{
		MidPointLine(x1, y1, x0, y0);
		return;
	}


	int slopex = 1, slopey = 1, i = 0;
	int dx = x1 - x0, dy = y1 - y0;
	int d, delE, delNE;

	int x = x0, y = y0;
	DrawPixel(x, y, 1);

	if (dy < 0) dy = -dy;

	if (y1 < y0) {
		slopey = -1;
	}
	//cout << "dy:" << dy << endl;


	if (dx > dy) {
		cout << slopey << endl;
		d = 2 * dy - dx;
		delE = 2 * dy;
		delNE = 2 * (dy - dx);
		while (i < dx) {
			if (d <= 0) {
				d += delE;
			}
			else {
				d += delNE;
				y += slopey;
			}
			x++;
			i++;
			DrawPixel(x, y, 1);
			cout << x << " " << y << endl;
		}
	}
	else {
		d = 2 * dx - dy;
		delE = 2 * dx;
		delNE = 2 * (dx - dy);
		//cout << "y=" << y << "  y1= " << y1 << endl;
		while (i < dy) {
			if (d <= 0) {
				d += delE;
			}
			else {
				d += delNE;
				x++;
			}
			y += slopey;
			i++;
			DrawPixel(x, y, 1);
			cout << x << " " << y << endl;
		}
	}
}

void AntiAliasingLine(int x0, int y0, int x1, int y1) {

	if (x0 > x1)
	{
		AntiAliasingLine(x1, y1, x0, y0);
		return;
	}

	float dx = x1 - x0;
	float dy = y1 - y0;
	float m = dy / dx;
	float x = x0, y = y0;
	if (dx > dy) { // 向兩側擴展大於上下擴展
		for (x = x0; x <= x1; x++) {
			int yi = floor(y);
			float f = y - yi;
			DrawPixel(x, yi, 1 - f);
			DrawPixel(x, yi + 1, f);
			y = y + m;
		}
	}
	else {
		m = dx / dy;
		for (y = y0; y <= y1; y++) {
			int xi = floor(x);
			float f = x - xi;
			DrawPixel(xi, y, 1 - f);
			DrawPixel(xi + 1, y, f);
			x = x + m;
		}
	}

}

void DrawPixel(int x, int y, float alpha) {
	// x and y always take a grid min point.
	glBegin(GL_POLYGON);
	if (alpha != 0) {
		glColor4f(1.0f, 0.0f, 1.0f, alpha);
		glVertex3f(x, y, 0);
		glVertex3f(x + 1, y, 0);
		glVertex3f(x + 1, y + 1, 0);
		glVertex3f(x, y + 1, 0);
	}
	glEnd();
}

void DrawPixelByVertex(vertex v) {
	glBegin(GL_POLYGON);

	glColor3f(v.r, v.g, v.b);
	glVertex3f(v.x, v.y, 0);
	glVertex3f(v.x + 1, v.y, 0);
	glVertex3f(v.x + 1, v.y + 1, 0);
	glVertex3f(v.x, v.y + 1, 0);

	glEnd();
}

void setPixel(int x, int y) {
	// just only setting x, y for pixel.
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0);
	glVertex3f(x + 1, y, 0);
	glVertex3f(x + 1, y + 1, 0);
	glVertex3f(x, y + 1, 0);
	glEnd();
}

// Crow Algorithm
void crow(vertex vList[], int n)
{
	int iMin = 0;
	for (int i = 1; i < n; i++)
		if (vList[i].y < vList[iMin].y)
			iMin = i;
	
	ScanY(vList, n, iMin);
}

void ScanY(vertex vList[], int n, int i)
{
	int li, ri; // left & right upper endpoint indices
	int ly, ry; // left & right upper endpoint y values
	vertex l, dl; // current left edge and delta
	vertex r, dr; // current right edge and delta
	int rem; // number of remaining vertices
	int y; // current scanline
	li = ri = i;
	ly = ry = y = ceil(vList[i].y);


	for (rem = n; rem > 0; )
	{
		// step 2.
		// find appropriate left edge
		while (ly <= y && rem > 0)
		{
			rem--;
			i = li - 1;
			if (i < 0)
				i = n - 1; // go clockwise
			ly = ceil(vList[i].y);
			if (ly > y) // replace left edge
				differenceY(&vList[li], &vList[i], &l, &dl, y);
			li = i;

		}
		// find appropriate right edge
		while (ry <= y && rem > 0)
		{
			rem--;
			i = ri + 1;
			if (i >= n)
				i = 0; // go clockwise
			ry = ceil(vList[i].y);
			if (ry > y) // replace left edge
				differenceY(&vList[ri], &vList[i], &r, &dr, y);
			ri = i;

		}
		// step3.
		for (; y < ly && y < ry; y++) // while l & r span y (the current scanline)
		{
			glColor3f(l.r, l.g, l.b);
			glBegin(GL_LINES);
			glVertex3f(l.x + 0.5, y + 0.5, 1);
			glVertex3f(l.x + dl.x + 0.5, y + 1 + 0.5, 1);
			glEnd();

			glColor3f(r.r, r.g, r.b);
			glBegin(GL_LINES);
			glVertex3f(r.x + 0.5, y + 0.5, 1);
			glVertex3f(r.x + dr.x + 0.5, y + 1 + 0.5, 1);
			glEnd();

			ScanX(&l, &r, y); // draw the span
			increment(&l, &dl);
			increment(&r, &dr);
		}
	}

}

void differenceY(vertex* v1, vertex* v2, vertex* e, vertex* de, int y) {
	difference(v1, v2, e, de, (v2->y - v1->y), y - v1->y);
}
void differenceX(vertex* v1, vertex* v2, vertex* e, vertex* de, int x) {
	difference(v1, v2, e, de, (v2->x - v1->x), x - v1->x);
}

void difference(vertex* v1, vertex* v2, vertex* e, vertex* de, float d, float f) {
	de->x = (v2->x - v1->x) / d;
	e->x = v1->x + f * de->x;
	de->r = (v2->r - v1->r) / d;
	e->r = v1->r + f * de->r;
	de->g = (v2->g - v1->g) / d;
	e->g = v1->g + f * de->g;
	de->b = (v2->b - v1->b) / d;
	e->b = v1->b + f * de->b;
}

void ScanX(vertex* l, vertex* r, int y)
{
	int x, lx, rx;
	vertex s, ds;
	lx = ceil(l->x + 0.1);
	rx = ceil(r->x + 0.1);
	
	if (lx < rx)
		differenceX(l, r, &s, &ds, lx);
    
	for (x = lx; x < rx; x++) {
		glColor3f(s.r, s.g, s.b);
		setPixel(x, y);
		increment(&s, &ds);
	}
}

void increment(vertex* v, vertex* dv)
{
	v->x += dv->x;
	v->r += dv->r;
	v->g += dv->g;
	v->b += dv->b;
}


