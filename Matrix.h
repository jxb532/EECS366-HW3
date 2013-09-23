#pragma once
class Matrix
{
private:
	float** matrix;

public:
	int rows, cols;
	Matrix(void);
	Matrix(int _rows, int _cols);
	Matrix(int _rows, int _cols, float** m);
	void set(float** m);
	Matrix operator*(Matrix m);
	Matrix operator*(float s);
	Matrix transpose();
	~Matrix(void);
};

