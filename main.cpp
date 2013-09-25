/* 
 * EECS 366 Assignment # 3
 *
 * Wes Rupert
 * wkr3
 *
 * Joshua Braun
 * jxb532
 *
 * 25 Sept 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "Matrix.h"
#include "Vector.h"
#include "main.h"

#define ON 1
#define OFF 0
#define ORIGIN 0
#define OBJECT 1
#define AXIS_LENGTH 3.0
#define RIGHT_BUTTON 2
#define MIDDLE_BUTTON 1
#define LEFT_BUTTON 0
#define PRESSED 0
#define RELEASED 1
#define TRANSLATE_STEP 5
#define ROTATE_STEP 5
#define SCALING_STEP 1

// XXX old
#define ROT_SCALE 0.3
#define ZOOM_SCALE 0.02

// Global variables
char * filename = "teapot.obj";

int window_width, window_height;    // Window dimensions
int PERSPECTIVE = OFF;
int AXES = OFF;
int OBJECTS = ON;
int LEFTDOWN = OFF;
int RIGHTDOWN = OFF;
int MIDDLEDOWN = OFF;
int NEEDS_TO_SNAP = ON;
int SNAP_TO = ORIGIN;

Matrix *localRot = new Matrix(3, 3);
Matrix *worldRot = new Matrix(3, 3);
Vector3 *translation = new Vector3();
int lastX = 0;
int lastY = 0;
float xTrans = 0;
float yTrans = 0;
float zTrans = 0;
float xRotWorld = 0;
float yRotWorld = 0;
float zRotWorld = 0;
float xRotLocal = 0;
float yRotLocal = 0;
float zRotLocal = 0;
float scaling = 1;
float xViewSwing = 0;
float yViewSwing = 0;
float xViewPan = 0;
float yViewPan = 0;
float zView = 5;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

// The mesh reader itself
// It can read *very* simple obj files
void meshReader (char *filename,int sign) {
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp)) {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v') {
	  ++verts;
	} else {
	  ++faces;
	}
   }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts;++i) {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;++i) {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;++i) {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;++i) {
      v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
      v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
      v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
      v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
      v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
      v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
      normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
      normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
      normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
      normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
      normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
      normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
      normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
      normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
      normCount[faceList[i].v1++];
      normCount[faceList[i].v2++];
      normCount[faceList[i].v3++];
    }
  for (i = 0;i < verts;++i) {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}



// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void) {
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO do we load identity and do all of our rotations/translations every time,
	// or do we just load the current model/view matrices and apply the most recent transform?
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (PERSPECTIVE) {
		// I think this is useless...
	}

	// we want to modify these matrices
	// remember that these are in COLUMN MAJOR!!!
	GLfloat modelviewMatrix[16];
	GLfloat projMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);

	float mod [16];
	for (int i = 0; i < 16; ++i) {
		mod[i] = (float)modelviewMatrix[i];
	}

	Matrix *model = new Matrix(4, 4, mod);
	Matrix *worldModel = modelMatrix(worldRot, translation);
	Matrix *localModel = modelMatrix(localRot, &Vector3(0, 0, 0));

	Matrix *rot1 = *worldModel * *model;
	Matrix *rot2 = *rot1 * *localModel;
	float *result = rot2->toArray();

	for (int i = 0; i < 16; ++i) {
		//modelviewMatrix[i] = result[i];
		modelviewMatrix[i] = 4.0;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelviewMatrix);

	//delete model; model = NULL;
	//delete worldModel; worldModel = NULL;
	//delete localModel; localModel = NULL;
	//delete rot1; rot1 = NULL;
	//delete rot2; rot2 = NULL;
	delete [] result;

	// TODO calculate projection (view) matrix
	if (NEEDS_TO_SNAP ==  ON) {
		NEEDS_TO_SNAP = OFF;
		if (SNAP_TO == ORIGIN) {
			xViewSwing = 0;
			yViewSwing = 0;
			xViewPan = 0;
			yViewPan = 0;
			zView = 5;
		} else {
			// TODO extract the object's origin from modelMatrix, point camera in that direction
			// getOriginFromModel(modelMatrix);
		}
	}

	float p[3] = {xViewPan, yViewPan, zView};
	float n[3] = {xViewSwing - xViewPan, yViewSwing - yViewPan, -zView};
	float v[3] = {0, 1, 0};

	//TODO
	//viewMatrix(p, n, v);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (OBJECTS == ON) {
		
		// Draw programatically
		glColor3f(1, 1, 0);
		for (int i = 0; i < faces; ++i) {
			point vertex;
			glBegin(GL_POLYGON);
				vertex = vertList[faceList[i].v1];
				glVertex3f(vertex.x, vertex.y, vertex.z);
				vertex = vertList[faceList[i].v2];
				glVertex3f(vertex.x, vertex.y, vertex.z);
				vertex = vertList[faceList[i].v3];
				glVertex3f(vertex.x, vertex.y, vertex.z);
			glEnd();
		}
	}

	if (AXES == ON) {
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(AXIS_LENGTH,0.0,0.0);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0.0,AXIS_LENGTH,0.0);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,AXIS_LENGTH);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
	}

    // (Note that the origin is lower left corner)
    // (Note also that the window spans (0,1) )
    // Finish drawing, update the frame buffer, and swap buffers
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y) {
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    if (PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
    }
    printf("Resized to %d %d\n",x,y);
}

void	setViewSwing(int x, int y) {
	xViewSwing += (x - lastX);
	yViewSwing += (y - lastY);
	glutPostRedisplay();
}

void	setViewPan(int x, int y) {
	xViewPan += (x - lastX);
	yViewPan += (y - lastY);
	glutPostRedisplay();
}

void	setViewDistance(int y) {
	zView += (y - lastY);
	glutPostRedisplay();
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y) {
	if (button == RIGHT_BUTTON) {
		if (state == PRESSED) {
			RIGHTDOWN = ON;
		} else {
			RIGHTDOWN = OFF;
		}
	} else if (button == MIDDLE_BUTTON) {
		if (state == PRESSED) {
			MIDDLEDOWN = ON;
		} else {
			MIDDLEDOWN = OFF;
		}
	} else if (button == LEFT_BUTTON) {
		if (state == PRESSED) {
			LEFTDOWN = ON;
		} else {
			LEFTDOWN = OFF;
		}
	}

	lastX = x;
	lastY = y;
    printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y) {
	if (LEFTDOWN == ON) {
		setViewSwing(x, y);
	} else if (MIDDLEDOWN == ON) {
		setViewPan(x, y);
	} else if (RIGHTDOWN == ON) {
		setViewDistance(y);
	}
	lastX = x;
	lastY = y;
	printf("Mouse is at %d, %d ; AngleX %f, AngleY %f, zDist %f\n", x,y);
}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y) {
	Matrix *rotate = NULL;
	Matrix *result = NULL;

    switch(key) {
    case '':                           /* Quit */
		exit(1);
		break;
    case 'p':
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
        if (PERSPECTIVE == ON) {
			// Orthogonal Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
			glutSetWindowTitle("Assignment 2 Template (orthogonal)");
			PERSPECTIVE = OFF;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		} else {
			// Perpective Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
			glutSetWindowTitle("Assignment 2 Template (perspective)");
			PERSPECTIVE = ON;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	case 'a':
    case 'A':
		if (AXES == ON) {
			AXES = OFF;
		} else {
			AXES = ON;
		}
		break;
	case 's':
    case 'S':
		if (OBJECTS == ON) {
			OBJECTS = OFF;
		} else {
			OBJECTS = ON;
		}
		break;
	case 'q':
    case 'Q':
		_exit(0);
		break;
	case '4':
		// negative x translation (world)
		translation->vector[0] = translation->vector[0] - TRANSLATE_STEP;
		break;
	case '6':
		// poitive x translation (world)
		translation->vector[0] = translation->vector[0] + TRANSLATE_STEP;
		break;
	case '2':
		// negative y translation (world)
		translation->vector[1] = translation->vector[1] - TRANSLATE_STEP;
		break;
	case '8':
		// positive y translation (world)
		translation->vector[1] = translation->vector[1] + TRANSLATE_STEP;
		break;
	case '1':
		// negative z translation (world)
		translation->vector[2] = translation->vector[2] - TRANSLATE_STEP;
		break;
	case '9':
		// positive z translation (world)
		translation->vector[2] = translation->vector[2] + TRANSLATE_STEP;
		break;
	case '[':
		// negative x rotation (world)
		rotate = rotateMatrix(-ROTATE_STEP, 'x');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case ']':
		// positive x rotation (world)
		rotate = rotateMatrix(ROTATE_STEP, 'x');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case ';':
		// negative y rotation (world)
		rotate = rotateMatrix(-ROTATE_STEP, 'y');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case '\'':
		// positive y rotation (world)
		rotate = rotateMatrix(ROTATE_STEP, 'y');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case '.':
		// negative z rotation (world)
		rotate = rotateMatrix(-ROTATE_STEP, 'z');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case '/':
		// positive z rotation (world)
		rotate = rotateMatrix(ROTATE_STEP, 'z');
		result = *rotate * *worldRot;
		delete worldRot; worldRot = NULL;
		delete rotate; rotate = NULL;
		worldRot = result;
		break;
	case '=':
		// increase uniform scaling (world)
		scaling += SCALING_STEP;
		break;
	case '-':
		// decrease uniform scaling (world)
		scaling -= SCALING_STEP;
		break;
	case 'i':
		// negative x rotation (local)
		rotate = rotateMatrix(-ROTATE_STEP, 'x');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case 'o':
		// poitive x rotation (local)
		rotate = rotateMatrix(ROTATE_STEP, 'x');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case 'k':
		// negative y rotation (local)
		rotate = rotateMatrix(-ROTATE_STEP, 'y');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case 'l':
		// positive y rotation (local)
		rotate = rotateMatrix(ROTATE_STEP, 'y');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case 'm':
		// negative z rotation (local)
		rotate = rotateMatrix(-ROTATE_STEP, 'z');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case ',':
		// positive z rotation (local)
		rotate = rotateMatrix(ROTATE_STEP, 'z');
		result = *localRot * *rotate;
		delete localRot; localRot = NULL;
		delete rotate; rotate = NULL;
		localRot = result;
		break;
	case 'c':
		NEEDS_TO_SNAP = ON;
		if (SNAP_TO == ORIGIN) {
			SNAP_TO = OBJECT;
		} else {
			SNAP_TO = ORIGIN;
		}
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


// Here's the main
int main(int argc, char* argv[]) {

	//// WHYYYY???????????
	//float nums [9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	//Matrix test (3, 3, nums);

	//// OH GOD, THE HUMANITY
	//float nums2 [9] = {1, 4, 7, 2, 5, 8, 3, 6, 9};
	//Matrix test2 (3, 3, nums2);

	//Matrix *result = test * test2;
	//delete result; result = NULL;
	
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // Constant values passed as logical OR
    glutCreateWindow("Assignment 2 Template (orthogonal)");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);	// Define State: Orthogonal projection to be used
    glLoadIdentity();	// initialize to Identity so that only specified projection transformation has an effect
    glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000); // Define left, right, bottom, top, far, near
    glMatrixMode(GL_MODELVIEW);		// Define State: Orthogonal projection to be used
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);	// Controls depth comparisons and updates to the depth buffer

	// So you can check the versions of software you use (not necessary for the assignment)
	printf( "OpenGL version: %s\n", (char*)glGetString(GL_VERSION));
    printf( "OpenGL renderer: %s\n", (char*)glGetString(GL_RENDERER));

	meshReader(filename, 1.0);

    // Switch to main loop
    glutMainLoop();				// Application does not come back here beyond this point !!
    return 0;        
}

Matrix* modelMatrix(Matrix* r, Vector3* p) {
	Matrix* temp = new Matrix(r->rows + 1, r->cols + 1);
	for (int i = 0; i < r->rows; ++i) {
		int j;
		for (j = 0; j < r->cols; ++j) {
			temp->set(i, j, r->get(i, j));
		}
		temp->set(i, j, p->vector[i]);
	}
	temp->set(temp->rows - 1, temp->cols - 1, 1);
	return temp;
}

Matrix* viewMatrix(Vector3* P, Vector3* N, Vector3* V) {
	Matrix *m = new Matrix(4,4);

	Vector3 n = *N * (-1.0 / N->magnitude());
	Vector3 u = (V->cross(&(*N * -1.0))) * ((V->cross(N)).magnitude());
	Vector3 v = n.cross(&v);

    for (int i = 0; i < 3; ++i)
    {
		m->set(0, i, u.vector[i]);
		m->set(1, i, v.vector[i]);
		m->set(2, i, n.vector[i]);
		m->set(3, i, 0);
    }

	m->set(0, 3, -1.0 * u.dot(P));
	m->set(1, 3, -1.0 * v.dot(P));
	m->set(2, 3, -1.0 * n.dot(P));
	m->set(3, 3, 1.0);

    return m;
}

Matrix* rotateMatrix(float thetaDeg, char axis) {
	float thetaRad = thetaDeg * (3.14159 / 180.0);
	float matrixX[9] = {1, 0, 0, 0, cos(thetaRad), sin(thetaRad), 0, -1.0 * sin(thetaRad), cos(thetaRad)};
	float matrixY[9] = {cos(thetaRad), 0, -1.0 * sin(thetaRad), 0, 1, 0, sin(thetaRad), 0, cos(thetaRad)};
	float matrixZ[9] = {cos(thetaRad), sin(thetaRad), 0, -1.0 * sin(thetaRad), cos(thetaRad), 0, 0, 1};

	switch (axis) {
	case 'x':
	case 'X':
		return new Matrix(3, 3, matrixX);
	case 'y':
	case 'Y':
		return new Matrix(3, 3, matrixY);
	case 'z':
	case 'Z':
		return new Matrix(3, 3, matrixZ);
	default:
		return new Matrix(3, 3);
	}
}

Matrix* translateMatrix(float x, float y, float z) {
	float matrix[3] = {x, y, z};
	return new Matrix(3, 1, matrix);
}
