#include <cmath>
#include <memory.h>
#include "Matrix3.h"
#include "Matrix4.h"
#include "Matrix4F.h"
#include "Vector3.h"
#include "Vector4.h"
#include "mLoc.h"
#include "Quaternion.h"
#include <cstring>
#include <algorithm>

/*! \fn in constructor matrix values are set to 0 with memset*/
Matrix4::Matrix4()
{
	memset(_matrix, 0, sizeof _matrix);
}

Matrix4::Matrix4(int identity)
{
	memset(_matrix, 0, sizeof _matrix);
	_matrix[0][0] = 1.0;
	_matrix[1][1] = 1.0;
	_matrix[2][2] = 1.0;
	_matrix[3][3] = 1.0;
}

Matrix4::~Matrix4()
{

}

/*! \fn converts double matrix to float */
Matrix4F Matrix4::toFloat() const
{
	Matrix4F temp;
	temp._matrix[0][0] = (float)_matrix[0][0]; 	temp._matrix[0][1] = (float)_matrix[0][1]; 	temp._matrix[0][2] = (float)_matrix[0][2];	temp._matrix[0][3] = (float)_matrix[0][3];
	temp._matrix[1][0] = (float)_matrix[1][0]; 	temp._matrix[1][1] = (float)_matrix[1][1]; 	temp._matrix[1][2] = (float)_matrix[1][2];	temp._matrix[1][3] = (float)_matrix[1][3];
	temp._matrix[2][0] = (float)_matrix[2][0]; 	temp._matrix[2][1] = (float)_matrix[2][1]; 	temp._matrix[2][2] = (float)_matrix[2][2];	temp._matrix[2][3] = (float)_matrix[2][3];
	temp._matrix[3][0] = (float)_matrix[3][0];	temp._matrix[3][1] = (float)_matrix[3][1];	temp._matrix[3][2] = (float)_matrix[3][2];	temp._matrix[3][3] = (float)_matrix[3][3];
	return temp;
}

/*! \fn operator[] overload for indexing */
double* Matrix4::operator[] (int index)
{
	return _matrix[index];
}

/*! \fn operator[] overload for indexing */
double Matrix4::operator[](loc const& mLoc) const
{
	return _matrix[mLoc.x][mLoc.y];
}

/*! \fn operator() overload for indexing*/
double Matrix4::operator() (int row, int col) const
{
	return _matrix[row][col];
}

/*! \fn transpose matrix returns new matrix*/
Matrix4 Matrix4::operator~ () const
{
	Matrix4 realTemp;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			//cout << first._matrix[r][c] << " result �r:" <<  result._matrix[r][c] << endl;
			realTemp._matrix[r][c] = _matrix[c][r];
		}
	}
	return realTemp;
}

/*! \fn num*matrix returns new matrix*/
Matrix4 operator*(const double& left, const Matrix4& right)
{
	Matrix4 temp;
	for (int i = 0; i < 4; i++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp._matrix[i][c] = left * right._matrix[i][c];
		}
	}
	return temp;
}
//when multiplying rotation matrices, multiply in order: z * y * x, quaternions are multiplied in order: x*y*z, might have to fix that
/*! \fn matrix*num returns new matrix*/
Matrix4 Matrix4::operator* (const double& right) const
{
	Matrix4 temp;
	for (int i = 0; i < 4; i++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp._matrix[i][c] = _matrix[i][c] * right;

		}
	}
	return temp;

}

/*! \fn matrix*matrix returns new matrix*/
Matrix4 Matrix4::operator* (const Matrix4& right) const// matrix multi
{
	Matrix4 temp;

	//multiply
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			for (int k = 0; k < 4; k++)
			{
				temp._matrix[r][c] = temp._matrix[r][c] + _matrix[r][k] * right._matrix[k][c];

			}
		}
	}
	return temp;
}

/*! \fn matrix*vector returns new vector*/
Vector4 Matrix4::operator* (const Vector4& right) const// matrix multi
{
	double vx = right.x;
	double vy = right.y;
	double vz = right.z;
	double vw = right.w;

	double _x = _matrix[0][0] * vx + _matrix[1][0] * vy + _matrix[2][0] * vz + _matrix[3][0] * vw;
	double _y = _matrix[0][1] * vx + _matrix[1][1] * vy + _matrix[2][1] * vz + _matrix[3][1] * vw;
	double _z = _matrix[0][2] * vx + _matrix[1][2] * vy + _matrix[2][2] * vz + _matrix[3][2] * vw;
	double _w = _matrix[0][3] * vx + _matrix[1][3] * vy + _matrix[2][3] * vz + _matrix[3][3] * vw;
	return Vector4(_x, _y, _z, _w);
}

/*! \fn matrix*vector returns new vector*/
Vector3 Matrix4::operator* (const Vector3& right) const// matrix multi
{
	//row major
	double vx = right.x;
	double vy = right.y;
	double vz = right.z;

	double _x = _matrix[0][0] * vx + _matrix[1][0] * vy + _matrix[2][0] * vz;
	double _y = _matrix[0][1] * vx + _matrix[1][1] * vy + _matrix[2][1] * vz;
	double _z = _matrix[0][2] * vx + _matrix[1][2] * vy + _matrix[2][2] * vz;
	return Vector3(_x, _y, _z);
}

Matrix4& Matrix4::operator=(const Matrix4F & right)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			_matrix[r][c] = right._matrix[r][c];

		}
	}
	return *this;
}

/*! \fn check if matrices are identical*/
bool Matrix4::operator== (const Matrix4& right) const
{
	return !std::memcmp((void*)this, (void*)&right, sizeof(Matrix4));
	/*
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			if (_matrix[r][c] != right._matrix[r][c])
			{
				return false;
			}
		}
	}
	return true;
	*/
}

/*! \fn function returning rotation matrix with specified rotation angle along X axis*/
Matrix4 Matrix4::rotateX(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixX;
	//row1
	rotationMatrixX._matrix[0][0] = 1;
	//rotationMatrixX._matrix[0][1] = 0;
	//rotationMatrixX._matrix[0][2] = 0;
	//rotationMatrixX._matrix[0][3] = 0;
	//row2
	//rotationMatrixX._matrix[1][0] = 0;
	rotationMatrixX._matrix[1][1] = cosAng;
	rotationMatrixX._matrix[1][2] = -sinAng;
	//rotationMatrixX._matrix[1][3] = 0;
	//row3
	//rotationMatrixX._matrix[2][0] = 0;
	rotationMatrixX._matrix[2][1] = sinAng;
	rotationMatrixX._matrix[2][2] = cosAng;
	//rotationMatrixX._matrix[2][3] = 0;
	//row4
	//rotationMatrixX._matrix[3][0] = 0;
	//rotationMatrixX._matrix[3][1] = 0;
	//rotationMatrixX._matrix[3][2] = 0;
	rotationMatrixX._matrix[3][3] = 1;
	return rotationMatrixX;

}

/*! \fn function returning rotation matrix with specified rotation angle along Y axis*/
Matrix4 Matrix4::rotateY(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixY;

	//row1
	rotationMatrixY._matrix[0][0] = cosAng;
	//rotationMatrixY._matrix[0][1] = 0;
	rotationMatrixY._matrix[0][2] = sinAng;
	//rotationMatrixY._matrix[0][3] = 0;
	//row2
	//rotationMatrixY._matrix[1][0] = 0;
	rotationMatrixY._matrix[1][1] = 1.0;
	//rotationMatrixY._matrix[1][2] = 0;
	//rotationMatrixY._matrix[1][3] = 0;
	//row3
	rotationMatrixY._matrix[2][0] = -sinAng;
	//rotationMatrixY._matrix[2][1] = 0;
	rotationMatrixY._matrix[2][2] = cosAng;
	//rotationMatrixY._matrix[2][3] = 1;
	//row4
	//rotationMatrixY._matrix[3][0] = 0;
	//rotationMatrixY._matrix[3][1] = 0;
	//rotationMatrixY._matrix[3][2] = 0;
	rotationMatrixY._matrix[3][3] = 1.0;
	return rotationMatrixY;
}

/*! \fn function returning rotation matrix with specified rotation angle along Z axis*/
Matrix4 Matrix4::rotateZ(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixZ;
	//row1
	rotationMatrixZ._matrix[0][0] = cosAng;
	rotationMatrixZ._matrix[0][1] = -sinAng;
	//rotationMatrixZ._matrix[0][2] = 0;
	//rotationMatrixZ._matrix[0][3] = 0;
	//row2
	rotationMatrixZ._matrix[1][0] = sinAng;
	rotationMatrixZ._matrix[1][1] = cosAng;
	//rotationMatrixZ._matrix[1][2] = 0;
	//rotationMatrixZ._matrix[1][3] = 0;
	//row3
	//rotationMatrixZ._matrix[2][0] = 0;
	//rotationMatrixZ._matrix[2][1] = 0;
	rotationMatrixZ._matrix[2][2] = 1.0;
	//rotationMatrixZ._matrix[2][3] = 0;
	//row4
	//rotationMatrixZ._matrix[3][0] = 0;
	//rotationMatrixZ._matrix[3][1] = 0;
	//rotationMatrixZ._matrix[3][2] = 0;
	rotationMatrixZ._matrix[3][3] = 1.0;

	return rotationMatrixZ;
}

/*! \fn function returning rotation matrix with specified rotation angle along specified axis(vector)*/
Matrix4 Matrix4::rotateAngle(const Vector3& v, const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	double T = 1 - cosAng;
	
	Vector3 normalizedVector = v.normalize();
	double x = normalizedVector.x;
	double y = normalizedVector.y;
	double z = normalizedVector.z;
	Matrix4 rotationMatrix;
	//row1
	rotationMatrix._matrix[0][0] = cosAng + (x*x) * T;
	rotationMatrix._matrix[0][1] = x * y * T - z * sinAng;
	rotationMatrix._matrix[0][2] = x * z * T + y * sinAng;
	//rotationMatrix._matrix[0][3] = 0;
	//row2
	rotationMatrix._matrix[1][0] = y * x * T + z * sinAng;
	rotationMatrix._matrix[1][1] = cosAng + y * y * T;
	rotationMatrix._matrix[1][2] = y * z * T - x * sinAng;
	//rotationMatrix._matrix[1][3] = 0;
	//row3
	rotationMatrix._matrix[2][0] = z * x * T - y * sinAng;
	rotationMatrix._matrix[2][1] = z * y * T + x * sinAng;
	rotationMatrix._matrix[2][2] = cosAng + z * z * T;
	//rotationMatrix._matrix[2][3] = 0;
	//row4
	//rotationMatrix._matrix[3][0] = 0;
	//rotationMatrix._matrix[3][1] = 0;
	//rotationMatrix._matrix[3][2] = 0;
	rotationMatrix._matrix[3][3] = 1.0;
	return rotationMatrix;
}

/*! \fn returns translation matrix with specified translation values*/
Matrix4 Matrix4::translation(const double &x, const double &y, const double &z)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[3][0] = x;
	translation._matrix[3][1] = y;
	translation._matrix[3][2] = z;
	return translation;
}

/*! \fn returns translation matrix with specified translation values*/
Matrix4 Matrix4::translation(const Vector3& right)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[3][0] = right.x;
	translation._matrix[3][1] = right.y;
	translation._matrix[3][2] = right.z;
	return translation;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix4 Matrix4::scale(const double &x, const double &y, const double &z)
{
	Matrix4 scaling;
	scaling._matrix[0][0] = x;
	scaling._matrix[1][1] = y;
	scaling._matrix[2][2] = z;
	scaling._matrix[3][3] = 1.0;
	return scaling;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix4 Matrix4::scale(const Vector3& right)
{
	Matrix4 scaling;
	scaling._matrix[0][0] = right.x;
	scaling._matrix[1][1] = right.y;
	scaling._matrix[2][2] = right.z;
	scaling._matrix[3][3] = 1.0;
	return scaling;
}


/*! \fn calculates inverse of matrix4x4 and returns as new one*/
Matrix4 Matrix4::inverse() const
{
	//find determinant
	double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
	a = _matrix[0][0]; 	b = _matrix[0][1]; 	c = _matrix[0][2];	d = _matrix[0][3];
	e = _matrix[1][0]; 	f = _matrix[1][1]; 	g = _matrix[1][2];	h = _matrix[1][3];
	i = _matrix[2][0]; 	j = _matrix[2][1]; 	k = _matrix[2][2];	l = _matrix[2][3];
	m = _matrix[3][0];	n = _matrix[3][1];	o = _matrix[3][2];	p = _matrix[3][3];

	double det = a*(Matrix3::det(f, g, h, j, k, l, n, o, p)) - b*(Matrix3::det(e, g, h, i, k, l, m, o, p)) + c*(Matrix3::det(e, f, h, i, j, l, m, n, p)) - d*(Matrix3::det(e, f, g, i, j, k, m, n, o));
	//cout << det << endl;
	Matrix4 temp;
	//matrix of minors(matrices 3x3, calculating their determinants), transpose and change signs in one
	temp._matrix[0][0] = (Matrix3::det(f, g, h, j, k, l, n, o, p));	temp._matrix[0][1] = -(Matrix3::det(b, c, d, j, k, l, n, o, p));	temp._matrix[0][2] = (Matrix3::det(b, c, d, f, g, h, n, o, p));	temp._matrix[0][3] = -(Matrix3::det(b, c, d, f, g, h, j, k, l));			// + - + -
	temp._matrix[1][0] = -(Matrix3::det(e, g, h, i, k, l, m, o, p));	temp._matrix[1][1] = (Matrix3::det(a, c, d, i, k, l, m, o, p));	temp._matrix[1][2] = -(Matrix3::det(a, c, d, e, g, h, m, o, p));	temp._matrix[1][3] = (Matrix3::det(a, c, d, e, g, h, i, k, l));			// - + - +
	temp._matrix[2][0] = (Matrix3::det(e, f, h, i, j, l, m, n, p));	temp._matrix[2][1] = -(Matrix3::det(a, b, d, i, j, l, m, n, p));	temp._matrix[2][2] = (Matrix3::det(a, b, d, e, f, h, m, n, p));	temp._matrix[2][3] = -(Matrix3::det(a, b, d, e, f, h, i, j, l)); 		// + - + -
	temp._matrix[3][0] = -(Matrix3::det(e, f, g, i, j, k, m, n, o));	temp._matrix[3][1] = (Matrix3::det(a, b, c, i, j, k, m, n, o));	temp._matrix[3][2] = -(Matrix3::det(a, b, c, e, f, g, m, n, o));	temp._matrix[3][3] = (Matrix3::det(a, b, c, e, f, g, i, j, k));			// - + - +
	double oneByDet = 1.0 / det;

	temp = oneByDet*temp;

	return temp;
}

Matrix4 Matrix4::CalculateRelativeTransform(const Matrix4 & parent, const Matrix4 & child)
{
	return child * parent.inverse();
}

/*! \fn returns transposed perspective projection matrix specified with given parameters*/
Matrix4 Matrix4::sPerspective(const double& near, const double& far, const double& fov)
{
	double PI = 3.14159265;
	double S = 1.0 / tan(((fov * 0.5) * (PI / 180.0)));
	Matrix4 temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);
	temp._matrix[2][3] = -far * near / (far - near);
	temp._matrix[3][2] = -1.0;
	//temp._matrix[3][3] = 0.0;

	return temp;
}

/*! \fn function returning perspective projection specified with given parameters*/
Matrix4 Matrix4::perspective(const double &near, const double &far, const double &fov)
{
	double PI = 3.14159265;
	double S = 1.0 / tan((fov * 0.5) * (PI / 180.0));
	Matrix4 temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);
	temp._matrix[3][2] = -far * near / (far - near);
	temp._matrix[2][3] = -1.0;
	//temp._matrix[3][3] = 0;

	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::sOrthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (top - bottom);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[0][3] = -((right + left) / (right - left));
	temp._matrix[1][3] = -((top + bottom) / (top - bottom));
	temp._matrix[2][3] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

Matrix4 Matrix4::sOrthographicTopToBottom(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (bottom - top);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[0][3] = -((right + left) / (right - left));
	temp._matrix[1][3] = -((top + bottom) / (bottom - top));
	temp._matrix[2][3] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::orthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (top - bottom);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[3][0] = -((right + left) / (right - left));
	temp._matrix[3][1] = -((top + bottom) / (top - bottom));
	temp._matrix[3][2] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::orthographicTopToBottom(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (bottom - top);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[3][0] = -((right + left) / (right - left));
	temp._matrix[3][1] = -((top + bottom) / (bottom - top));
	temp._matrix[3][2] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn identity matrix */
/*
Matrix4 Matrix4::identityMatrix()
{
	Matrix4 temp;
	temp._matrix[0][0] = 1.0;
	temp._matrix[1][1] = 1.0;
	temp._matrix[2][2] = 1.0;
	temp._matrix[3][3] = 1.0;
	return temp;
}
*/

/*! \fn bias shadowmap matrix*/
Matrix4 Matrix4::biasMatrix()
{
	Matrix4 temp;
	temp._matrix[0][0] = 0.5;
	temp._matrix[1][1] = 0.5;
	temp._matrix[2][2] = 0.5;
	temp._matrix[3][3] = 1.0;
	temp._matrix[3][0] = 0.5;
	temp._matrix[3][1] = 0.5;
	temp._matrix[3][2] = 0.5;
	return temp;
}

/*! \fn returns transposed translation matrix with specified translation values*/
Matrix4 Matrix4::sTranslate(const double &x, const double &y, const double &z)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[0][3] = x;
	translation._matrix[1][3] = y;
	translation._matrix[2][3] = z;
	return translation;
}

/*! \fn lookAt matrix not optimized*/
Matrix4 Matrix4::nolookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalize();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalize(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4 orientation;
	orientation[0][0] = xaxis.x;
	orientation[0][1] = yaxis.x;
	orientation[0][2] = zaxis.x;

	orientation[1][0] = xaxis.y;
	orientation[1][1] = yaxis.y;
	orientation[1][2] = zaxis.y;

	orientation[2][0] = xaxis.z;
	orientation[2][1] = yaxis.z;
	orientation[2][2] = zaxis.z;

	orientation[3][3] = 1.0;

	// Create a 4x4 translation matrix.
	// The eye position is negated which is equivalent
	// to the inverse of the translation matrix.
	// T(v)^-1 == T(-v)
	Matrix4 translation;
	translation[0][0] = 1.0;

	translation[1][1] = 1.0;

	translation[2][2] = 1.0;

	translation[3][0] = -eye.x;
	translation[3][1] = -eye.y;
	translation[3][2] = -eye.z;
	translation[3][3] = 1.0;

	// Combine the orientation and translation to compute
	// the final view matrix
	return (translation * orientation);

}

/*! \fn lookAt matrix optimized*/
Matrix4 Matrix4::lookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalize();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalize(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4 orientation;
	orientation[0][0] = xaxis.x;
	orientation[0][1] = yaxis.x;
	orientation[0][2] = zaxis.x;

	orientation[1][0] = xaxis.y;
	orientation[1][1] = yaxis.y;
	orientation[1][2] = zaxis.y;

	orientation[2][0] = xaxis.z;
	orientation[2][1] = yaxis.z;
	orientation[2][2] = zaxis.z;

	orientation[3][0] = -(xaxis.dot(eye));
	orientation[3][1] = -(yaxis.dot(eye));
	orientation[3][2] = -(zaxis.dot(eye));
	orientation[3][3] = 1.0;

	// the final view matrix
	return orientation;

}

// Pitch should be in the range of [-90 ... 90] degrees and yaw
// should be in the range of [0 ... 360] degrees.
/*! \fn fps cam matrix*/
Matrix4 Matrix4::FPScam(Vector3 eye, const double &pitch, const double &yaw)
{
	// If the pitch and yaw angles are in degrees,
	// they need to be converted to radians. Here
	// I assume the values are already converted to radians.
	double cosPitch = cos(pitch);
	double sinPitch = sin(pitch);
	double cosYaw = cos(yaw);
	double sinYaw = sin(yaw);

	Vector3 xaxis = Vector3(cosYaw, 0.0, -sinYaw);
	Vector3 yaxis = Vector3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	Vector3 zaxis = Vector3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	Matrix4 FPSView;
	FPSView[0][0] = xaxis.x;
	FPSView[0][1] = yaxis.x;
	FPSView[0][2] = zaxis.x;
	//FPSView[0][3] = 0;

	FPSView[1][0] = xaxis.y;
	FPSView[1][1] = yaxis.y;
	FPSView[1][2] = zaxis.y;
	//FPSView[1][3] = 0;

	FPSView[2][0] = xaxis.z;
	FPSView[2][1] = yaxis.z;
	FPSView[2][2] = zaxis.z;
	//FPSView[2][3] = 0;

	FPSView[3][0] = -(xaxis.dot(eye));
	FPSView[3][1] = -(yaxis.dot(eye));
	FPSView[3][2] = -(zaxis.dot(eye));
	FPSView[3][3] = 1.0;

	return FPSView;
}

/*! \fn fustrum matrix used for building perspective with aspect ratio*/
Matrix4 Matrix4::Frustum(const double &l, const double &r, const double &b, const double &t, const double &n, const double &f)
{
	Matrix4 fustrum;

	fustrum._matrix[0][0] = 2.0 * n / (r - l);
	//fustrum._matrix[0][1] = 0;
	//fustrum._matrix[0][2] = 0;
	//fustrum._matrix[0][3] = 0;

	//fustrum._matrix[1][0] = 0;
	fustrum._matrix[1][1] = 2.0 * n / (t - b);
	//fustrum._matrix[1][2] = 0;
	//fustrum._matrix[1][3] = 0;

	fustrum._matrix[2][0] = (r + l) / (r - l);
	fustrum._matrix[2][1] = (t + b) / (t - b);
	fustrum._matrix[2][2] = -(f + n) / (f - n);
	fustrum._matrix[2][3] = -1.0;

	//fustrum._matrix[3][0] = 0;
	//fustrum._matrix[3][1] = 0;
	fustrum._matrix[3][2] = -2.0 * f * n / (f - n);
	//fustrum._matrix[3][3] = 0;

	return fustrum;
}

/*! \fn transposed fustrum matrix used for building transposed perspective with aspect ratio*/
Matrix4 Matrix4::sFrustum(const double &left, const double &right, const double &bottom, const double &top, const double &near, const double &far)
{
	Matrix4 fustrum;

	fustrum._matrix[0][0] = 2.0 * near / (right - left);
	//fustrum._matrix[0][1] = 0.0;
	fustrum._matrix[0][2] = (right + left) / (right - left);
	//fustrum._matrix[0][3] = 0.0;

	//fustrum._matrix[1][0] = 0.0;
	fustrum._matrix[1][1] = 2.0 * near / (top - bottom);
	fustrum._matrix[1][2] = (top + bottom) / (top - bottom);
	//fustrum._matrix[1][3] = 0.0;

	//fustrum._matrix[2][0] = 0.0;
	//fustrum._matrix[2][1] = 0.0;
	fustrum._matrix[2][2] = -(far + near) / (far - near);
	fustrum._matrix[2][3] = -2.0 * far * near / (far - near);

	//fustrum._matrix[3][0] = 0.0;
	//fustrum._matrix[3][1] = 0.0;
	fustrum._matrix[3][2] = -1.0;
	//fustrum._matrix[3][3] = 0.0;

	return fustrum;
}

/*! \fn transposed perspective matrix with aspect ratio*/
Matrix4 Matrix4::sOpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far)
{
	double PI = 3.14159265;
	double scale = tan((fov * 0.5)*(PI / 180.0)) * near;
	double right = imageAspectRatio * scale;
	double left = -right;
	double top = scale;
	double bottom = -top;
	return sFrustum(left, right, bottom, top, near, far);
}

/*! \fn perspective matrix with aspect ratio*/
Matrix4 Matrix4::OpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far)
{
	double PI = 3.14159265;
	double scale = tan((fov * 0.5)*(PI / 180.0)) * near;
	double right = imageAspectRatio * scale;
	double left = -right;
	double top = scale;
	double bottom = -top;
	return Frustum(left, right, bottom, top, near, far);
}

//no need to transpose that either
Matrix3 Matrix4::convertToMatrix3() const
{
	Matrix3 mat3;
	mat3[0][0] = _matrix[0][0];
	mat3[0][1] = _matrix[0][1];
	mat3[0][2] = _matrix[0][2];

	mat3[1][0] = _matrix[1][0];
	mat3[1][1] = _matrix[1][1];
	mat3[1][2] = _matrix[1][2];

	mat3[2][0] = _matrix[2][0];
	mat3[2][1] = _matrix[2][1];
	mat3[2][2] = _matrix[2][2];

	return mat3;
}

Vector3 Matrix4::getScale() const
{
	return Vector3(_matrix[0][0], _matrix[1][1], _matrix[2][2]);
}

void Matrix4::setScale(const Vector3 & scale)
{
	_matrix[0][0] = scale.x;
	_matrix[1][1] = scale.y;
	_matrix[2][2] = scale.z;
}


Vector3 Matrix4::getPosition() const
{
	return Vector3(_matrix[3][0], _matrix[3][1], _matrix[3][2]);
}

void Matrix4::setPosition(const Vector3 & position)
{
	_matrix[3][0] = position.x;
	_matrix[3][1] = position.y;
	_matrix[3][2] = position.z;
}

void Matrix4::translate(const Vector3 & position)
{
	_matrix[3][0] += position.x;
	_matrix[3][1] += position.y;
	_matrix[3][2] += position.z;
}

void Matrix4::operator*=(const Matrix4 & right)
{
	Matrix4 temp;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			for (int k = 0; k < 4; k++)
			{
				temp._matrix[r][c] = temp._matrix[r][c] + _matrix[r][k] * right._matrix[k][c];
			}
		}
	}
	*this = temp;
}

void Matrix4::setIdentity()
{
	memset(_matrix, 0, sizeof _matrix);
	_matrix[0][0] = 1.0;
	_matrix[1][1] = 1.0;
	_matrix[2][2] = 1.0;
	_matrix[3][3] = 1.0;
}

void Matrix4::clear()
{
	memset(_matrix, 0, sizeof _matrix);
}

void Matrix4::zeroPosition()
{
	_matrix[3][0] = 0.0;
	_matrix[3][1] = 0.0;
	_matrix[3][2] = 0.0;
}

void Matrix4::resetScale()
{
	_matrix[0][0] = 1.0;
	_matrix[1][1] = 1.0;
	_matrix[2][2] = 1.0;
}

void Matrix4::resetRotation()
{
	_matrix[0][0] = 1.0;
	_matrix[0][1] = 0.0;
	_matrix[0][2] = 0.0;
	_matrix[1][0] = 0.0;
	_matrix[1][1] = 1.0;
	_matrix[1][2] = 0.0;
	_matrix[2][0] = 0.0;
	_matrix[2][1] = 0.0;
	_matrix[2][2] = 1.0;
}

Vector3 Matrix4::getLeft() const
{
	return Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalize();
}

Vector3 Matrix4::getInvLeft() const
{
	return Vector3(_matrix[0][0], _matrix[1][0], _matrix[2][0]).normalize();
}

Vector3 Matrix4::getUp() const
{
	return Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalize();
}

Vector3 Matrix4::getInvUp() const
{
	return Vector3(_matrix[0][1], _matrix[1][1], _matrix[2][1]).normalize();
}

Vector3 Matrix4::getBack() const
{
	return Vector3(_matrix[2][0] * -1.0, _matrix[2][1] * -1.0, _matrix[2][2] * -1.0).normalize();
}

Vector3 Matrix4::getInvBack() const
{
	return Vector3(_matrix[0][2] * -1.0, _matrix[1][2] * -1.0, _matrix[2][2] * -1.0).normalize();
}

Vector3 Matrix4::getForward() const
{
	return Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalize();
}

Vector3 Matrix4::getInvForward() const
{
	return Vector3(_matrix[0][2], _matrix[1][2], _matrix[2][2]).normalize();
}

Vector3 Matrix4::getAxis(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]);
}

Vector3 Matrix4::getAxisNormalized(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]).normalize();
}

void Matrix4::setUp(const Vector3& axis)
{
	_matrix[1][0] = axis.x;
	_matrix[1][1] = axis.y;
	_matrix[1][2] = axis.z;
}

void Matrix4::setRight(const Vector3& axis)
{
	_matrix[0][0] = axis.x;
	_matrix[0][1] = axis.y;
	_matrix[0][2] = axis.z;
}

void Matrix4::setForward(const Vector3& axis)
{
	_matrix[2][0] = axis.x;
	_matrix[2][1] = axis.y;
	_matrix[2][2] = axis.z;
}

Vector3 Matrix4::extractScale() const
{
	double scaleX = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).lengt();
	double scaleY = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).lengt();
	double scaleZ = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).lengt();
	return Vector3(scaleX, scaleY, scaleZ);
}
Matrix3 Matrix4::extractRotation3() const
{
	Matrix3 rotation;
	Vector3 xAxis = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalize();
	Vector3 yAxis = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalize();
	Vector3 zAxis = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalize();
	rotation[0][0] = xAxis.x;
	rotation[0][1] = xAxis.y;
	rotation[0][2] = xAxis.z;
	rotation[1][0] = yAxis.x;
	rotation[1][1] = yAxis.y;
	rotation[1][2] = yAxis.z;
	rotation[2][0] = zAxis.x;
	rotation[2][1] = zAxis.y;
	rotation[2][2] = zAxis.z;
	return rotation;
}
Matrix4 Matrix4::extractRotation() const
{
	Matrix4 rotation;
	rotation[3][3] = 1.0;
	Vector3 xAxis = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalize();
	Vector3 yAxis = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalize();
	Vector3 zAxis = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalize();
	rotation[0][0] = xAxis.x;
	rotation[0][1] = xAxis.y;
	rotation[0][2] = xAxis.z;
	rotation[1][0] = yAxis.x;
	rotation[1][1] = yAxis.y;
	rotation[1][2] = yAxis.z;
	rotation[2][0] = zAxis.x;
	rotation[2][1] = zAxis.y;
	rotation[2][2] = zAxis.z;
	return rotation;
}

Quaternion Matrix4::toQuaternion() const
{
	double tr = _matrix[0][0] + _matrix[1][1] + _matrix[2][2];
	Quaternion temp;
	if (tr > 0) {
		double S = sqrt(tr + 1.0) * 2.0; // S=4*qw
		temp.w = 0.25 * S;
		temp.x = (_matrix[1][2] - _matrix[2][1]) / S;
		temp.y = (_matrix[2][0] - _matrix[0][2]) / S;
		temp.z = (_matrix[0][1] - _matrix[1][0]) / S;
	}

	else if ((_matrix[0][0] > _matrix[1][1]) && (_matrix[0][0] > _matrix[2][2])) {
		double S = sqrt(1.0 + _matrix[0][0] - _matrix[1][1] - _matrix[2][2]) * 2.0; // S=4*qx
		temp.w = (_matrix[1][2] - _matrix[2][1]) / S;
		temp.x = 0.25 * S;
		temp.y = (_matrix[1][0] + _matrix[0][1]) / S;
		temp.z = (_matrix[2][0] + _matrix[0][2]) / S;
	}

	else if (_matrix[1][1] > _matrix[2][2]) {
		double S = sqrt(1.0 + _matrix[1][1] - _matrix[0][0] - _matrix[2][2]) * 2.0; // S=4*qy
		temp.w = (_matrix[2][0] - _matrix[0][2]) / S;
		temp.x = (_matrix[1][0] + _matrix[0][1]) / S;
		temp.y = 0.25 * S;
		temp.z = (_matrix[2][1] + _matrix[1][2]) / S;
	}

	else {
		double S = sqrt(1.0 + _matrix[2][2] - _matrix[0][0] - _matrix[1][1]) * 2.0; // S=4*qz
		temp.w = (_matrix[0][1] - _matrix[1][0]) / S;
		temp.x = (_matrix[2][0] + _matrix[0][2]) / S;
		temp.y = (_matrix[2][1] + _matrix[1][2]) / S;
		temp.z = 0.25 * S;
	}
	return temp;
}

double Matrix4::AngleX() const
{
	double PI = 3.14159265;
	double decompRotX = (atan2(_matrix[2][1], _matrix[2][2]) * 180.0) / PI;
	//if (decompRotX > 180.0f) {
	//	decompRotX -= 360.0f;
	//}
	//else if (decompRotX < -180.0f) {
	//	decompRotX += 360.0f;
	//}
	decompRotX = -1.0f * decompRotX;
	return decompRotX;
}

double Matrix4::AngleY() const
{
	double PI = 3.14159265;
	double decompRotY = (atan2(-_matrix[2][0], sqrt((_matrix[2][1] * _matrix[2][1]) + (_matrix[2][2] * _matrix[2][2]))) * 180.0) / PI;
	//if (decompRotY > 180.0f) {
	//	decompRotY -= 360.0f;
	//}
	//else if (decompRotY < -180.0f) {
	//	decompRotY += 360.0f;
	//}
	decompRotY = -1.0f * decompRotY;
	return decompRotY;
}

double Matrix4::AngleZ() const
{
	double PI = 3.14159265;
	double decompRotZ = (atan2(_matrix[1][0], _matrix[0][0]) * 180.0) / PI;
	//if (decompRotZ > 180.0f) {
	//	decompRotZ -= 360.0f;
	//}
	//else if (decompRotZ < -180.0f) {
	//	decompRotZ += 360.0f;
	//}
	decompRotZ = -1.0f * decompRotZ;
	return decompRotZ;
}

Vector3 Matrix4::Angles() const
{
	Vector3 rotation;
	
	rotation.y = asin(-_matrix[2][0]); //atan2f(-_matrix[2][0], sqrtf(_matrix[2][1] * _matrix[2][1] + _matrix[2][2] * _matrix[2][2]));
	double res2 = abs(cos(rotation.y - 0.0));
	if (!(abs(cos(rotation.y - 0.0)) <= 0.0001)) {
		rotation.x = atan2(_matrix[2][1], _matrix[2][2]);
		rotation.z = atan2(_matrix[1][0], _matrix[0][0]);
	}
	else {
		rotation.x = atan2(-_matrix[0][2], _matrix[1][1]);
		rotation.z = 0;
	}
	return -1.0*rotation;
}

void Matrix4::setOrientation(const Quaternion & q)
{
	_matrix[0][0] = 1.0 - 2.0 * q.y*q.y - 2.0 * q.z*q.z;
	_matrix[1][0] = (2.0 * q.x * q.y) - (2.0 * q.w * q.z);
	_matrix[2][0] = (2.0 * q.x * q.z) + (2.0 * q.w * q.y);

	_matrix[0][1] = (2.0 * q.x * q.y) + (2.0 * q.w * q.z);
	_matrix[1][1] = 1.0 - 2.0 * q.x*q.x - 2.0 * q.z*q.z;
	_matrix[2][1] = (2.0 * q.y * q.z) - (2.0 * q.w * q.x);

	_matrix[0][2] = (2.0 * q.x * q.z) - (2.0 * q.w * q.y);
	_matrix[1][2] = (2.0 * q.y * q.z) + (2.0 * q.w * q.x);
	_matrix[2][2] = 1.0 - 2.0 * q.x*q.x - 2.0 * q.y*q.y;
}