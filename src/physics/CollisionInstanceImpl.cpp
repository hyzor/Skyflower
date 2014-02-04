#include "CollisionInstanceImpl.h"

// Must be included last!
#include "shared/debug.h"

CollisionInstanceImpl::CollisionInstanceImpl(CollisionModel* Model, Vec3 Position)
{
	this->Model = Model;
	this->Position = Position;

	this->scaleInv = Vec3(1, 1, 1);

	this->rot1Inv = Vec3(1, 0, 0);
	this->rot2Inv = Vec3(0, 1, 0);
	this->rot3Inv = Vec3(0, 0, 1);

	this->_isActive = true;
}

CollisionInstanceImpl::~CollisionInstanceImpl()
{
}

float CollisionInstanceImpl::Test(Ray r)
{
	if (this->_isActive)
	{
		//transform ray instead of all triangles
		Vec3 rPos = r.GetPos();
		Vec3 rDir = r.GetDir();

		rPos -= Position; 

		Vec3 p = rPos;
		p.X = rPos.Dot(rot1Inv);
		p.Y = rPos.Dot(rot2Inv);
		p.Z = rPos.Dot(rot3Inv);
		rPos = p;

		Vec3 d = rDir;
		d.X = rDir.Dot(rot1Inv);
		d.Y = rDir.Dot(rot2Inv);
		d.Z = rDir.Dot(rot3Inv);
		rDir = d;

		rPos *= Vec3(1, 1, -1);
		rDir *= Vec3(1, 1, -1);

		rPos *= scaleInv;
		rDir *= scaleInv;

		r.Set(rPos, rDir);

		
		//check bounding box
		Box rBounds = r.GetBox();
		if (rBounds.Test(Model->GetBox()))
		{
			//triangle test
			if (!Model->GetTree())
			{
				float hit = 0;
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
				return hit;
			}
			else
				return Model->GetTree()->Test(r);
		}		
	}
	return 0;
}

bool CollisionInstanceImpl::Test(Triangle t)
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

bool CollisionInstanceImpl::Test(CollisionInstance* ci)
{
	CollisionInstanceImpl *cii = (CollisionInstanceImpl *)ci;
	CollisionModel *ciModel = cii->GetModel();
	Vec3 ciPosition = cii->GetPosition();

	Box b1 = Model->GetBox();
	Box b2 = ciModel->GetBox();
	b2.Position += ciPosition*Vec3(1, 1, -1) - Position*Vec3(1, 1, -1);
	if (b1.Test(b2))
	{
		//if (!ci->Model->GetTree())
		{
			for (int i = 0; i < ciModel->Triangles(); i++)
			{
				Triangle* t = ciModel->GetTriangle(i);
				Triangle t2 = *t;
				t2.P1 += ciPosition*Vec3(1, 1, -1);
				t2.P2 += ciPosition*Vec3(1, 1, -1);
				t2.P3 += ciPosition*Vec3(1, 1, -1);
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

void CollisionInstanceImpl::SetScale(Vec3 scale)
{
	scaleInv = Vec3(1, 1, 1) / scale;
}

void CollisionInstanceImpl::SetRotation(Vec3 rot)
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

void CollisionInstanceImpl::SetPosition(Vec3 position)
{
	this->Position = position;
}

CollisionModel *CollisionInstanceImpl::GetModel()
{
	return Model;
}

Box CollisionInstanceImpl::GetBox()
{
	Box bounds = Model->GetBox();
	bounds.Position += this->Position;

	return bounds;
}

Vec3 CollisionInstanceImpl::GetPosition()
{
	return this->Position;
}

bool CollisionInstanceImpl::isActive()
{
	return this->_isActive;
}

void CollisionInstanceImpl::setIsActive(bool status)
{
	this->_isActive = status;
}