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

		Vec3 operator+(const Vec3 &v) const;
		Vec3 operator-(const Vec3 &v) const;
		Vec3 operator*(const Vec3 &v) const;
		Vec3 operator/(const Vec3 &v) const;

		Vec3 operator+=(const Vec3 &v);
		Vec3 operator-=(const Vec3 &v);
		Vec3 operator*=(const Vec3 &v);
		Vec3 operator/=(const Vec3 &v);

		Vec3 operator*(float v) const;
		Vec3 operator/(float v) const;
		Vec3 operator*=(float v);
		Vec3 operator/=(float v);
		Vec3 operator*(int v) const;
		Vec3 operator/(int v) const;
		Vec3 operator*=(int v);
		Vec3 operator/=(int v);

		bool operator==(const Vec3 &v) const;
		bool operator!=(const Vec3 &v) const;
		bool operator==(float v) const;
		bool operator!=(float v) const;
		bool operator==(int v) const;
		bool operator!=(int v) const;

		static Vec3 Max();
		static Vec3 Min();
		static Vec3 Zero();

		static Vec3 Max(const Vec3 &v1, const Vec3 &v2);
		static Vec3 Min(const Vec3 &v1, const Vec3 &v2);

	private:
};



#endif