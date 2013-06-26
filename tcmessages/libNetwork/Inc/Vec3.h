#pragma once

#include <math.h>

class Vector3;
inline float DotProduct(const Vector3 &va, const Vector3 &vb);
inline float Magnitude(const Vector3 &v);


class Vector3
{
public :
	Vector3() {};
	Vector3(float fX, float fY, float fZ);
	Vector3(int xi, int yi, int zi );
	Vector3( int p[] );

	float Dot( Vector3 v) { return DotProduct( *this, v ); }
	float Length() const  { return Magnitude( *this ); }
	void  Normalize()	  { float len = Length(); x /= len; y /= len; z/=len; }
	Vector3 & operator  = (const int pos[]);
	Vector3 & operator += (const Vector3 &v);
	Vector3 & operator -= (const Vector3 &v);
	Vector3 & operator *= (const Vector3 &m);
	Vector3 & operator *= (float scale);
	Vector3 operator - () const;

	void Get2DPos( int &xi, int &yi ) { xi = (int)x, yi = (int)(y + z); }

	bool operator == (const Vector3& Vec) const
	{
		if(x == Vec.x && y == Vec.y && z == Vec.z ) return true;
		else return false;
	}

	static const Vector3 ZERO;					// (0, 0, 0)
	static const Vector3 AXISX;					// (1, 0, 0)
	static const Vector3 AXISY;					// (0, 1, 0)
	static const Vector3 AXISZ;					// (0, 0, 1)
#pragma warning(disable:4201)
	union {
		struct {
			float x, y, z;
		} ;
		float v[3];
	} ;
#pragma warning(default:4201)
};

float GetNearPos(Vector3& po_Out, const Vector3& p, const Vector3& s0, const Vector3& s1);


inline Vector3::Vector3(float fX, float fY, float fZ) {
	x = fX, y = fY; z = fZ;
}
inline Vector3::Vector3(int xi, int yi, int zi ) {
	x = (float)xi, y = (float)yi; z = (float)zi;
}

inline Vector3::Vector3(int p[]) {
	x = (float)p[0], y = (float)p[1], z = (float)p[2];
}

inline float DotProduct(const Vector3 &va, const Vector3 &vb) 
{
	return va.x*vb.x + va.y*vb.y + va.z*vb.z;
}

inline float Magnitude(const Vector3 &v) {
	return (float)sqrt(DotProduct(v, v));
}

inline Vector3& Vector3::operator= (const int pos[] ) {
	x = (float)pos[0];
	y = (float)pos[1];
	z = (float)pos[2];
	return *this;
}

inline Vector3 & Vector3 :: operator += (const Vector3 &v) {
	x += v.x; y += v.y; z +=v.z;
	return *this;
}
inline Vector3 & Vector3 :: operator -= (const Vector3 &v) {
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}
inline Vector3 & Vector3 :: operator *= (float scale) {
	x *= scale, y *= scale; z *= scale;
	return *this;
}

inline Vector3 Vector3::operator - () const {
	return Vector3(-x, -y, -z);
}

inline Vector3 & Vector3Sub(Vector3 &vo, const Vector3 &va, const Vector3 &vb) {
	vo.x = va.x - vb.x, vo.y = va.y - vb.y; vo.x = va.z - vb.z;
	return vo;
}
inline Vector3 & Vector3Add(Vector3 &vo, const Vector3 &va, const Vector3 &vb) {
	vo.x = va.x + vb.x, vo.y = va.y+vb.y; vo.z = va.z + vb.z;
	return vo;
}
inline Vector3 & Vector3Mul(Vector3 &vo, const Vector3 &vi, float scale) {
	vo.x = vi.x * scale, vo.y = vi.y * scale; vo.z = vi.z *scale;
	return vo;
}


inline Vector3 operator - (const Vector3 &va, const Vector3 &vb) {
	Vector3 v;
	return Vector3Sub(v, va, vb);
}
inline Vector3 operator + (const Vector3 &va, const Vector3 &vb) {
	Vector3 v;
	return Vector3Add(v, va, vb);
}
inline Vector3 operator * (const Vector3 &vin, float scale) {
	Vector3 v;
	return Vector3Mul(v, vin, scale);
}
inline Vector3 operator * (float scale, const Vector3 &vin) {
	Vector3 v;
	return Vector3Mul(v, vin, scale);
}