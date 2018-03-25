#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
using namespace std;

// CONSTANTS //
#define TO_RAD (3.14159265/180.0);  //Conversion from degrees to radians

//--Globals ---------------------------------------------------------------
float *x, *y, *z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles

float cam_x, cam_y, cam_z;
float cam_angle = 0;

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

//----------draw a floor plane-------------------
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
	float lpos[4] = {400.0, 100.0, -200.0, 1.0};  //light's position

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam_x, cam_y, cam_z, cam_x + cos(cam_angle), cam_y, cam_z + sin(cam_angle), 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);   //set light position

	//glRotatef(angle, 0.0, 1.0, 0.0);		//rotate the whole scene

	glPushMatrix();
		glTranslatef(250, 0, 0);
		glScalef(100, 200, 100);
		drawPyramid();
	glPopMatrix();

	drawFloor();

	//--start here

	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize() {
	loadMeshFile("./models/Pyramid.off");				//Specify mesh file name here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 50, 1000);  //The camera view volume

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
