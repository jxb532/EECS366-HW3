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

#define ON 1
#define OFF 0
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
float zView = 0;

// Vertex and Face data structure used in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

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
	  verts++;
	} else {
	  faces++;
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
  for(i = 0;i < verts;i++) {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;i++) {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++) {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++) {
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
      normCount[faceList[i].v1]++;
      normCount[faceList[i].v2]++;
      normCount[faceList[i].v3]++;
    }
  for (i = 0;i < verts;i++) {
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

	glLoadIdentity();
	

	if (PERSPECTIVE) {
		// Set the camera position, orientation and target
		gluLookAt(0,0,zDist, 0,0,0, 0,1,0);
	}

	glRotatef(angleY, 1.0, 0.0, 0.0);
	glRotatef(angleX, 0.0, 1.0, 0.0);
	   
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (OBJECTS == ON) {
		
		// Draw programatically
		glColor3f(1, 1, 0);
		for (int i = 0; i < faces; i++) {
			point vertex;
			glBegin(GL_POLYGON);
				vertex = vertList[faceList[i].v1];
				glVertex3f(vertex.x / zDist, vertex.y / zDist, vertex.z / zDist);
				vertex = vertList[faceList[i].v2];
				glVertex3f(vertex.x / zDist, vertex.y / zDist, vertex.z / zDist);
				vertex = vertList[faceList[i].v3];
				glVertex3f(vertex.x / zDist, vertex.y / zDist, vertex.z / zDist);
			glEnd();
		}
	}

	if (AXES == ON) {
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(AXIS_LENGTH / zDist,0.0,0.0);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0.0,AXIS_LENGTH / zDist,0.0);
			glVertex3f(0.0,0.0,0.0);
		glEnd();
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,AXIS_LENGTH / zDist);
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
	// TODO
	xViewSwing += (x - lastX);
	yViewSwing += (y - lastY);
	glutPostRedisplay();
}

void	setViewPan(int x, int y) {
	// TODO
	glutPostRedisplay();
}

void	setViewDistance(int y) {
	// TODO
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
		xTrans -= TRANSLATE_STEP;
		break;
	case '6':
		// poitive x translation (world)
		xTrans += TRANSLATE_STEP;
		break;
	case '2':
		// negative y translation (world)
		yTrans -= TRANSLATE_STEP;
		break;
	case '8':
		// positive y translation (world)
		yTrans += TRANSLATE_STEP;
		break;
	case '1':
		// negative z translation (world)
		zTrans -= TRANSLATE_STEP;
		break;
	case '9':
		// positive z translation (world)
		zTrans += TRANSLATE_STEP;
		break;
	case '[':
		// negative x rotation (world)
		xRotWorld -= ROTATE_STEP;
		break;
	case ']':
		// positive x rotation (world)
		xRotWorld += ROTATE_STEP;
		break;
	case ';':
		// negative y rotation (world)
		yRotWorld -= ROTATE_STEP;
		break;
	case '\'':
		// positive y rotation (world)
		yRotWorld += ROTATE_STEP;
		break;
	case '.':
		// negative z rotation (world)
		zRotWorld -= ROTATE_STEP;
		break;
	case '/':
		// positive z rotation (world)
		zRotWorld += ROTATE_STEP;
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
		xRotLocal -= ROTATE_STEP;
		break;
	case 'o':
		// poitive x rotation (local)
		xRotLocal += ROTATE_STEP;
		break;
	case 'k':
		// negative y rotation (local)
		yRotLocal -= ROTATE_STEP;
		break;
	case 'l':
		// positive y rotation (local)
		yRotLocal += ROTATE_STEP;
		break;
	case 'm':
		// negative z rotation (local)
		zRotLocal -= ROTATE_STEP;
		break;
	case ',':
		// positive z rotation (local)
		zRotLocal += ROTATE_STEP;
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


// Here's the main
int main(int argc, char* argv[]) {
	
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
