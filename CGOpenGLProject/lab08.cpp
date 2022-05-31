#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
/***freeglut***/
#include <.\GL\freeglut.h>
#include <cmath>

#define X .525731112119133606
#define Z .850650808352039932
#define Pi 3.1415928
#define PI 3.14159265
static GLfloat vdata[12][3] = { //12個點
{-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
{Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

static GLuint tindices[20][3] = { //20個面
{1,4,0}, {4,9,0}, {4,9,5}, {8,5,4}, {1,8,4},
{1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
{3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
{10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};
GLfloat d1[3], d2[3], n[3];

int window_width = 900, window_height = 400;
int Depth = 1;
double thetaX, thetaY = 0;
bool isPolygonFill = true; // false equal just use line.

void SetupRC();
void ChangeSize(int, int);
void RenderScene(void);
void display();
void myKeyboard(unsigned char, int, int);
void mySepcialKey(int, int, int);
void do_rotateX(GLdouble);
void do_rotateY(GLdouble);

void drawTriangle(GLfloat v1[], GLfloat v2[], GLfloat v3[]) {
    // 球形的好處在於，其法向量等於頂點的 xyz
    glNormal3fv(&v1[0]);
    glVertex3fv(&v1[0]);
    glNormal3fv(&v2[0]);
    glVertex3fv(&v2[0]);
    glNormal3fv(&v3[0]);
    glVertex3fv(&v3[0]);
}

void normalize(GLfloat v[]) { // 讓向量變成 unique vector
    GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); // 取得向量長度
    v[0] /= d; v[1] /= d; v[2] /= d; // 向量除該向量長度
}

void normCrossProd(float u[3], float v[3], float out[3])
{
    out[0] = u[1] * v[2] - u[2] * v[1];
    out[1] = u[2] * v[0] - u[0] * v[2];
    out[2] = u[0] * v[1] - u[1] * v[0];
    normalize(out); // 讓向量變成 unique vector
}

void icoNormVec(int i)
{
    for (int k = 0; k < 3; k++) {
        d1[k] = vdata[tindices[i][0]][k] - vdata[tindices[i][1]][k];
        d2[k] = vdata[tindices[i][1]][k] - vdata[tindices[i][2]][k];
    }
    // 取得對於共通點的向量後，要取外積才能 normalize，否則會出錯
    normCrossProd(d1, d2, n);
    glNormal3fv(n);
}

void subdivide(GLfloat v1[], GLfloat v2[], GLfloat v3[], int depth) {
    GLfloat v12[3], v23[3], v31[3]; int i;
    if (depth == 0) { // 遞迴到深度為零就畫出三角形 (終止條件)
        drawTriangle(v1, v2, v3);
        return;
    }

    for (i = 0; i < 3; i++) {
        v12[i] = (v1[i] + v2[i]) / 2.0;
        v23[i] = (v2[i] + v3[i]) / 2.0;
        v31[i] = (v3[i] + v1[i]) / 2.0;
    }

    normalize(v12);
    normalize(v23);
    normalize(v31);

    subdivide(v1, v12, v31, depth - 1);
    subdivide(v2, v23, v12, depth - 1);
    subdivide(v3, v31, v23, depth - 1);
    subdivide(v12, v23, v31, depth - 1);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 400);
    glutInitWindowPosition(500, 200);
    glutCreateWindow("Lighting and Subdivion");

    SetupRC(); // Light
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySepcialKey);
    

    // 3個 viewport : 1.flat 2.smooth 3.subdivision
    std::cout << "鍵盤上的 1: polygon fill、2: polygon line" << std::endl;
    std::cout << "按下方向鍵進行旋轉" << std::endl;
    std::cout << "數字鍵+ - 決定 Subdivision 的 depth" << std::endl;
    glutMainLoop();
    return 0;
}

void SetupRC()
{
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    //GLfloat light_position[] = { -1.0, 1.0, -1.0, 1.0 }; //第四點為 0 = 純方向(太陽光); 1 = 點光源
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_diffuse[] = { 0.8, 0.6, 0.4, 1.0 };
    GLfloat mat_ambient[] = { 0.8, 0.6, 0.4, 1.0 };
    GLfloat mat_shininess = { 20.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); // 塑膠感
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);// 金屬感
    glShadeModel(GL_SMOOTH); /*enable smooth shading */
    glEnable(GL_LIGHTING); /* enable lighting */
    glEnable(GL_LIGHT0); /* enable light 0 */
}

void ChangeSize(int w, int h)
{
    window_width = w;
    window_height = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, 2, -2, 2, -3, 3);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderScene(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 3.0f, 0, 0, 0, 0, 1, 0);

    do_rotateX(thetaX);
    do_rotateY(thetaY);

    // viewport1 - Left - flat shading
    glViewport(0, 0, window_width / 3, window_height);
    
    glShadeModel(GL_FLAT);
    if (isPolygonFill)
        glBegin(GL_TRIANGLES);
    else
        glBegin(GL_LINES);
    for (int i = 0; i < 20; i++)
    {

        icoNormVec(i);
        glVertex3fv(&vdata[tindices[i][0]][0]);
        glVertex3fv(&vdata[tindices[i][1]][0]);
        glVertex3fv(&vdata[tindices[i][2]][0]);
    }
    glEnd();

    // viewport2 - Center - smooth shading
    glViewport(window_width / 3, 0, window_width / 3, window_height);
    glShadeModel(GL_SMOOTH);
    if (isPolygonFill)
        glBegin(GL_TRIANGLES);
    else
        glBegin(GL_LINES);
    for (int i = 0; i < 20; i++) //每一頂點都算一次法向量，會變得 smooth，計算量也上升
    {
        // 球形的好處在於，其法向量等於頂點的 xyz
        glNormal3fv(&vdata[tindices[i][0]][0]);
        glVertex3fv(&vdata[tindices[i][0]][0]);
        glNormal3fv(&vdata[tindices[i][1]][0]);
        glVertex3fv(&vdata[tindices[i][1]][0]);
        glNormal3fv(&vdata[tindices[i][2]][0]);
        glVertex3fv(&vdata[tindices[i][2]][0]);
    }
    glEnd();

    // viewport3 - Right - Subdivision
    glViewport(window_width - window_width / 3, 0, window_width / 3, window_height);
    glShadeModel(GL_SMOOTH);
    if (isPolygonFill)
        glBegin(GL_TRIANGLES);
    else
        glBegin(GL_LINES);

    display();
    glEnd();

    glutSwapBuffers();
}

void display(void)
{
    for (int i = 0; i < 20; i++) {

        subdivide(&vdata[tindices[i][0]][0],
            &vdata[tindices[i][1]][0],
            &vdata[tindices[i][2]][0],
            Depth);
    }
    glFlush();
}

void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':
        isPolygonFill = true;
        break;
    case '2':
        isPolygonFill = false;
        break;
    case '+':
        Depth += 1;
        break;
    case '-':
        if (Depth > 0)
            Depth -= 1;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void mySepcialKey(int key, int x, int y)
{
    // Rotation
    switch (key)
    {
    case GLUT_KEY_LEFT:
        thetaY += 3;
        break;
    case GLUT_KEY_RIGHT:
        thetaY -= 3;
        break;
    case GLUT_KEY_UP:
        thetaX += 3;
        break;
    case GLUT_KEY_DOWN:
        thetaX -= 3;
        break;
    default:
        break;
    }
    glutPostRedisplay();
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