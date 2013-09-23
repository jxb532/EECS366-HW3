#include "Matrix.h"


Matrix::Matrix(int _rows, int _cols) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; i++) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = 0;
		}
	}
}


Matrix::~Matrix(void) {
	for (int i = 0; i < rows; i++) {
		delete [] matrix[i];
	}
	delete [] matrix;
}

Matrix Matrix::operator*(Matrix m)
{
	Matrix * temp = new Matrix(rows, m.cols);

	for (int i = 0; i < m.GetLength(0); i++)
	{
		for (int j = 0; j < m.GetLength(1); j++)
		{
			m[i, j] = 0;
			for (int k = 0; k < m2.GetLength(0); k++)
			{
				m[i, j] += m1[i, k] * m2[k, j];
			}
		}
	}

	return m;
}

static float[,] mult(float[,] m1, float s)
{
	float[,] m = new float[m1.GetLength(0), m1.GetLength(1)];
	for (int i = 0; i < m.GetLength(0); i++)
	{
		for (int j = 0; j < m.GetLength(1); j++)
		{
			m[i, j] = s * m1[i, j];
		}
	}

	return m;
}

static float[] mult(float[] v1, float s)
{
	float[] v = new float[v1.Length];

	for (int i = 0; i < v.Length; i++)
	{
		v[i] = v1[i] * s;
	}

	return v;
}

static void print(float[,] m, string name)
{
	if (m.GetLength(1) > 1)
	{
		Console.WriteLine("Matrix {0}:", name);
		for (int i = 0; i < m.GetLength(0); i++)
		{
			for (int j = 0; j < m.GetLength(1); j++)
			{
				Console.Write(m[i, j] + "\t");
			}
			Console.WriteLine();
		}
		Console.WriteLine();
	}
	else
	{
		Console.WriteLine("Point {0}:", name);
		Console.WriteLine("({0}, {1}, {2})", m[0, 0], m[1, 0], m[2, 0]);
		Console.WriteLine();
	}
}

static float[,] modelMatrix(float[,] r, float[,] p)
{
	float[,] m = new float[r.GetLength(0) + 1, r.GetLength(1) + 1];
	for (int i = 0; i < r.GetLength(0); i++)
	{
		for (int j = 0; j < r.GetLength(1); j++)
		{
			m[i, j] = r[i, j];
		}
		m[i, r.GetLength(1)] = p[i, 0];
	}

	for (int j = 0; j < m.GetLength(1); j++)
	{
		m[m.GetLength(0) - 1, j] = 0;
	}
	m[m.GetLength(0) - 1, m.GetLength(1) - 1] = 1;

	return m;
}

static float[,] viewMatrix(float[,] P, float[] N, float[] V)
{
	float[,] m = new float[4, 4];

	float[] n = mult(mult(N, -1f), 1 / magnitude(N));
	float[] u = mult(crossProduct(V, mult(N, -1f)), 1 / magnitude(crossProduct(V, N)));
	float[] v = crossProduct(n, u);

	for (int i = 0; i < 3; i++)
	{
		m[0, i] = u[i];
		m[1, i] = v[i];
		m[2, i] = n[i];
		m[3, i] = 0;
	}

	m[0, 3] = mult(mult(new float[,] { { u[0], u[1], u[2] } }, -1), P)[0, 0];
	m[1, 3] = mult(mult(new float[,] { { v[0], v[1], v[2] } }, -1), P)[0, 0];
	m[2, 3] = mult(mult(new float[,] { { n[0], n[1], n[2] } }, -1), P)[0, 0];
	m[3, 3] = 1;

	return m;
}

enum Axis { X, Y, Z }
static float[,] rotateMatrix(float theta, Axis axis, bool appendLastLine = false)
{
	theta = theta * (float)Math.PI / 180f;

	switch (axis)
	{
		case Axis.X:
			if (appendLastLine)
				return new float[,] {
					{1, 0, 0, 0},
					{0, (float)Math.Cos(theta), -1f * (float)Math.Sin(theta), 0},
					{0, (float)Math.Sin(theta), (float)Math.Cos(theta), 0},
					{0, 0, 0, 1}
				};
			else
				return new float[,] {
					{1, 0, 0},
					{0, (float)Math.Cos(theta), -1f * (float)Math.Sin(theta)},
					{0, (float)Math.Sin(theta), (float)Math.Cos(theta)},
				};
		case Axis.Y:
			if (appendLastLine)
				return new float[,] {
					{(float)Math.Cos(theta), 0, (float)Math.Sin(theta), 0},
					{0, 1, 0, 0},
					{-1f * (float)Math.Sin(theta), 0, (float)Math.Cos(theta), 0},
					{0, 0, 0, 1}
				};
			else
				return new float[,] {
					{(float)Math.Cos(theta), 0, (float)Math.Sin(theta)},
					{0, 1, 0},
					{-1f * (float)Math.Sin(theta), 0, (float)Math.Cos(theta)},
				};
		case Axis.Z:
			if (appendLastLine)
				return new float[,] {
					{(float)Math.Cos(theta), -1f * (float)Math.Sin(theta), 0, 0},
					{ (float)Math.Sin(theta), (float)Math.Cos(theta), 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}
				};
			else
				return new float[,] {
					{(float)Math.Cos(theta), -1f * (float)Math.Sin(theta), 0},
					{ (float)Math.Sin(theta), (float)Math.Cos(theta), 0},
					{0, 0, 1},
				};
	}

	return null;
}

static float[,] translateMatrix(float x, float y, float z)
{
	return new float[,] { { x }, { y }, { z } };
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

static float[,] transpose(float[,] m1)
{
	float[,] m = new float[m1.GetLength(1), m1.GetLength(0)];

	for (int i = 0; i < m.GetLength(0); i++)
	{
		for (int j = 0; j < m.GetLength(1); j++)
		{
			m[i, j] = m1[j, i];
		}
	}

	return m;
}
}
}
