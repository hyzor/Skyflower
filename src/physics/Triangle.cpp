#include "Triangle.h"


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
	Vec3 pvec = r.Dir.Cross(edge2);
	float det = edge1.Dot(pvec);
	if (det == 0) 
		return false;
	float invDet = 1 / det;
	Vec3 tvec = r.Pos - t.P1;
	float u = tvec.Dot(pvec) * invDet;
	if (u < 0 || u > 1) 
		return false;
	Vec3 qvec = tvec.Cross(edge1);
	float v = r.Dir.Dot(qvec) * invDet;
	if (v < 0 || u + v > 1)
		return false;
	float t2 = edge2.Dot(qvec) * invDet;


	return t2;

	/*




	epsilon = 0.00001;

	e1 = p1 - p0;
	e2 = p2 - p0;
	q = cross(d, e2);
	a = dot(e1, q); % determinant of the matrix M

	if (a>-epsilon && a<epsilon)
	% the vector is parallel to the plane(the intersection is at infinity)
	[flag, u, v, t] = deal(0, 0, 0, 0);
	return;
	end;

	f = 1 / a;
	s = o - p0;
	u = f*dot(s, q);

	if (u<0.0)
	% the intersection is outside of the triangle
	[flag, u, v, t] = deal(0, 0, 0, 0);
	return;
	end;

	r = cross(s, e1);
	v = f*dot(d, r);

	if (v<0.0 || u + v>1.0)
	% the intersection is outside of the triangle
	[flag, u, v, t] = deal(0, 0, 0, 0);
	return;
	end;

	t = f*dot(e2, r); % verified!
	flag = 1;
	return;






	*/




	/*Vec3 v0v1 = t.P2 - t.P1;
	Vec3 v0v2 = t.P3 - t.P1;
	Vec3 N = v0v1.Cross(v0v2);

	float nDotRay = N.Dot(r.Dir);
	if (nDotRay == 0) 
		return false; // ray parallel to triangle 
	float d = N.Dot(t.P1);
	float t = -(N.Dot(r.Pos) + d) / nDotRay;

	// inside-out test
	Vec3 Phit = r(t);

	// inside-out test edge0
	Vec3 v0p = Phit - t.P1;
	float v = dot(N, cross(v0v1, v0p));
	if (v < 0) return false; // P outside triangle

	// inside-out test edge1
	Vec3 v1p = Phit - v1;
	Vec3 v1v2 = v2 - v1;
	float w = dot(N, cross(v1v2, v1p));
	if (w < 0) return false; // P outside triangle

	// inside-out test edge2
	Vec3 v2p = Phit - v2;
	Vec3 v2v0 = v0 - v2;
	float u = dot(N, cross(v2v0, v2p));
	if (u < 0) return false; // P outside triangle

	//isectData.t = t;

	return true;*/
}

