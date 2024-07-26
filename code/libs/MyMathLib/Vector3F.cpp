#include <cmath>
#include "Vector2F.h"
#include "Vector3F.h"
#include "Vector3.h"
#include <cstring>

Vector3F::Vector3F(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3F::Vector3F(const Vector2F& vec, float zIn)
{
	x = vec.x;
	y = vec.y;
	z = zIn;
}

Vector3F::~Vector3F() {}


Vector3F& Vector3F::operator=(const Vector3& right)
{
	x = (float)right.x;
	y = (float)right.y;
	z = (float)right.z;
	return *this;
}

/*! \fn add vectors amd return new one*/
Vector3F Vector3F::operator+ (const Vector3F& right) const
{
	return Vector3F(x + right.x, y + right.y, z + right.z);
}

/*! \fn substract vectors and return new one*/
Vector3F Vector3F::operator- (const Vector3F& right) const
{
	return Vector3F(x - right.x, y - right.y, z - right.z);
}
/*! \fn dot product returns scalar*/
float Vector3F::dot(const Vector3F& right) const
{
	return x * right.x + y * right.y + z * right.z;
}

/*! \fn function returning length of instanced vector*/
float Vector3F::lengt() const
{
	return sqrtf(x*x + y*y + z*z);
}

float Vector3F::squareLength() const
{
	return x*x + y*y + z*z;
}

/*! \fn function returning new normalized vector*/
Vector3F Vector3F::normalize() const
{
	float squareLength = x * x + y * y + z * z;
	if (squareLength == 0.0) return Vector3F();
	float length = sqrt(squareLength);
	return Vector3F(x / length, y / length, z / length);
}
/*! \fn cross product function returning normal vector*/
Vector3F Vector3F::crossProd(const Vector3F& right) const
{
	float tx = (y * right.z) - (right.y * z);
	float ty = (x * right.z) - (right.x * z);
	float tz = (x * right.y) - (right.x * y);
	return Vector3F(tx, ty, tz);
}

/*! \fn function to convert 3D vector to 4D vector*/
Vector4F Vector3F::vec3TOvec4(const Vector3F& vector, float w)
{
	return Vector4F(vector.x, vector.y, vector.z, w);
}

Vector3 Vector3F::toDouble() const
{
	return Vector3(x, y, z);
}

/*! \fn vector*num returns new matrix*/
Vector3F Vector3F::operator* (const float& right) const
{
	return Vector3F(x * right, y * right, z * right);
}

Vector3F Vector3F::operator*(const Vector3F& right) const
{
	return Vector3F(x*right.x, y*right.y, z*right.z);
}

/*! \fn num*vector returns new matrix*/
Vector3F operator* (const float& left, const Vector3F& right)
{
	return Vector3F(right.x * left, right.y * left, right.z * left);
}

/*! \fn vector/num returns new matrix*/
Vector3F Vector3F::operator/ (const float& right) const
{
	return Vector3F(x / right, y / right, z / right);
}

Vector3F Vector3F::operator/ (const Vector3F& right) const
{
	return Vector3F(x / right.x, y / right.y, z / right.z);
}

/*! \fn adds vector to this vector*/
void Vector3F::operator+=(const Vector3F& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

/*! \fn subtracts vector from this vector*/
void Vector3F::operator-=(const Vector3F& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

/*! \fn multiplies this vector with vector and sets result*/
void Vector3F::operator*=(const float& number)
{
	x *= number;
	y *= number;
	z *= number;
}

/*! \fn divides this vector with number and assings result*/
void Vector3F::operator/=(const float& number)
{
	x /= number;
	y /= number;
	z /= number;
}

/*! \fn compares vectors for equality*/
bool Vector3F::operator==(const Vector3F& v)const
{
	return !std::memcmp((void*)this, (void*)&v, sizeof(Vector3F));
	//if (x != v.x || y != v.y || z != v.z) return false;	
	//else return true;
}

/*! \fn operator[] overload for indexing */
float& Vector3F::operator[] (int index)
{
	return vect[index];
}