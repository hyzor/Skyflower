#include "ModelTree.h"

ModelTreeNode::ModelTreeNode(){}
ModelTreeNode::~ModelTreeNode(){}
ModelTreeLeaf::ModelTreeLeaf() : ModelTreeLeaf::ModelTreeNode() {}
ModelTreeLeaf::~ModelTreeLeaf() {}
void ModelTreeNode::Add(Triangle* t, int layers){}
float ModelTreeNode::Test(Ray &r) { return 0; }

ModelTreeParent::ModelTreeParent(Vec3 min, Vec3 max) : ModelTreeParent::ModelTreeNode()
{
	this->min = Vec3::Min(min, max);
	this->max = Vec3::Max(max, min);

	left = nullptr;
	right = nullptr;
}

ModelTreeParent::~ModelTreeParent()
{
	if (!left)
		delete left;
	if (!right)
		delete right;
}


void ModelTreeParent::Add(Triangle* t, int layers)
{
	//create plane in center
	Vec3 center = (min + max) / 2;
	Vec3 normal = Vec3(0, 0, 1);
	Vec3 size = max - min;
	if (size.Y > size.Z)
		normal = Vec3(0, 1, 0);
	if (size.X > size.Y && size.X > size.Z)
		normal = Vec3(1, 0, 0);

	//create nodes
	if (layers > 0)
	{
		if(!left)
			left = new ModelTreeParent(min + size * normal / 2, max);
		if(!right)
			right = new ModelTreeParent(min, min + size*normal / 2);
	}
	else
	{
		if(!left)
			left = new ModelTreeLeaf();
		if(!right)
			right = new ModelTreeLeaf();
	}

	//check side to add
	bool addright = false;
	bool addleft = false;
	if (InfrontOfPlane(t->P1, center, normal))
		addright = true;
	else
		addleft = true;
	if (InfrontOfPlane(t->P2, center, normal))
		addright = true;
	else
		addleft = true;
	if (InfrontOfPlane(t->P3, center, normal))
		addright = true;
	else
		addleft = true;

	//add to side
	if (addright)
		right->Add(t, layers - 1);
	if (addleft)
		left->Add(t, layers - 1);
}

bool ModelTreeParent::InfrontOfPlane(Vec3 point, Vec3 &center, Vec3 &normal)
{
	point -= center;
	return point.Dot(normal) > 0;
}

void ModelTreeLeaf::Add(Triangle* t, int layers)
{
	triangles.push_back(t);
}

float ModelTreeParent::Test(Ray &r)
{
	//create plane in center
	Vec3 center = (min + max) / 2;
	Vec3 normal = Vec3(0, 0, 1);
	Vec3 size = max - min;
	if (size.Y > size.Z)
		normal = Vec3(0, 1, 0);
	if (size.X > size.Y && size.X > size.Z)
		normal = Vec3(1, 0, 0);

	//check side to test
	bool testright = false;
	bool testleft = false;
	if (InfrontOfPlane(r.Pos, center, normal))
		testright = true;
	else
		testleft = true;
	if (InfrontOfPlane(r.Pos+r.Dir, center, normal))
		testright = true;
	else
		testleft = true;

	//test side
	float hit = 0;
	if (testright)
		hit = right->Test(r);
	if (testleft)
		hit = left->Test(r);
	return hit;
}

float ModelTreeLeaf::Test(Ray &r)
{
	Box rBounds = r.GetBox();
	float hit = 0;
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		float t = triangles[i]->Test(r);
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