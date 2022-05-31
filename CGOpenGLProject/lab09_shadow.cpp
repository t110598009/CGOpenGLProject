// Shadow.cpp
// OpenGL SuperBible
// Demonstrates simple planar shadows
// Program by Richard S. Wright Jr.

//以下兩者要自己實作，因牽扯較多檔案
//#include "../../shared/gltools.h" 
//#include "../../shared/math3d.h"
#include "./GL/freeglut.h"
#include "cmath"
#include <iostream>
//#include "math3d.h"

typedef float M3DVector4f[4];
typedef float M3DVector3f[3];
typedef float M3DMatrix44f[16];
// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

// These values need to be available globally
// Light values and coordinates
GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat	 lightPos[] = { -125.0f, 150.0f, -50.0f, 0.0f };
GLfloat	 lightPos2[] = { -50.0f, 150.0f, -50.0f, 0.0f };
GLfloat	 lightPos3[] = { 50.0f, 150.0f, -50.0f, 0.0f };
GLfloat	 lightPos4[] = { 125.0f, 150.0f, -50.0f, 0.0f };
GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };

// Transformation matrix to project shadow
// 光源打到飛機的陰影
// 類似 projectionMatrix
M3DMatrix44f shadowMat;

GLfloat d1[3], d2[3], n[3];
int lightChoice = 1;

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

void icoNormVec(const M3DVector3f point1, const M3DVector3f point2,
    const M3DVector3f point3)
{
    for (int k = 0; k < 3; k++) {
        d1[k] = point1[k] - point2[k];
        d2[k] = point2[k] - point3[k];
    }
    // 取得對於共通點的向量後，要取外積才能 normalize，否則會出錯
    normCrossProd(d1, d2, n);
    glNormal3fv(n);
}

void GetPlaneEquation(M3DVector4f planeEq, const M3DVector3f p1, const M3DVector3f p2, const M3DVector3f p3)
{
    // Get two vectors... do the cross product
    M3DVector3f v1, v2;

    // V1 = p3 - p1
    v1[0] = p3[0] - p1[0];
    v1[1] = p3[1] - p1[1];
    v1[2] = p3[2] - p1[2];

    // V2 = P2 - p1
    v2[0] = p2[0] - p1[0];
    v2[1] = p2[1] - p1[1];
    v2[2] = p2[2] - p1[2];

    // Unit normal to plane - Not sure which is the best way here
    //m3dCrossProduct(planeEq, v1, v2);
    //m3dNormalizeVector(planeEq);
    normCrossProd(v1, v2, planeEq);
    glNormal3fv(planeEq);
    // Back substitute to get D
    planeEq[3] = -(planeEq[0] * p3[0] + planeEq[1] * p3[1] + planeEq[2] * p3[2]);
}

void MakePlanarShadowMatrix(M3DMatrix44f proj, const M3DVector4f planeEq, const M3DVector3f vLightPos)
{
    // These just make the code below easier to read. They will be 
    // removed by the optimizer.	
    float a = planeEq[0];
    float b = planeEq[1];
    float c = planeEq[2];
    float d = planeEq[3];

    float dx = -vLightPos[0];
    float dy = -vLightPos[1];
    float dz = -vLightPos[2];

    // Now build the projection matrix
    proj[0] = b * dy + c * dz;
    proj[1] = -a * dy;
    proj[2] = -a * dz;
    proj[3] = 0.0;

    proj[4] = -b * dx;
    proj[5] = a * dx + c * dz;
    proj[6] = -b * dz;
    proj[7] = 0.0;

    proj[8] = -c * dx;
    proj[9] = -c * dy;
    proj[10] = a * dx + b * dy;
    proj[11] = 0.0;

    proj[12] = -d * dx;
    proj[13] = -d * dy;
    proj[14] = -d * dz;
    proj[15] = a * dx + b * dy + c * dz;
    // Shadow matrix ready
}

////////////////////////////////////////////////
// This function just specifically draws the jet
void DrawJet(int nShadow) //畫出飛機或陰影，nShadow 是陰影顏色
	{
	M3DVector3f vNormal;	// Storeage for calculated surface normal

	// Nose Cone /////////////////////////////
	// Set material color, note we only have to set to black
	// for the shadow once
	if(nShadow == 0)
           glColor3ub(255, 0, 0); //畫飛機
	else
            glColor3ub(139, 0, 0); //nShadow == 1 畫陰影


	// Nose Cone - Points straight down
    // Set material color
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 60.0f);
		glVertex3f(-15.0f, 0.0f, 30.0f);
		glVertex3f(15.0f,0.0f,30.0f);
                
	
        // Verticies for this panel
        {
        M3DVector3f vPoints[3] = {{ 15.0f, 0.0f,  30.0f},
                                        { 0.0f,  15.0f, 30.0f},
                                        { 0.0f,  0.0f,  60.0f}};

        // Calculate the normal for the plane
        //m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]); // 可用lab08中的函式替代
		//glNormal3fv(vNormal);
        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }	


        {
        M3DVector3f vPoints[3] = {{ 0.0f, 0.0f, 60.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { -15.0f, 0.0f, 30.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }


        // Body of the Plane ////////////////////////
        {
        M3DVector3f vPoints[3] = {{ -15.0f, 0.0f, 30.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { 0.0f, 0.0f, -56.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                	
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 0.0f, -56.0f },
                                  { 0.0f, 15.0f, 30.0f },
                                  { 15.0f,0.0f,30.0f }};
	
        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                		
    
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(15.0f,0.0f,30.0f);
		glVertex3f(-15.0f, 0.0f, 30.0f);
		glVertex3f(0.0f, 0.0f, -56.0f);
    
        ///////////////////////////////////////////////
        // Left wing
        // Large triangle for bottom of wing
        {
        M3DVector3f vPoints[3] = {{ 0.0f,2.0f,27.0f },
                                  { -60.0f, 2.0f, -8.0f },
                                  { 60.0f, 2.0f, -8.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        	
        {
        M3DVector3f vPoints[3] = {{ 60.0f, 2.0f, -8.0f},
                                   {0.0f, 7.0f, -8.0f},
                                   {0.0f,2.0f,27.0f }};
                
        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{60.0f, 2.0f, -8.0f},
					{-60.0f, 2.0f, -8.0f},
					{0.0f,7.0f,-8.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{0.0f,2.0f,27.0f},
                                    {0.0f, 7.0f, -8.0f},
                                    {-60.0f, 2.0f, -8.0f}};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        // Tail section///////////////////////////////
        // Bottom of back fin
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-30.0f, -0.50f, -57.0f);
		glVertex3f(30.0f, -0.50f, -57.0f);
		glVertex3f(0.0f,-0.50f,-40.0f);

        {
        M3DVector3f vPoints[3] = {{ 0.0f,-0.5f,-40.0f },
                                  {30.0f, -0.5f, -57.0f},
                                {0.0f, 4.0f, -57.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 4.0f, -57.0f },
                { -30.0f, -0.5f, -57.0f },
                { 0.0f,-0.5f,-40.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }

        {
        M3DVector3f vPoints[3] = {{ 30.0f,-0.5f,-57.0f },
					{ -30.0f, -0.5f, -57.0f },
					{ 0.0f, 4.0f, -57.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{ 0.0f,0.5f,-40.0f },
					{ 3.0f, 0.5f, -57.0f },
					{ 0.0f, 25.0f, -65.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                        
        {
        M3DVector3f vPoints[3] = {{ 0.0f, 25.0f, -65.0f },
					{ -3.0f, 0.5f, -57.0f},
					{ 0.0f,0.5f,-40.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
        {
        M3DVector3f vPoints[3] = {{ 3.0f,0.5f,-57.0f },
					{ -3.0f, 0.5f, -57.0f },
					{ 0.0f, 25.0f, -65.0f }};

        icoNormVec(vPoints[0], vPoints[1], vPoints[2]);
		glVertex3fv(vPoints[0]);
		glVertex3fv(vPoints[1]);
		glVertex3fv(vPoints[2]);
        }
                
                
        glEnd();
	}

void drawShadow()
{
    // Any three points on the ground (counter clockwise order)
    M3DVector3f points[3] = { { -30.0f, -149.0f, -20.0f },
                            { -30.0f, -149.0f, 20.0f },
                            { 40.0f, -149.0f, 20.0f } };
    // 怎麼計算出陰影的矩陣? 給它光源的位置、平面的資訊，從光源的方向往地板打光，
    // 其放射狀的光打到飛機，再計算出陰影。
    // Get the plane equation from three points on the ground
        M3DVector4f vPlaneEquation;
        GetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]); //算兩個向量求外積後正規化得出平面方程式

        // Calculate projection matrix to draw shadow on the ground
        switch (lightChoice) {
        case 1:
            //參數分別為: 陰影矩陣存入位置、平面方程式、自訂的光源
            MakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos);
            break;
        case 2:
            MakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos2);
            break;
        case 3:
            MakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos3);
            break;
        case 4:
            MakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos4);
            break;
        }
}
// Called to draw scene
void RenderScene(void)
    {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the ground, we do manual shading to a darker green
    // in the background to give the illusion of depth
    glBegin(GL_QUADS);
        glColor3ub(0,32,0); //light green ground
        glVertex3f(400.0f, -150.0f, -200.0f);
        glVertex3f(-400.0f, -150.0f, -200.0f);
        glColor3ub(0,255,0);  // make it in green gradient
        glVertex3f(-400.0f, -150.0f, 200.0f);
        glVertex3f(400.0f, -150.0f, 200.0f);
    glEnd();

    // 希望在這之中的 transformation 不會影響其他物件
    // 使用 glPushMatrix() 和 glPopMatrix()，而他們是一組的
    
    // Save the matrix state and do the rotations
    glPushMatrix();

    // Draw jet at new orientation, put light in correct position
    // before rotating the jet
    glEnable(GL_LIGHTING);
    switch (lightChoice) {
    case 1:
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        break;
    case 2:
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
        break;
    case 3:
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos3);
        break;
    case 4:
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos4);
        break;
    }
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    DrawJet(0); //畫飛機

    // Restore original matrix state
    glPopMatrix();	


    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    // 陰影是同一平面，所以關掉深度測試，節省資源等等。但不關結果一樣
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glPushMatrix(); // 291 to 304 lines is a group.

    drawShadow();
    // Multiply by shadow projection matrix
    glMultMatrixf((GLfloat *)shadowMat);

    // Now rotate the jet around in the new flattend space
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Pass true to indicate drawing shadow
    DrawJet(1);	// 畫陰影

    // Restore the projection to normal
    glPopMatrix();

    // Draw the light source
    glPushMatrix();
    switch (lightChoice) {
    case 1:
        glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
        break;
    case 2:
        glTranslatef(lightPos2[0], lightPos2[1], lightPos2[2]);
        break;
    case 3:
        glTranslatef(lightPos3[0], lightPos3[1], lightPos3[2]);
        break;
    case 4:
        glTranslatef(lightPos4[0], lightPos4[1], lightPos4[2]);
        break;
    }
    glColor3ub(255,255,0);
    glutSolidSphere(5.0f,10,10); //前面有先註銷光影，所以這光點就不會有被打光的效果
    glPopMatrix();

    // Restore lighting state variables
    glEnable(GL_DEPTH_TEST); //重啟深度測試

    // Display the results
    glutSwapBuffers();
    }

// This function does any needed initialization on the rendering
// context. 
void SetupRC()
    {
    // Any three points on the ground (counter clockwise order)
    M3DVector3f points[3] = {{ -30.0f, -149.0f, -20.0f },
                            { -30.0f, -149.0f, 20.0f },
                            { 40.0f, -149.0f, 20.0f }};

    glEnable(GL_DEPTH_TEST);	// Hidden surface removal
    glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
    glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

    // Setup and enable light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
    glEnable(GL_LIGHT0);

    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);
	
    // Set Material properties to follow glColor values
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // All materials hereafter have full specular reflectivity
    // with a high shine
    glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
    glMateriali(GL_FRONT,GL_SHININESS,128);

    // Light blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

    // Draw jet's shadow
    drawShadow();

    // Can copy this function to draw jet's shadow
    glEnable(GL_NORMALIZE);
    }

void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        xRot-= 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot += 5.0f;

    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    if(key > 356.0f)
        xRot = 0.0f;

    if(key < -1.0f)
        xRot = 355.0f;

    if(key > 356.0f)
        yRot = 0.0f;

    if(key < -1.0f)
        yRot = 355.0f;

    // Refresh the Window
    glutPostRedisplay();
    }

void myKeyBoard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        lightChoice = 1;
        break;
    case '2':
        lightChoice = 2;
        break;
    case '3':
        lightChoice = 3;
        break;
    case '4':
        lightChoice = 4;
        break;
    } 
    std::cout << "Change light source: " << lightChoice << std::endl;
    // Refresh the Window
    glutPostRedisplay();
}

void ChangeSize(int w, int h)
    {
    GLfloat fAspect;

    // Prevent a divide by zero
    if(h == 0)
        h = 1;

	
    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    // Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    fAspect = (GLfloat)w/(GLfloat)h;
    gluPerspective(60.0f, fAspect, 200.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move out Z axis so we can see everything
    glTranslatef(0.0f, 0.0f, -400.0f);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
    }

int main(int argc, char* argv[])
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("lab09 Shadow");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(myKeyBoard);
    glutDisplayFunc(RenderScene);
    SetupRC();
    glutMainLoop();

    return 0;
    }
