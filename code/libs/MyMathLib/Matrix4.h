#pragma once
#include "Vector4.h"

class Vector3;
class Matrix4F;
class Matrix3;
class Quaternion;

struct loc;

class Matrix4

{
public:
	union
	{
		double _matrix[4][4];
		double _matrix16[16];
		struct
		{
			Vector4 right, up, dir, position;
		} v;
		Vector4 component[4];
	};
	

	Matrix4(); //!< in constructor matrix values are set to 0 with memset
	Matrix4(int identity); //!< in constructor matrix values are set to 0 with memset
	~Matrix4();

	Matrix4 operator~ () const; //!< transpose matrix returns new matrix
	Matrix4& operator= (const Matrix4F& rightMatrix); //!< copy matrix returns this matrix
	bool operator== (const Matrix4& rightMatrix) const; //!< check if matrices are identical
	Matrix4 operator* (const Matrix4& rightMatrix) const; //!< matrix*matrix returns new matrix
	double operator() (int row, int col) const; //!< operator() overload for indexing
	Matrix4 operator* (const double& rightDouble) const; //!< matrix*num returns new matrix
	Vector4 operator* (const Vector4& rightVector) const; //!< matrix*vector returns new vector
	Vector3 operator* (const Vector3& rightVector) const; //!< matrix*vector returns new vector
	double operator[] (loc const& cLoc) const; //!< operator[] overload for indexing
	double* operator[] (int index); //!< operator[] overload for indexing

	friend Matrix4 operator* (const double& leftDouble, const Matrix4& rightMatrix); //!< num*matrix returns new matrix

	Matrix4 inverse() const; //!< calculates inverse of matrix4x4 and returns as new one
	Matrix4F toFloat() const; //!< just converts double matrix to float matrix
	Matrix3 convertToMatrix3() const;
	Vector3 getScale() const;
	Vector3 getPosition() const;
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
	Matrix3 extractRotation3() const;
	Matrix4 extractRotation() const;
	Quaternion toQuaternion() const;
	double AngleX() const;
	double AngleY() const;
	double AngleZ() const;
	Vector3 Angles() const;

	void setOrientation(const Quaternion& orientation);

	void setUp(const Vector3& axis);
	void setRight(const Vector3& axis);
	void setForward(const Vector3& axis);
	void setScale(const Vector3& scale);
	void setPosition(const Vector3& position);
	void translate(const Vector3& position);
	void operator*=(const Matrix4& rightMatrix);
	void setIdentity();
	void clear();
	void zeroPosition();
	void resetScale();
	void resetRotation();
	
	static Matrix4 CalculateRelativeTransform(const Matrix4& parent, const Matrix4& child);
	static Matrix4 perspective(const double &near, const double &far, const double &fov); //!< function returning perspective projection specified with given parameters
	static Matrix4 orthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom); //!< function returninng orthographic projection specified with given parameters
	static Matrix4 orthographicTopToBottom(const double& near, const double& far, const double& left, const double& right, const double& top, const double& bottom);
	static Matrix4 translation(const double &x, const double &y, const double &z); //!< returns translation matrix with specified translation values
	static Matrix4 translation(const Vector3& rightVector); //!< returns translation matrix with specified translation values
	static Matrix4 scale(const double &x, const double &y, const double &z); //!< function returning new scale matrix with specified scale values
	static Matrix4 scale(const Vector3& rightVector); //!< function returning new scale matrix with specified scale values
	static Matrix4 rotateX(const double &angle); //!< function returning rotation matrix with specified rotation angle along X axis
	static Matrix4 rotateY(const double &angle); //!< function returning rotation matrix with specified rotation angle along Y axis
	static Matrix4 rotateZ(const double &angle); //!< function returning rotation matrix with specified rotation angle along Z axis
	static Matrix4 rotateAngle(const Vector3& thisVector, const double &angle); //!< function returning rotation matrix with specified rotation angle along specified axis(vector)
	
	static Matrix4 nolookAt(Vector3 eye, Vector3 target, Vector3 up); //!< lookAt matrix not optimized
	static Matrix4 lookAt(Vector3 eye, Vector3 target, Vector3 up); //!< lookAt matrix optimized
	static Matrix4 FPScam(Vector3 eye, const double &pitch, const double &yaw); //!< fps camera matrix
	static Matrix4 Frustum(const double &l, const double &r, const double &b, const double &t, const double &n, const double &f); //!< frustum matrix used to produce perspective matrix with aspect ratio
	static Matrix4 OpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far); //!< perspective matrix with aspect ratio

	static Matrix4 sFrustum(const double &left, const double &right, const double &bottom, const double &top, const double &near, const double &far); //!< transposed frustum matrix used to produce transposed perspective matrix with aspect ratio
	static Matrix4 sOpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far); //!< transposed perspective matrix with aspect ratio
	static Matrix4 sPerspective(const double &near, const double &far, const double &fov); //!< function returning transposed perspective 
	static Matrix4 sTranslate(const double &x, const double &y, const double &z); //!< returns transposed translation matrix with specified translation values
	static Matrix4 sOrthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom); //!< function returninng orthographic projection specified with given parameters
	static Matrix4 sOrthographicTopToBottom(const double& near, const double& far, const double& left, const double& right, const double& top, const double& bottom);
	//static Matrix4 identityMatrix(); //!< identity matrix 
	static Matrix4 biasMatrix(); //!< bias shadowmap matrix 
};