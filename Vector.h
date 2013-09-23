#pragma once
class Vector
{
private:
	float* vector;
public:
	int length;
	Vector(void);
	Vector crossProduct(Vector v);
	float magnitude();
	~Vector(void);
};

