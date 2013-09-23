/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#include "Matrix.h"
#include "Vector.h"

// Vertex and Face data structure used in the mesh reader.
typedef struct _point {
  float x,y,z;
} point;
typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

// Main action methods.
void   meshReader (char *filename,int sign);
void   display(void);
void   resize(int x,int y);
void   setRotation(int x, int y);
void   setViewDistance(int y);
void   mouseButton(int button,int state,int x,int y);
void   mouseMotion(int x, int y);
void   keyboard(unsigned char key, int x, int y);
int    main(int argc, char* argv[]);

// Graphical matrix generators.
Matrix modelMatrix(Matrix r, Matrix p);
Matrix viewMatrix(Matrix P, Vector N, Vector V);
Matrix rotateMatrix(float theta, char axis);
Matrix translateMatrix(float x, float y, float z);