#include "Matrix.h"
#include "Vector.h"

enum Axis { X, Y, Z };
static Matrix modelMatrix(Matrix r, Matrix p);
static Matrix viewMatrix(Matrix P, Vector N, Vector V);
static Matrix rotateMatrix(float theta, Axis axis, bool appendLastLine = false);
static Matrix translateMatrix(float x, float y, float z);
