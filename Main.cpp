#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
#include "loadTGA.h"
using namespace std;

// CONSTANTS //
#define GL_CLAMP_TO_EDGE 0x812F   //To get rid of seams between textures
#define TO_RAD (3.14159265/180.0);  //Conversion from degrees to radians

//--Globals ---------------------------------------------------------------
float *x, *y, *z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles

float cam_x, cam_y, cam_z;
float cam_angle = 0;

GLuint texId[7];
enum Texture { SKYBOX_LEFT, SKYBOX_FRONT, SKYBOX_RIGHT, SKYBOX_BACK, SKYBOX_TOP, SKYBOX_BOTTOM, SAND };

void loadGLTextures()				// Load bitmaps And Convert To Textures
{
	glGenTextures(7, texId); 		// Create texture ids
	// *** left ***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_LEFT]);
	loadTGA("textures/skybox/left.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** front ***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_FRONT]);
	loadTGA("textures/skybox/front.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** right ***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_RIGHT]);
	loadTGA("textures/skybox/right.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** back***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_BACK]);
	loadTGA("textures/skybox/back.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** top ***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_TOP]);
	loadTGA("textures/skybox/top.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// *** down ***
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_BOTTOM]);
	loadTGA("textures/skybox/down.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, texId[SAND]);
	loadTGA("textures/sand.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname) {
	ifstream fp_in;
	int num, ne;

	fp_in.open(fname, ios::in);
	if(!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nvrt >> ntri >> ne;			    // read number of vertices, polygons, edges

	x = new float[nvrt];                        //create arrays
	y = new float[nvrt];
	z = new float[nvrt];

	t1 = new int[ntri];
	t2 = new int[ntri];
	t3 = new int[ntri];

	for(int i=0; i < nvrt; i++)                         //read vertex list
		fp_in >> x[i] >> y[i] >> z[i];

	for(int i=0; i < ntri; i++)                         //read polygon list
	{
		fp_in >> num >> t1[i] >> t2[i] >> t3[i];
		if(num != 3)
		{
			cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
			exit(1);
		}
	}

	fp_in.close();
	cout << " File successfully read." << endl;
}

//--Function to compute the normal vector of a triangle with index tindx ----------
void normal(int tindx) {
	float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
	float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
	float z1 = z[t1[tindx]], z2 = z[t2[tindx]], z3 = z[t3[tindx]];
	float nx, ny, nz;
	nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
	ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
	nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
	glNormal3f(-nx, -ny, -nz);
}

void skybox(){
	glEnable(GL_TEXTURE_2D);

	////////////////////// LEFT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_LEFT]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f(-1000,  0, 1000);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 0., -1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000., -1000);
		glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000, 1000);
	glEnd();

	////////////////////// FRONT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_FRONT]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f(-1000,  0, -1000);
		glTexCoord2f(0.0, 0.0); glVertex3f(1000, 0., -1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(1000, 1000, -1000);
		glTexCoord2f(1.0, 1.0); glVertex3f(-1000,  1000, -1000);
	glEnd();

	////////////////////// RIGHT WALL ///////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_RIGHT]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f(1000,  0, -1000);
		glTexCoord2f(0.0, 0.0); glVertex3f(1000, 0, 1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(1000, 1000,  1000);
		glTexCoord2f(1.0, 1.0); glVertex3f(1000,  1000,  -1000);
	glEnd();


	////////////////////// REAR WALL ////////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_BACK]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1000, 0, 1000);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 0,  1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000,  1000);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1000, 1000, 1000);
	glEnd();

	/////////////////////// TOP //////////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_TOP]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000, -1000);
		glTexCoord2f(1.0, 0.0); glVertex3f(1000, 1000,  -1000);
		glTexCoord2f(0.0, 0.0); glVertex3f(1000, 1000,  1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000, 1000);
	glEnd();

	/////////////////////// FLOOR //////////////////////////
	glBindTexture(GL_TEXTURE_2D, texId[SKYBOX_BOTTOM]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 0., 1000);
		glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0.,  1000);
		glTexCoord2f(1.0, 1.0); glVertex3f(1000, 0., -1000);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 0., -1000);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

//----------draw a floor plane-------------------
/*
void drawFloor() {
	bool flag = false;

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	for(int x = -400; x <= 400; x += 20)
	{
		for(int z = -400; z <= 400; z += 20)
		{
			if(flag) glColor3f(0.6, 1.0, 0.8);
			else glColor3f(0.8, 1.0, 0.6);
			glVertex3f(x, 0, z);
			glVertex3f(x, 0, z+20);
			glVertex3f(x+20, 0, z+20);
			glVertex3f(x+20, 0, z);
			flag = !flag;
		}
	}
	glEnd();
}
*/

void drawFloor() {
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texId[SAND]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 16.0); glVertex3f(-400, 0, -400);
		glTexCoord2f(0.0, 0.0); glVertex3f(-400, 0, 400);
		glTexCoord2f(16.0, 0.0); glVertex3f(400, 0, 400);
		glTexCoord2f(16.0, 16.0); glVertex3f(400, 0, -400);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawPyramid() {
	glColor3f(0.8, 0.8, 0);

	//Construct the object model here using triangles read from OFF file
	glBegin(GL_TRIANGLES);
	for(int tindx = 0; tindx < ntri; tindx++)
	{
		normal(tindx);
		glVertex3d(x[t1[tindx]], y[t1[tindx]], z[t1[tindx]]);
		glVertex3d(x[t2[tindx]], y[t2[tindx]], z[t2[tindx]]);
		glVertex3d(x[t3[tindx]], y[t3[tindx]], z[t3[tindx]]);
	}
	glEnd();
}

//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display() {
	float lpos[4] = {-400.0, 100.0, 200.0, 1.0};  //light's position

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(cam_x, cam_y, cam_z, cam_x + cos(cam_angle), cam_y, cam_z + sin(cam_angle), 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);   //set light position

	drawFloor();

	glPushMatrix();
		glTranslatef(250, 0, 0);
		glScalef(100, 200, 100);
		drawPyramid();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, -2000, 0);
		glScalef(5, 4, 5);
		skybox();
	glPopMatrix();

	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize() {
	loadGLTextures();
	loadMeshFile("./models/Pyramid.off");				//Specify mesh file name here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 1, 10000);  //The camera view volume

	// Set initial camera position & angle //
	cam_x = -50;
	cam_y = 30;
	cam_z = -250;
	cam_angle = 45.0*TO_RAD
}

void special(int key, int x, int y) {
	const float CHANGE_VIEW_ANGLE = 1.0;
	const float MOVE_DISTANCE = 2.0;

	switch (key) {
		case GLUT_KEY_LEFT:
			cam_angle -= CHANGE_VIEW_ANGLE * TO_RAD;
			break;
		case GLUT_KEY_RIGHT:
			cam_angle += CHANGE_VIEW_ANGLE * TO_RAD;
			break;
		case GLUT_KEY_UP:
			cam_x += MOVE_DISTANCE * cos(cam_angle);
			cam_z += MOVE_DISTANCE * sin(cam_angle);
			break;
		case GLUT_KEY_DOWN:
			cam_x -= MOVE_DISTANCE * cos(cam_angle);
			cam_z -= MOVE_DISTANCE * sin(cam_angle);
			break;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case ' ':
			cam_y++;
			break;
		case 'x':
			cam_y--;
			break;
	}
	glutPostRedisplay();
}

//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize (1200, 720);
	glutInitWindowPosition (10, 10);
	glutCreateWindow ("Cannon");
	initialize();

	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
