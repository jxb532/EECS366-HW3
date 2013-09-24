/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#define LENGTH 3

#pragma once
class Vector3
{
private:
	
public:
	         Vector3(void);
		     Vector3(float x, float y, float z);
	         ~Vector3(void);
	Vector3* operator*(float s);
	Vector3* cross(Vector3* v);
	float    dot(Vector3* v);
	float    magnitude();
	float vector [LENGTH];
};