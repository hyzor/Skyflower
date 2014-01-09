#include "CollisionInstance.h"

CollisionInstance::CollisionInstance(CollisionModel* Model, Vec3 Position)
{
	this->Model = Model;
	this->Position = Position;

	this->scaleInv = Vec3(1, 1, 1);

	this->rot1Inv = Vec3(1, 0, 0);
	this->rot2Inv = Vec3(0, 1, 0);
	this->rot3Inv = Vec3(0, 0, 1);
}

float CollisionInstance::Test(Ray r)
{
	Vec3 p = r.Pos;
	p.X = r.Pos.Dot(rot1Inv);
	p.Y = r.Pos.Dot(rot2Inv);
	p.Z = r.Pos.Dot(rot3Inv);
	r.Pos = p;

	Vec3 d = r.Dir;
	d.X = r.Dir.Dot(rot1Inv);
	d.Y = r.Dir.Dot(rot2Inv);
	d.Z = r.Dir.Dot(rot3Inv);
	r.Dir = d;



	r.Pos *= scaleInv;
	r.Dir *= scaleInv;
	r.Pos -= Position; //translate ray instead of all triangles
	if (!Model->GetTree())
	{
		Box rBounds = r.GetBox();
		float hit = 0;
		if (rBounds.Test(Model->GetBox()))
		{
			for (int i = 0; i < Model->Triangles(); i++)
			{
				float t = Model->GetTriangle(i)->Test(r);
				if (t > 0 && t < 1)
				{
					if (hit == 0)
						hit = t;
					else if (t < hit)
						hit = t;
				}
			}
		}
		return hit;
	}
	else
		return Model->GetTree()->Test(r);
}

bool CollisionInstance::Test(Triangle t)
{
	t.P1 -= Position*Vec3(1, 1, -1);
	t.P2 -= Position*Vec3(1, 1, -1);
	t.P3 -= Position*Vec3(1, 1, -1);
	if (!Model->GetTree())
	{
		Box tBounds = t.GetBox();
		Box b1 = Model->GetBox();
		if (tBounds.Test(b1))
		{

			for (int i = 0; i < Model->Triangles(); i++)
			{
				Triangle* t2 = Model->GetTriangle(i);
				if (t2->Test(t) != 0)
					return true;
			}
		}
		return false;
	}
	else
		return Model->GetTree()->Test(t);
}

bool CollisionInstance::Test(CollisionInstance* ci)
{
	Box b1 = Model->GetBox();
	Box b2 = ci->Model->GetBox();
	b2.Position += ci->Position*Vec3(1, 1, -1) - Position*Vec3(1, 1, -1);
	if (b1.Test(b2))
	{
		//if (!ci->Model->GetTree())
		{
			for (int i = 0; i < ci->Model->Triangles(); i++)
			{
				Triangle* t = ci->Model->GetTriangle(i);
				Triangle t2 = *t;
				t2.P1 += ci->Position*Vec3(1, 1, -1);
				t2.P2 += ci->Position*Vec3(1, 1, -1);
				t2.P3 += ci->Position*Vec3(1, 1, -1);
				if (Test(t2))
					return true;
			}
			return false;
		}
		/*else
		{
			std::vector<std::vector<Triangle*>*> ts = ci->Model->GetTree()->GetTriangles(b2);
			for (int i = 0; i < ts.size(); i++)
			{
				for (int j = 0; j < ts[i]->size(); j++)
				{
					Triangle* t = ts[i]->at(j);
					Triangle t2 = *t;
					t2.P1 += ci->Position*Vec3(1, 1, -1);
					t2.P2 += ci->Position*Vec3(1, 1, -1);
					t2.P3 += ci->Position*Vec3(1, 1, -1);
					if (Test(t2))
						return true;
				}
			}
		}*/
	}
	return false;
}

void CollisionInstance::SetScale(Vec3 scale)
{
	scaleInv = Vec3(1, 1, 1) / scale;
}

void CollisionInstance::SetRotation(Vec3 rot)
{
	//x rot
	Vec3 r1 = Vec3(1, 0, 0);
	Vec3 r2 = Vec3(0, cosf(rot.X), sinf(rot.X));
	Vec3 r3 = Vec3(0, -r2.Z, r2.Y);

	//y rot
	Vec3 c1 = Vec3(cosf(rot.Y), 0, sinf(rot.Y));
	Vec3 c2 = Vec3(0, 1, 0);
	Vec3 c3 = Vec3(-c1.Z, 0, c1.X);

	//x * y rot
	r1 = Vec3(r1.Dot(c1), r1.Dot(c2), r1.Dot(c3));
	r2 = Vec3(r2.Dot(c1), r2.Dot(c2), r2.Dot(c3));
	r3 = Vec3(r3.Dot(c1), r3.Dot(c2), r3.Dot(c3));

	//z rot
	c1 = Vec3(cosf(rot.Z), -sinf(rot.Z), 0);
	c2 = Vec3(-c1.Y, c1.X, 0);
	c3 = Vec3(0, 0, 1);

	//x*y*z rot
	r1 = Vec3(r1.Dot(c1), r1.Dot(c2), r1.Dot(c3));
	r2 = Vec3(r2.Dot(c1), r2.Dot(c2), r2.Dot(c3));
	r3 = Vec3(r3.Dot(c1), r3.Dot(c2), r3.Dot(c3));

	rot1Inv = r1;
	rot2Inv = r2;
	rot3Inv = r3;
}