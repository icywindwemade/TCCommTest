/**
 * @file	MathBase.h
 *
 * Declares the mathematics base class.
 */
#ifndef __MATHBASE_H__
#define __MATHBASE_H__

#include "Core.h"
#include <math.h>

// Constants.
#undef	PI
#define PI					(3.1415926535897932)
#define SMALL_NUMBER		(1.e-8)
#define KINDA_SMALL_NUMBER	(1.e-4)
#define EULERS_NUMBER		(2.71828182845904523536)

// Aux constants.
#define INV_PI			(0.31830988618)
#define HALF_PI			(1.57079632679)

// Magic numbers for numerical precision.
#define DELTA			(0.00001f)

//#ifndef INDEX_NONE
//#define INDEX_NONE	-1
//#endif	// INDEX_NONE

//
//__forceinline std::string format_arg_list(const char *fmt, va_list args) 
//{ 
//	if (!fmt) return ""; 
//
//	int   result = -1, length = 256; 
//	char *buffer = 0; 
//
//	while (result == -1) 
//	{ 
//		if (buffer) delete [] buffer; 
//		buffer = new char [length + 1]; 
//		memset(buffer, 0, length + 1); 
//		result = vsnprintf(buffer, length, fmt, args); 
//		length *= 2; 
//	} 
//
//	std::string s(buffer); 
//	delete [] buffer; 
//	return s; 
//} 
//
//__forceinline std::string PrintfToSTLString(const char *fmt, ...) 
//{ 
//	va_list args;
//	va_start(args, fmt); 
//	std::string s = format_arg_list(fmt, args); 
//	va_end(args); 
//	return s; 
//}

//
// NIntPoint
//
struct FIntPoint
{
	int	X, Y;
	
	FIntPoint()
		: X(0)
		, Y(0)
	{
	}
	
	FIntPoint(int InX, int InY)
		: X(InX)
		, Y(InY)
	{
	}
	
	static FIntPoint ZeroValue()
	{
		return FIntPoint(0, 0);
	}
	
	static FIntPoint NoneValue()
	{
		return FIntPoint(INDEX_NONE, INDEX_NONE);
	}
	
	const int& operator()(int i) const
	{
		return (&X)[i];
	}
	
	int& operator()(int i)
	{
		return (&X)[i];
	}
	
	static int Num()
	{
		return 2;
	}
	
	bool operator==(const FIntPoint& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}
	
	bool operator!=(const FIntPoint& Other) const
	{
		return X != Other.X || Y != Other.Y;
	}
	
	FIntPoint& operator*=(int Scale)
	{
		X *= Scale;
		Y *= Scale;
		
		return *this;
	}
	
	FIntPoint& operator+=(const FIntPoint& Other)
	{
		X += Other.X;
		Y += Other.Y;
		
		return *this;
	}
	
	FIntPoint& operator-=(const FIntPoint& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		
		return *this;
	}
	
	FIntPoint operator*(int Scale) const
	{
		return FIntPoint(*this) *= Scale;
	}
	
	FIntPoint operator+(const FIntPoint& Other) const
	{
		return FIntPoint(*this) += Other;
	}
	
	FIntPoint operator-(const FIntPoint& Other) const
	{
		return FIntPoint(*this) -= Other;
	}
	
	int Size() const
	{
		return int( appSqrt( FLOAT( X * X + Y * Y ) ) );
	}

	FLOAT Dist( const FIntPoint& Other)
	{
		return appSqrt( FLOAT( ( (X - Other.X) * (X - Other.X) ) + ( (Y - Other.Y) * (Y - Other.Y) ) ) );
	}
};

//
// A 2x1 of FLOATs.
//
struct FVector2D
{
	float X, Y;
	
	// Constructors.
	inline FVector2D()
		: X(0)
		, Y(0)
	{
	}
	
	inline FVector2D(float InX, float InY)
		: X(InX)
		, Y(InY)
	{
	}

	inline FVector2D(int InX, int InY)
		: X((float)InX)
		, Y((float)InY)
	{
	}
	
	inline FVector2D(FIntPoint InPos)
	{
		X = (float)InPos.X;
		Y = (float)InPos.Y;
	}
	
	// Binary math operators.
	inline FVector2D operator+(const FVector2D& V) const
	{
		return FVector2D(X + V.X, Y + V.Y);
	}
	
	inline FVector2D operator-(const FVector2D& V) const
	{
		return FVector2D(X - V.X, Y - V.Y);
	}
	
	inline FVector2D operator*(float Scale) const
	{
		return FVector2D(X * Scale, Y * Scale);
	}
	
	FVector2D operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return FVector2D(X * RScale, Y * RScale);
	}
	
	inline FVector2D operator*(const FVector2D& V) const
	{
		return FVector2D(X * V.X, Y * V.X);
	}
	
	inline float operator|(const FVector2D& V) const
	{
		return X * V.X + Y * V.Y;
	}
	
	inline float operator^(const FVector2D& V) const
	{
		return X * V.Y - Y * V.X;
	}
	
	// Binary comparison operators.
	bool operator==(const FVector2D& V) const
	{
		return X == V.X && Y == V.Y;
	}
	
	bool operator!=(const FVector2D& V) const
	{
		return X != V.X || Y != V.Y;
	}
	
	bool operator<(const FVector2D& Other) const
	{
		return X < Other.X && Y < Other.Y;
	}
	
	bool operator>(const FVector2D& Other) const
	{
		return X > Other.X && Y > Other.Y;
	}
	
	bool operator<=(const FVector2D& Other) const
	{
		return X <= Other.X && Y <= Other.Y;
	}
	
	bool operator>=(const FVector2D& Other) const
	{
		return X >= Other.X && Y >= Other.Y;
	}
	
	// Error-tolerant comparison.
	bool Equals(const FVector2D& V, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return abs(X - V.X) < Tolerance && abs(Y - V.Y) < Tolerance;
	}
	
	// Unary operators.
	inline FVector2D operator-() const
	{
		return FVector2D(-X, -Y);
	}
	
	// Assignment operators.
	inline FVector2D operator+=(const FVector2D& V)
	{
		X += V.X;
		Y += V.Y;
		
		return *this;
	}
	
	inline FVector2D operator-=(const FVector2D& V)
	{
		X -= V.X;
		Y -= V.Y;
		
		return *this;
	}
	
	inline FVector2D operator*=(float Scale)
	{
		X *= Scale;
		Y *= Scale;
		
		return *this;
	}
	
	FVector2D operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV;
		return *this;
	}
	
	FVector2D operator*=(const FVector2D& V)
	{
		X *= V.X;
		Y *= V.Y;
		
		return *this;
	}
	
	FVector2D operator/=(const FVector2D& V)
	{
		X /= V.X;
		Y /= V.Y;
		
		return *this;
	}
	
	float& operator[](int i)
	{
		return ((i == 0) ? X : Y);
	}
	
	float operator[](int i) const
	{
		return ((i == 0) ? X : Y);
	}
	
	// Simple functions.
	void Set(float InX, float InY)
	{
		X = InX;
		Y = InY;
	}
	
	float GetMax() const
	{
		return Max(X, Y);
	}
	
	float GetAbsMax() const
	{
		return Max(abs(X), abs(Y));
	}
	
	float GetMin() const
	{
		return Min(X, Y);
	}
	
	float Size() const
	{
		return appSqrt(X * X + Y * Y);
	}
	
	float SizedSquared() const
	{
		return X * X + Y * Y;
	}
	
	FVector2D SafeNormal(float Tolerance = SMALL_NUMBER) const
	{
		const float SquareSum = X * X + Y * Y;
		if (SquareSum > Tolerance)
		{
			const float Scale = appInvSqrt(SquareSum);
			return FVector2D(X * Scale , Y * Scale);
		}
		
		return FVector2D(0.f, 0.f);
	}
	
	void Normalize(float Tolerance = SMALL_NUMBER)
	{
		const float SquareSum = X * X + Y * Y;
		if (SquareSum > Tolerance)
		{
			const float Scale = appInvSqrt(SquareSum);
			X *= Scale;
			Y *= Scale;
			
			return;
		}
		
		X = 0.0f;
		Y = 0.0f;
	}
	
	int IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return abs(X) < Tolerance && abs(Y) < Tolerance;
	}
	
	bool IsZero() const
	{
		return X == 0.f && Y == 0.f;
	}
	
	float& Component(int Index)
	{
		return (&X)[Index];
	}
	
	FIntPoint IntPoint() const
	{
		return FIntPoint(appRound(X), appRound(Y));
	}
	
	//std::string ToString() const
	//{
	//	return PrintfToSTLString("X=%3.3f Y=%3.3f", X, Y);
	//}
};

inline FVector2D operator*(float Scale, const FVector2D& V)
{
	return V.operator*(Scale);
}

inline float GetAngle(FVector2D V1, FVector2D V2)
{
	float Angle;
	FVector2D Delta;
	
	
	Delta = V2 - V1;
	FVector2D DeltaNormal = Delta;
	float Length = Delta.Size();
	DeltaNormal.X = Delta.X / Length;
	DeltaNormal.Y = Delta.Y / Length;
	
	if (DeltaNormal.X == 0.0f)
	{
		DeltaNormal.X = DELTA;
	}
	
	if (DeltaNormal.X <= 0.0f)
	{
		Angle = float(atan(DeltaNormal.Y / DeltaNormal.X) - HALF_PI);
	}
	else
	{
		Angle = float(atan(DeltaNormal.Y / DeltaNormal.Y) + HALF_PI);
	}
	
	Angle *= 57.32484076f;
	
	while (Angle < 0.0f)
	{
		Angle += 360.f;
	}
	
	while (Angle > 360.0f)
	{
		Angle -= 360.0f;
	}
	
	return Angle;
};

//
// NSize2D
//
struct FSize2D
{
	float	Width;
	float	Height;
	
	FSize2D()
		: Width(0)
		, Height(0)
	{
	}
	
	FSize2D(float InWidth, float InHeight)
		: Width(InWidth)
		, Height(InHeight)
	{
	}
};


#endif	// __MATHBASE_H__