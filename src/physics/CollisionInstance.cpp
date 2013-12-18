#include "CollisionInstance.h"

CollisionInstance::CollisionInstance(CollisionModel* Model, Vec3 Position)
{
	this->Model = Model;
	this->Position = Position;
}

float CollisionInstance::Test(Ray r)
{
	r.Pos -= Position; //translate ray instead of all triangles
	//if (!Model->GetTree())
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
	//else
		//return Model->GetTree()->Test(r);
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