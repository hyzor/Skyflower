#include "Triangle.h"

// Must be included last!
#include "shared/debug.h"

Triangle::Triangle()
{
}

Triangle::Triangle(Vec3 P1, Vec3 P2, Vec3 P3)
{
	this->P1 = P1;
	this->P2 = P2;
	this->P3 = P3;
}

float Triangle::Test(Ray &r)
{
	return Test(*this, r);
}
float Triangle::Test(Triangle &t, Ray &r)
{
	Vec3 edge1 = t.P2 - t.P1;
	Vec3 edge2 = t.P3 - t.P1;
	Vec3 pvec = r.GetDir().Cross(edge2);
	float det = edge1.Dot(pvec);
	if (det == 0) 
		return 0;
	float invDet = 1 / det;
	Vec3 tvec = r.GetPos();
	tvec -= t.P1;
	float u = tvec.Dot(pvec) * invDet;
	if (u < 0 || u > 1) 
		return 0;
	Vec3 qvec = tvec.Cross(edge1);
	float v = r.GetDir().Dot(qvec) * invDet;
	if (v < 0 || u + v > 1)
		return 0;
	float t2 = edge2.Dot(qvec) * invDet;


	return t2;
}

bool Triangle::Test(Triangle &t)
{
	return Test(*this, t);
}
bool Triangle::Test(Triangle &t1, Triangle &t2)
{
	float val;
	
	val = Test(t2, Ray(t1.P1, t1.P2 - t1.P1));
	if (val < 1 && val > 0)
		return true;

	val = Test(t2, Ray(t1.P1, t1.P3 - t1.P1));
	if (val < 1 && val > 0)
		return true;

	val = Test(t2, Ray(t1.P2, t1.P3 - t1.P2));
	if (val < 1 && val > 0)
		return true;


	val = Test(t1, Ray(t2.P1, t2.P2 - t2.P1));
	if (val < 1 && val > 0)
		return true;

	val = Test(t1, Ray(t2.P1, t2.P3 - t2.P1));
	if (val < 1 && val > 0)
		return true;

	val = Test(t1, Ray(t2.P2, t2.P3 - t2.P2));
	if (val < 1 && val > 0)
		return true;


	return false;
}

Box Triangle::GetBox()
{
	Vec3 min = Vec3::Min(Vec3::Min(this->P1, this->P2), this->P3);
	Vec3 max = Vec3::Max(Vec3::Max(this->P1, this->P2), this->P3);
	Box bounds = Box(min, max - min);
	return bounds;
}

Vec3 Triangle::GetNormal()
{
	return (P2 - P1).Cross(P3 - P1).Normalize();
}