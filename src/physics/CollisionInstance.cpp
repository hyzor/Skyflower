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
				int t = Model->GetTriangle(i)->Test(r);
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