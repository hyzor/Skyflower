#include "CollisionModel.h"

// Must be included last!
#include "shared/debug.h"

CollisionModel::CollisionModel()
{
	tree = nullptr;
}
CollisionModel::~CollisionModel()
{
	if (tree)
		delete tree;
}

void CollisionModel::Load(std::string file)
{
	std::stringstream ssTree;
	ssTree << file << ".tree2";
	if (!LoadTree(ssTree.str()))
	{
		//load obj
		std::stringstream ss;
		ss << file << ".obj";
		std::vector<Vec3> positions;
		std::ifstream infile(ss.str());
		std::string line;
		Vec3 center;
		while (std::getline(infile, line))
		{
			std::vector<std::string> elements = split(line);
			if (elements.size() > 0)
			{
				if (elements.at(0) == "v")
				{
					positions.push_back(Vec3((float)atof(elements.at(1).c_str()), (float)atof(elements.at(2).c_str()), (float)atof(elements.at(3).c_str())));
					center += positions[positions.size() - 1];
				}
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
		center /= (int)positions.size();

		//calculate bounds and sphere
		Vec3 vMin = Vec3::Max();
		Vec3 vMax = Vec3::Min();
		float radius = 0;
		for (unsigned int i = 0; i < positions.size(); i++)
		{
			//calculate box
			vMin = Vec3::Min(vMin, positions[i]);
			vMax = Vec3::Max(vMax, positions[i]);


			//calculate sphere
			Vec3 p = positions[i];
			p.Y = center.Y; // skipp height
			float len = (p - center).Length();
			if (len > radius)
				radius = len;
		}
		bounds = Box(vMin, vMax - vMin);
		sphere = Sphere(center, radius);

		

		//create tree
		tree = new ModelTreeParent(vMin, vMax);
		for (unsigned int i = 0; i < triangles.size(); i++)
			tree->Add(&triangles[i], 6);

		SaveTree(ssTree.str());
	}
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
	return (int)triangles.size();
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

void CollisionModel::SaveTree(std::string file)
{
	std::fstream outfile(file, std::ios::out | std::ios::binary);
	
	outfile.write((char*)&bounds, sizeof(Box));
	outfile.write((char*)&sphere, sizeof(Sphere));

	size_t tCount = triangles.size();
	outfile.write((char*)&tCount, 4);

	for (size_t i = 0; i < tCount; i++)
		outfile.write((char*)&triangles[i], 12*3); //12 size of vec3, 3 vec3 in triangle

	tree->Write(&outfile, &triangles[0]);
}

bool CollisionModel::LoadTree(std::string file)
{
	triangles = std::vector<Triangle>();

	std::ifstream infile(file, std::ios::in | std::ios::binary);

	if (infile)
	{
		infile.read((char*)&bounds, sizeof(Box));
		infile.read((char*)&sphere, sizeof(Sphere));

		int tCount;
		infile.read((char*)&tCount, 4);

		for (int i = 0; i < tCount; i++)
		{
			Triangle t;
			infile.read((char*)&t, sizeof(Triangle));
			triangles.push_back(t);
		}
		if (!tree)
			tree = new ModelTreeParent();
		tree->Read(&infile, &triangles[0]);
		return true;
	}
	else
		return false;
}