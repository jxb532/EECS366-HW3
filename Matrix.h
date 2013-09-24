/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#pragma once
class Matrix
{
private:
	float** matrix;
public:
	int     rows;
	int     cols;
	        Matrix(void);
	        Matrix(int _rows, int _cols);
			Matrix(int _rows, int _cols, float** _matrix);
	        ~Matrix(void);
	float   get(int i, int j);
	void    set(int i, int j, float value);
	Matrix* operator*(Matrix* m);
	Matrix* operator*(float s);
	Matrix* transpose();
	float* toArray();
};