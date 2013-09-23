/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#pragma once
class Vector
{
private:
	float* vector;
public:
	int    length;
	       Vector(void);
	       ~Vector(void);
	Vector crossProduct(Vector v);
	float  magnitude();
};