#pragma once
#include "Vector3.h"

class Vector3;
class Matrix3F;
class Matrix4F;
class Matrix4;
struct loc;
class Quaternion;

/*! class for constructing matrix*/
class Matrix3

{
public:
	union
	{
		double _matrix[3][3];
		double _matrix9[9];
		struct
		{
			Vector3 right, up, dir;
		} v;
		Vector3 component[3];
	};

	Matrix3(); //!< in constructor matrix values are set to 0 with memset
	Matrix3(int identity); //!< in constructor matrix values are set to 0 with memset
	~Matrix3();

	Matrix3 operator~ (); //!< transpose matrix returns new matrix
	Matrix3 operator+ (const Matrix3& rightMatrix);
	Matrix3& operator= (const Matrix3F& rightMatrix); //!< assign matrix returns this matrix
	Matrix3& operator= (const Matrix4& rightMatrix); //!< assign matrix returns this matrix
	Matrix3& operator= (const Matrix4F& rightMatrix); //!< assign matrix returns this matrix
	bool operator== (const Matrix3& rightMatrix); //!< check if matrices are identical
	Matrix3 operator* (const Matrix3& rightMatrix); //!< matrix*matrix returns new matrix
	double operator() (int row, int col);//!< operator() overload for indexing
	Matrix3 operator* (const double& rightDouble); //!< matrix*num returns new matrix
	Vector3 operator* (const Vector3& rightVector) const; //!< matrix*vector returns new vector
	double operator[] (loc const& cLoc); //!< operator[] overload for indexing
	double* operator[] (int index); //!< operator[] overload for indexing

	friend Matrix3 operator* (const double& leftDouble, const Matrix3& rightMatrix); //!< num*matrix returns new matrix

	Matrix3 inverse() const; //!< calculates inverse of matrix4x4 and returns as new one
	Matrix3F toFloat(); //!< just converts double matrix to float matrix
	Vector3 getUp() const;
	Vector3 getInvUp() const;
	Vector3 getLeft() const;
	Vector3 getInvLeft() const;
	Vector3 getBack() const;
	Vector3 getInvBack() const;
	Vector3 getForward() const;
	Vector3 getInvForward() const;
	Vector3 getAxis(int axis) const;
	Vector3 getAxisNormalized(int axis) const;
	Vector3 extractScale() const;
	Vector3 getScale() const;
	Quaternion toQuaternion() const;
	double AngleX() const;
	double AngleY() const;
	double AngleZ() const;

	void setUp(const Vector3& axis);
	void setRight(const Vector3& axis);
	void setForward(const Vector3& axis);
	void setAxes(const Vector3& right, const Vector3& up, const Vector3& forward);
	void setScale(const Vector3& scale);
	void setScale(double x, double y, double z);
	void setPosition(const Vector3& position);
	void translate(const Vector3& position);
	void operator*=(const Matrix3& rightMatrix);
	void operator*=(const double& rightDouble);
	void setIdentity();
	void inverseThis();
	void clear();

	void setSkewSymmetric(const Vector3& vector);
	static Matrix3 CuboidInertiaTensor(const Vector3& dimensions);
	static double det(double a, double b, double c, double d, double e, double f, double g, double h, double i); //!< calculates determinant of 3x3 matrix
	static Matrix3 translation(double x, double y, double z); //!< returns translation matrix with specified translation values
	static Matrix3 translation(const Vector3 & right);
	static Matrix3 scale(double x, double y, double z); //!< function returning new scale matrix with specified scale values
	static Matrix3 scale(const Vector3& rightVect); //!< function returning new scale matrix with specified scale values
	static Matrix3 rotateX(double angle); //!< function returning rotation matrix with specified rotation angle along X axis
	static Matrix3 rotateY(double angle); //!< function returning rotation matrix with specified rotation angle along Y axis
	static Matrix3 rotateZ(double angle); //!< function returning rotation matrix with specified rotation angle along Z axis
	static Matrix3 rotateAngle(Vector3& thisVector, double angle); //!< function returning rotation matrix with specified rotation angle along specified axis(vector)
	//static Matrix3 identityMatrix();

};