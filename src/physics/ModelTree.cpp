#include "ModelTree.h"

ModelTreeNode::ModelTreeNode(){}
ModelTreeLeaf::ModelTreeLeaf() : ModelTreeLeaf::ModelTreeNode() {}
void ModelTreeNode::Add(Triangle* t, int layers){}

ModelTreeParent::ModelTreeParent(Vec3 min, Vec3 max) : ModelTreeParent::ModelTreeNode()
{
	this->min = Vec3::Min(min, max);
	this->max = Vec3::Max(max, min);
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
		left = new ModelTreeParent(min+size*normal/2, max);
		right = new ModelTreeParent(min, min + size*normal / 2);
	}
	else
	{
		left = new ModelTreeLeaf();
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
	if(addright)
		right->Add(t, layers - 1);
	if (addleft)
		left->Add(t, layers - 1);
}

bool ModelTreeParent::InfrontOfPlane(Vec3 point, Vec3 &center, Vec3 &normal)
{
	point -= center;
	return point.Dot(normal);
}

void ModelTreeLeaf::Add(Triangle* t, int layers)
{
	triangles.push_back(t);
}