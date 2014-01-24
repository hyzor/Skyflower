#include <algorithm>
#include "Vec3.h"

Vec3::Vec3()
{
	this->X = 0.0f;
	this->Y = 0.0f;
	this->Z = 0.0f;
}

Vec3::Vec3(float X, float Y)
{
	this->X = X;
	this->Y = Y;
	this->Z = 0.0f;
}

Vec3::Vec3(float X, float Y, float Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

float Vec3::Length() const
{
	return sqrtf(this->X*this->X + this->Y*this->Y + this->Z*this->Z);
}
Vec3 Vec3::Normalize()
{
	float invLength = 1.0f / this->Length();
	*this *= invLength;
	return *this;
}

float Vec3::Dot(const Vec3 &v) const
{
	return Dot(*this, v);
}
Vec3 Vec3::Cross(const Vec3 &v) const
{
	return Cross(*this, v);
}

float Vec3::Dot(const Vec3 &v1, const Vec3 &v2)
{
	return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
}

Vec3 Vec3::Cross(const Vec3 &v1, const Vec3 &v2)
{
	Vec3 res;
	res.X = v1.Y*v2.Z - v1.Z*v2.Y;
	res.Y = v1.Z*v2.X - v1.X*v2.Z;
	res.Z = v1.X*v2.Y - v1.Y*v2.X;
	return res;
}

Vec3 Vec3::operator+(const Vec3 &v)
{
	return Vec3(this->X+v.X, this->Y+v.Y, this->Z+v.Z);
}
Vec3 Vec3::operator-(const Vec3 &v)
{
	return Vec3(this->X - v.X, this->Y - v.Y, this->Z - v.Z);
}
Vec3 Vec3::operator*(const Vec3 &v)
{
	return Vec3(this->X * v.X, this->Y * v.Y, this->Z * v.Z);
}
Vec3 Vec3::operator/(const Vec3 &v)
{
	return Vec3(this->X / v.X, this->Y / v.Y, this->Z / v.Z);
}

Vec3 Vec3::operator+=(const Vec3 &v)
{
	*this = *this + v;
	return *this;
}
Vec3 Vec3::operator-=(const Vec3 &v)
{
	*this = *this - v;
	return *this;
}
Vec3 Vec3::operator*=(const Vec3 &v)
{
	*this = *this * v;
	return *this;
}
Vec3 Vec3::operator/=(const Vec3 &v)
{
	*this = *this / v;
	return *this;
}

Vec3 Vec3::operator*(float v)
{
	return Vec3(this->X * v, this->Y * v, this->Z * v);
}
Vec3 Vec3::operator/(float v)
{
	return Vec3(this->X / v, this->Y / v, this->Z / v);
}

Vec3 Vec3::operator*(int v)
{
	return Vec3(this->X * v, this->Y * v, this->Z * v);
}
Vec3 Vec3::operator/(int v)
{
	return Vec3(this->X / v, this->Y / v, this->Z / v);
}

Vec3 Vec3::operator*=(float v)
{
	*this = *this * v;
	return *this;
}
Vec3 Vec3::operator/=(float v)
{
	*this = *this / v;
	return *this;
}

Vec3 Vec3::operator*=(int v)
{
	*this = *this * v;
	return *this;
}
Vec3 Vec3::operator/=(int v)
{
	*this = *this / v;
	return *this;
}

bool Vec3::operator==(const Vec3 &v)
{
	return this->X == v.X && this->Y == v.Y && this->Z == v.Z;
}

bool Vec3::operator!=(const Vec3 &v)
{
	return !(*this == v);
}

bool Vec3::operator==(float v)
{
	return this->X == v && this->Y == v && this->Z == v;
}
bool Vec3::operator!=(float v)
{
	return !(*this == v);
}
bool Vec3::operator==(int v)
{
	return this->X == v && this->Y == v && this->Z == v;
}
bool Vec3::operator!=(int v)
{
	return !(*this == v);
}

Vec3 Vec3::Max()
{
	return Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
}
Vec3 Vec3::Min()
{
	return Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}
Vec3 Vec3::Zero()
{
	return Vec3();
}
Vec3 Vec3::Max(const Vec3 &v1, const Vec3 &v2)
{
	return Vec3(std::max(v1.X, v2.X), std::max(v1.Y, v2.Y), std::max(v1.Z, v2.Z));
}
Vec3 Vec3::Min(const Vec3 &v1, const Vec3 &v2)
{
	return Vec3(std::min(v1.X, v2.X), std::min(v1.Y, v2.Y), std::min(v1.Z, v2.Z));
}