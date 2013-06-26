#pragma once

#include <math.h>

class Vector2;
inline float DotProduct(const Vector2 &va, const Vector2 &vb);
inline float Magnitude(const Vector2 &v);


class Vector2
{
public :
	Vector2() {};
	Vector2(float xi, float yi);
	Vector2(int xi, int yi);
	Vector2( int p[] );

	float Dot( Vector2 v) { return DotProduct( *this, v ); }
	float Length() const  { return Magnitude( *this ); }
	void  Normalize()	  { float len = Length(); x /= len; y /= len; }
	Vector2 & operator  = (const int pos[]);
	Vector2 & operator += (const Vector2 &v);
	Vector2 & operator -= (const Vector2 &v);
	Vector2 & operator *= (const Vector2 &m);
	Vector2 & operator *= (float scale);
	Vector2 operator - () const;

	bool operator == (const Vector2& Vec) const
	{
		if(x == Vec.x && y == Vec.y ) return true;
		else return false;
	}
#pragma warning(disable:4201)
	union {
		struct {
			float x, y;
		} ;
		float v[2];
	} ;
#pragma warning(default:4201)
};

float GetNearPos(Vector2& po_Out, const Vector2& p, const Vector2& s0, const Vector2& s1);


inline Vector2::Vector2(float xi, float yi) {
	x = xi, y = yi;
}
inline Vector2::Vector2(int xi, int yi) {
	x = (float)xi, y = (float)yi;
}

inline Vector2::Vector2(int p[]) {
	x = (float)p[0], y = (float)p[1];
}

inline float DotProduct(const Vector2 &va, const Vector2 &vb) 
{
	return va.x*vb.x + va.y*vb.y;
}

inline float Magnitude(const Vector2 &v) {
	return (float)sqrt(DotProduct(v, v));
}

inline Vector2& Vector2::operator= (const int pos[] ) {
	x = (float)pos[0];
	y = (float)pos[1];
	return *this;
}

inline Vector2 & Vector2 :: operator += (const Vector2 &v) {
	x += v.x; y += v.y;
	return *this;
}
inline Vector2 & Vector2 :: operator -= (const Vector2 &v) {
	x -= v.x; y -= v.y;
	return *this;
}
inline Vector2 & Vector2 :: operator *= (float scale) {
	x *= scale, y *= scale;
	return *this;
}

inline Vector2 Vector2::operator - () const {
	return Vector2(-x, -y);
}

inline Vector2 & Vector2Sub(Vector2 &vo, const Vector2 &va, const Vector2 &vb) {
	vo.x = va.x - vb.x, vo.y = va.y - vb.y;
	return vo;
}
inline Vector2 & Vector2Add(Vector2 &vo, const Vector2 &va, const Vector2 &vb) {
	vo.x = va.x + vb.x, vo.y = va.y+vb.y;
	return vo;
}
inline Vector2 & Vector2Mul(Vector2 &vo, const Vector2 &vi, float scale) {
	vo.x = vi.x * scale, vo.y = vi.y * scale;
	return vo;
}


inline Vector2 operator - (const Vector2 &va, const Vector2 &vb) {
	Vector2 v;
	return Vector2Sub(v, va, vb);
}
inline Vector2 operator + (const Vector2 &va, const Vector2 &vb) {
	Vector2 v;
	return Vector2Add(v, va, vb);
}
inline Vector2 operator * (const Vector2 &vin, float scale) {
	Vector2 v;
	return Vector2Mul(v, vin, scale);
}
inline Vector2 operator * (float scale, const Vector2 &vin) {
	Vector2 v;
	return Vector2Mul(v, vin, scale);
}