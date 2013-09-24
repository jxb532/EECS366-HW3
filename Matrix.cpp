/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 09/25/2013 - Assignment 3                  */

#include "Matrix.h"

Matrix::Matrix() {
	rows = 0;
	cols = 0;
	matrix = 0;
}

Matrix::Matrix(int _rows, int _cols) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; ++i) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; ++j) {
			matrix[i][j] = 0;
		}
	}
}

Matrix::Matrix(int _rows, int _cols, float** _matrix) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; ++i) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; ++j) {
			matrix[i][j] = 0;
		}
	}
}

Matrix::~Matrix(void) {
	for (int i = 0; i < rows; ++i) {
		if (matrix[i]) delete [] matrix[i];
	}
	if (matrix) delete [] matrix;
}

float Matrix::get(int row, int col) {
	return matrix[row][col];
}

void Matrix::set(int row, int col, float value) {
	matrix[row][col] = value;
}

Matrix* Matrix::operator*(Matrix* m) {
	if (this->cols != m->rows) return 0;
	Matrix * temp = new Matrix(this->rows, m->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] = 0;
			for (int k = 0; k < m->rows; ++k) {
				temp->matrix[i][j] += this->matrix[i][k] * m->matrix[k][j];
			}
		}
	}
	return temp;
}

Matrix* Matrix::operator*(float s) {
	Matrix* temp = new Matrix(this->rows, this->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] *= s;
		}
	}
	return temp;
}

Matrix* Matrix::transpose() {
	Matrix* temp = new Matrix(this->rows, this->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] = this->matrix[j][i];
		}
	}
	return temp;
}

float** Matrix::toArray() {
	float** temp = new float*[this->rows];
	for (int i = 0; i < this->rows; ++i) {
		temp[i] = new float[this->cols];
		for (int j = 0; j < this->cols; ++j) {
			temp[i][j] = this->matrix[i][j];
		}
	}
	return temp;
}