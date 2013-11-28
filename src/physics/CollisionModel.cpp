#include "CollisionModel.h"

CollisionModel::CollisionModel()
{
	tree = nullptr;
}
CollisionModel::~CollisionModel()
{
	if (!tree)
		delete tree;
}

void CollisionModel::LoadObj(std::string file)
{
	//load obj
	std::vector<Vec3> positions;
	std::ifstream infile(file);
	std::string line;
	while (std::getline(infile, line))
	{
		std::vector<std::string> elements = split(line);
		if (elements.size() > 0)
		{
			if (elements.at(0) == "v")
				positions.push_back(Vec3((float)atof(elements.at(1).c_str()), (float)atof(elements.at(2).c_str()), (float)atof(elements.at(3).c_str())));
			else if (elements.at(0) == "f" && elements.size() > 6)
			{
				Triangle t;
				t.P1 = positions.at(atoi(elements.at(1).c_str()) - 1);
				t.P2 = positions.at(atoi(elements.at(4).c_str()) - 1);
				t.P3 = positions.at(atoi(elements.at(7).c_str()) - 1);
				triangles.push_back(t);
			}
		}
	}
	infile.close();

	//calculate bounds
	Vec3 vMin = Vec3::Max();
	Vec3 vMax = Vec3::Min();
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		Vec3 pos = positions[i];

		vMin = Vec3::Min(vMin, pos);
		vMax = Vec3::Max(vMax, pos);
	}
	bounds = Box(vMin, vMax - vMin);

	//create tree
	tree = new ModelTreeParent(vMin, vMax);
	for (unsigned int i = 0; i < triangles.size(); i++)
		tree->Add(&triangles[i], 3);
}

Box CollisionModel::GetBox()
{
	return bounds;
}

Triangle* CollisionModel::GetTriangle(int index)
{
	return &triangles[index];
}

int CollisionModel::Triangles()
{
	return triangles.size();
}

ModelTreeParent* CollisionModel::GetTree()
{
	return tree;
}


std::vector<std::string> CollisionModel::split(std::string line)
{
	std::vector<std::string> elements;
	std::string element = "";
	for (int i = 0; i < (int)line.length(); i++)
	{
		if (line[i] != ' ' && line[i] != '/')
			element += line[i];
		else
		{
			elements.push_back(element);
			element = "";
		}
	}
	elements.push_back(element);

	return elements;
}