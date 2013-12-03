#include "CollisionInstance.h"

CollisionInstance::CollisionInstance(CollisionModel* Model, Vec3 Position)
{
	this->Model = Model;
	this->Position = Position;
}

float CollisionInstance::Test(Ray r)
{
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
	t.P1 -= Position;
	t.P2 -= Position;
	t.P3 -= Position;

	if (!Model->GetTree())
	{
		Box tBounds = t.GetBox();
		if (tBounds.Test(Model->GetBox()))
		{
			for (int i = 0; i < Model->Triangles(); i++)
			{
				if (Model->GetTriangle(i)->Test(t))
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
	if (ci->Model->GetBox().Test(Model->GetBox()))
	{
		if (!Model->GetTree() || !ci->Model->GetTree())
		{
			for (int i = 0; i < Model->Triangles(); i++)
			{
				if (Test(*Model->GetTriangle(i)))
					return true;
			}
			return false;
		}
		else
			return Model->GetTree()->Test(ci->Model->GetTree());
	}
	return false;
}