#pragma once
class Vector
{
private:
	float* vector;
public:
	int length;
	Vector(void);
	Vector operator*(float s);
	float dotProduct(Vector v);
	Vector crossProduct(Vector v);
	float magnitude();
	~Vector(void);
};

