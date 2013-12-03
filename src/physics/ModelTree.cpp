#include "ModelTree.h"

ModelTreeNode::ModelTreeNode(){}
ModelTreeNode::~ModelTreeNode(){}
ModelTreeLeaf::ModelTreeLeaf() : ModelTreeLeaf::ModelTreeNode() {}
ModelTreeLeaf::~ModelTreeLeaf() {}
void ModelTreeNode::Add(Triangle* t, int layers){}
float ModelTreeNode::Test(Ray &r) { return 0; }
bool ModelTreeNode::Test(Triangle &r) { return false; }
bool ModelTreeNode::Test(ModelTreeNode* mtn) { return false; }

ModelTreeParent::ModelTreeParent() : ModelTreeParent::ModelTreeNode()
{
	left = nullptr;
	right = nullptr;
}
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

bool ModelTreeParent::Test(Triangle &t)
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
	if (InfrontOfPlane(t.P1, center, normal))
		testright = true;
	else
		testleft = true;
	if (InfrontOfPlane(t.P2, center, normal))
		testright = true;
	else
		testleft = true;
	if (InfrontOfPlane(t.P3, center, normal))
		testright = true;
	else
		testleft = true;

	//test side
	bool hit = false;
	if (testright)
		hit = right->Test(t);
	if (!hit && testleft)
		hit = left->Test(t);
	return hit;
}

bool ModelTreeParent::Test(ModelTreeNode *mtn)
{
	ModelTreeParent* mtp = (ModelTreeParent*)mtn;

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
	if (InfrontOfPlane(mtp->min, center, normal))
		testright = true;
	else
		testleft = true;
	if (InfrontOfPlane(mtp->max, center, normal))
		testright = true;
	else
		testleft = true;

	//test side
	bool hit = false;
	if (testright)
		hit = right->Test(mtp);
	if (!hit && testleft)
		hit = left->Test(mtp);
	return hit;
}

float ModelTreeLeaf::Test(Ray &r)
{
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

bool ModelTreeLeaf::Test(Triangle &t)
{
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		if (triangles[i]->Test(t))
			return true;
	}
	return false;
}

bool ModelTreeLeaf::Test(ModelTreeNode *mtp)
{
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		if (mtp->Test(*triangles[i]))
			return true;
	}
	return false;
}


int ModelTreeNode::GetType()
{
	return 0;
}
int ModelTreeParent::GetType()
{
	return 1;
}
int ModelTreeLeaf::GetType()
{
	return 2;
}

void ModelTreeNode::Write(std::fstream* outfile, Triangle* first){}
void ModelTreeParent::Write(std::fstream* outfile, Triangle* first)
{
	int type = GetType();
	outfile->write((char*)&type, 4);
	outfile->write((char*)&min, 12);
	outfile->write((char*)&max, 12);

	int create; //2 left parent, 3 left leaf, 4 right parent, 5 right leaf
	if (left->GetType() == 1)
		create = 2;
	else if (left->GetType() == 2)
		create = 3;
	outfile->write((char*)&create, 4);
	left->Write(outfile, first);

	if (right->GetType() == 1)
		create = 4;
	else if (right->GetType() == 2)
		create = 5;
	outfile->write((char*)&create, 4);
	right->Write(outfile, first);
}
void ModelTreeLeaf::Write(std::fstream* outfile, Triangle* first)
{
	int type = GetType();
	outfile->write((char*)&type, 4);

	int cTriangles = triangles.size();
	outfile->write((char*)&cTriangles, 4);

	for (int i = 0; i < cTriangles; i++)
	{
		int relativeAddress = (int)(triangles[i] - first);
		outfile->write((char*)&relativeAddress, 4);
	}
}

void ModelTreeNode::Read(std::ifstream* infile, Triangle* first){}
void ModelTreeParent::Read(std::ifstream* infile, Triangle* first)
{
	int type;
	infile->read((char*)&type, 4);
	infile->read((char*)&min, 12);
	infile->read((char*)&max, 12);

	int create; //2 left parent, 3 left leaf, 4 right parent, 5 right leaf
	infile->read((char*)&create, 4);
	if (create == 2)
		left = new ModelTreeParent();
	else if (create == 3)
		left = new ModelTreeLeaf();
	left->Read(infile, first);

	infile->read((char*)&create, 4);
	if (create == 4)
		right = new ModelTreeParent();
	else if (create == 5)
		right = new ModelTreeLeaf();
	right->Read(infile, first);
}
void ModelTreeLeaf::Read(std::ifstream* infile, Triangle* first)
{
	int type;
	infile->read((char*)&type, 4);

	int cTriangles;
	infile->read((char*)&cTriangles, 4);

	for (int i = 0; i < cTriangles; i++)
	{
		int relativeAddress;
		infile->read((char*)&relativeAddress, 4);
		triangles.push_back(first+relativeAddress);
	}
}