#ifndef VEC3_H
#define VEC3_H

#include "platform.h"
#include <math.h>
#include <cfloat>

class DLL_API Vec3
{
	public:
		float X;
		float Y;
		float Z;

		Vec3();
		Vec3(float X, float Y);
		Vec3(float X, float Y, float Z);
		Vec3(int X, int Y);
		Vec3(int X, int Y, int Z);

		Vec3(float X, float Y, int Z);
		Vec3(float X, int Y, float Z);
		Vec3(float X, int Y, int Z);
		Vec3(int X, float Y, float Z);
		Vec3(int X, float Y, int Z);
		Vec3(int X, int Y, float Z);

		Vec3(float X, int Y);
		Vec3(int X, float Y);

		float Length();
		Vec3 Normalize();

		float Dot(Vec3 &v);
		Vec3 Cross(Vec3 &v); 
		static float Dot(Vec3 &v1, Vec3 &v2);
		static Vec3 Cross(Vec3 &v1, Vec3 &v2);

		Vec3 operator+(Vec3 &v);
		Vec3 operator-(Vec3 &v);
		Vec3 operator*(Vec3 &v);
		Vec3 operator/(Vec3 &v);

		Vec3 operator+=(Vec3 &v);
		Vec3 operator-=(Vec3 &v);
		Vec3 operator*=(Vec3 &v);
		Vec3 operator/=(Vec3 &v);

		Vec3 operator*(float v);
		Vec3 operator/(float v);
		Vec3 operator*=(float v);
		Vec3 operator/=(float v);
		Vec3 operator*(int v);
		Vec3 operator/(int v);
		Vec3 operator*=(int v);
		Vec3 operator/=(int v);

		bool operator==(Vec3 &v);
		bool operator!=(Vec3 &v);
		bool operator==(float v);
		bool operator!=(float v);
		bool operator==(int v);
		bool operator!=(int v);

		static Vec3 Max();
		static Vec3 Min();
		static Vec3 Zero();

		static Vec3 Max(Vec3 v1, Vec3 v2);
		static Vec3 Min(Vec3 v1, Vec3 v2);

	private:
};



#endif