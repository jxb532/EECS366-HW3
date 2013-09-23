/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#include "Vector.h"


Vector::Vector(void)
{
}


Vector::~Vector(void)
{
}

// TODO: This is still shit. Giant, festering loads of shit.
static float[] mult(float[] v1, float s)
{
	float[] v = new float[v1.Length];

	for (int i = 0; i < v.Length; i++)
	{
		v[i] = v1[i] * s;
	}

	return v;
}


static float[] crossProduct(float[] u, float[] v)
{
	return new float[] {
		u[1]*v[2]-u[2]*v[1],
		u[2]*v[0]-u[0]*v[2],
		u[0]*v[1]-u[1]*v[0]
	};
}

static float magnitude(float[] v)
{
	return (float)Math.Sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}