#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Vector3F.h"
#include <cstring>

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3::Vector3(const Vector2& vec, double zIn)
{
	x = vec.x;
	y = vec.y;
	z = zIn;
}

Vector3::Vector3(const Vector4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3::~Vector3() {}

Vector3& Vector3::operator=(const Vector3F& right)
{
	x = right.x;
	y = right.y;
	z = right.z;
	return *this;
}

Vector3& Vector3::operator=(const Vector4& right)
{
	x = right.x;
	y = right.y;
	z = right.z;
	return *this;
}

/*! \fn add vectors amd return new one*/
Vector3 Vector3::operator+ (const Vector3& right) const
{
	return Vector3(x + right.x, y + right.y, z + right.z);
}

/*! \fn substract vectors and return new one*/
Vector3 Vector3::operator- (const Vector3& right) const
{
	return Vector3(x - right.x, y - right.y, z - right.z);
}
/*! \fn dot product returns scalar*/
double Vector3::dot(const Vector3& right) const
{
	return x * right.x + y * right.y + z * right.z;
}

/*! \fn function returning length of instanced vector*/
double Vector3::lengt() const
{
	return sqrt(x*x + y*y + z*z);
}

double Vector3::squareLength() const
{
	return x*x + y*y + z*z;
}

/*! \fn function returning new normalized vector*/
Vector3 Vector3::normalize() const
{
	double squareLength = x * x + y * y + z * z;
	if (squareLength == 0.0) return Vector3();
	double length = sqrt(squareLength);
	return Vector3(x / length, y / length, z / length);
}
/*! \fn cross product function returning normal vector*/
Vector3 Vector3::crossProd(const Vector3& right) const
{
	double tx = (y * right.z) - (right.y * z);
	double ty = (x * right.z) - (right.x * z);
	double tz = (x * right.y) - (right.x * y);
	return Vector3(tx, ty, tz);
}

/*! \fn convert this 3D vector to 4D vector*/
Vector4 Vector3::vec3TOvec4(const Vector3& vector, double w)
{
	return Vector4(vector.x, vector.y, vector.z, w);
}

/*! \fn this vector*num returns new vector*/
Vector3 Vector3::operator* (const double& right) const
{
	return Vector3(x * right, y * right, z * right);
}

/*! \fn this vector*vector returns new vector*/
Vector3 Vector3::operator*(const Vector3& right) const
{
	return Vector3(x * right.x, y * right.y, z * right.z);
}

/*! \fn num*vector returns new vector*/
Vector3 operator* (const double& left, const Vector3& right)
{
	return Vector3(right.x * left, right.y * left, right.z * left);
}

/*! \fn this vector/num returns new vector*/
Vector3 Vector3::operator/ (const double& right) const
{
	return Vector3(x / right, y / right, z / right);
}

/*! \fn this vector/vector returns new vector*/
Vector3 Vector3::operator/ (const Vector3& right) const
{
	return Vector3(x / right.x, y / right.y, z / right.z);
}

void Vector3::zero()
{
	x = 0, y = 0, z = 0;
}

void Vector3::one()
{
	x = 1.0, y = 1.0, z = 1.0;
}

/*! \fn adds vector to this vector*/
void Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

/*! \fn subtracts vector from this vector*/
void Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

/*! \fn multiplies this vector with vector and sets result*/
void Vector3::operator*=(const double& number)
{
	x *= number;
	y *= number;
	z *= number;
}

/*! \fn divides this vector with number and assings result*/
void Vector3::operator/=(const double& number)
{
	x /= number;
	y /= number;
	z /= number;
}

/*! \fn compares vectors for equality*/
bool Vector3::operator==(const Vector3& v)const
{
	return !std::memcmp((void*)this, (void*)&v, sizeof(Vector3));
	//if (x != v.x || y != v.y || z != v.z) return false;	
	//else return true;
}

/*! \fn operator[] overload for indexing */
double& Vector3::operator[] (int index)
{
	return vect[index];
}

Vector3F Vector3::toFloat() const
{
	return Vector3F((float)x, (float)y, (float)z);
}