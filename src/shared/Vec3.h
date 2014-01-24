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

		template<typename T1, typename T2>
		Vec3(T1 X, T2 Y)
		{
			this->X = (float)X;
			this->Y = (float)Y;
			this->Z = 0.0f;
		}

		template<typename T1, typename T2, typename T3>
		Vec3(T1 X, T2 Y, T3 Z)
		{
			this->X = (float)X;
			this->Y = (float)Y;
			this->Z = (float)Z;
		}

		float Length() const;
		Vec3 Normalize();

		float Dot(const Vec3 &v) const;
		Vec3 Cross(const Vec3 &v) const; 
		static float Dot(const Vec3 &v1, const Vec3 &v2);
		static Vec3 Cross(const Vec3 &v1, const Vec3 &v2);

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