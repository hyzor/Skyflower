#include "Vec3.h"


Vec3::Vec3()
{
	this->X = 0;
	this->Y = 0;
	this->Z = 0;
}

Vec3::Vec3(float X, float Y)
{
	this->X = X;
	this->Y = Y;
	this->Z = 0;
}

Vec3::Vec3(float X, float Y, float Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

Vec3::Vec3(int X, int Y)
{
	this->X = X;
	this->Y = Y;
	this->Z = 0;
}

Vec3::Vec3(int X, int Y, int Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

float Vec3::Length()
{
	return (float)sqrt(this->X*this->X + this->Y*this->Y + this->Z*this->Z);
}
void Vec3::Normalize()
{
	*this /= this->Length();
}

float Vec3::Dot(Vec3 &v)
{
	return Dot(*this, v);
}
Vec3 Vec3::Cross(Vec3 &v)
{
	return Cross(*this, v);
}

float Vec3::Dot(Vec3 &v1, Vec3 &v2)
{
	return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
}

Vec3 Vec3::Cross(Vec3 &v1, Vec3 &v2)
{
	Vec3 res;
	res.X = v1.Y*v2.Z - v1.Z*v2.Y;
	res.Y = v1.Z*v2.X - v1.X*v2.Z;
	res.Z = v1.X*v2.Y - v1.Y*v2.X;
	return res;
}

Vec3 Vec3::operator+(Vec3 &v)
{
	return Vec3(this->X+v.X, this->Y+v.Y, this->Z+v.Z);
}
Vec3 Vec3::operator-(Vec3 &v)
{
	return Vec3(this->X - v.X, this->Y - v.Y, this->Z - v.Z);
}
Vec3 Vec3::operator*(Vec3 &v)
{
	return Vec3(this->X * v.X, this->Y * v.Y, this->Z * v.Z);
}
Vec3 Vec3::operator/(Vec3 &v)
{
	return Vec3(this->X / v.X, this->Y / v.Y, this->Z / v.Z);
}

Vec3 Vec3::operator+=(Vec3 &v)
{
	*this = *this + v;
	return *this;
}
Vec3 Vec3::operator-=(Vec3 &v)
{
	*this = *this - v;
	return *this;
}
Vec3 Vec3::operator*=(Vec3 &v)
{
	*this = *this * v;
	return *this;
}
Vec3 Vec3::operator/=(Vec3 &v)
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

bool Vec3::operator==(Vec3 &v)
{
	return this->X == v.X && this->Y == v.Y && this->Z == v.Z;
}

bool Vec3::operator!=(Vec3 &v)
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
Vec3 Vec3::Max(Vec3 v1, Vec3 v2)
{
	Vec3 res = v1;
	if (v2.X > res.X)
		res.X = v2.X;
	if (v2.Y > res.Y)
		res.Y = v2.Y;
	if (v2.Z > res.Z)
		res.Z = v2.Z;
	return res;
}
Vec3 Vec3::Min(Vec3 v1, Vec3 v2)
{
	Vec3 res = v1;
	if (v2.X < res.X)
		res.X = v2.X;
	if (v2.Y < res.Y)
		res.Y = v2.Y;
	if (v2.Z < res.Z)
		res.Z = v2.Z;
	return res;
}