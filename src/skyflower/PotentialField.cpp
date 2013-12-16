#include "potentialfield.h"


PotentialField::PotentialField()
{
	srand((unsigned int)time(0));
}

PotentialField::~PotentialField()
{
	for (unsigned int i = 0; i < fields.size(); i++)
		delete fields[i];
}

Field* PotentialField::CreateField(float weight, float size, Vec3 pos)
{
	Field* f = new Field(weight, size, pos, Box(Vec3(), Vec3()));
	fields.push_back(f);
	return f;
}

Field* PotentialField::CreateField(std::string file, Vec3 pos)
{
	//load obj
	std::stringstream ss;
	ss << file << ".obj";
	std::ifstream infile(ss.str());
	std::string line;

	int count = 0;
	Vec3 center;
	std::vector<Vec3> positions;
	while (std::getline(infile, line))
	{
		std::vector<std::string> elements = split(line);
		if (elements.size() > 0)
		{
			if (elements.at(0) == "v")
			{
				Vec3 v = Vec3((float)atof(elements.at(1).c_str()), (float)atof(elements.at(2).c_str()), (float)atof(elements.at(3).c_str()));
				center += v;
				count++;
				positions.push_back(v);
			}
		}
	}
	infile.close();

	Vec3 vMin = Vec3::Max();
	Vec3 vMax = Vec3::Min();
	center /= count;
	float radius = 9999999999;
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		//calculate sphere
		float len = (positions[i] - center).Length();
		if (len < radius)
			radius = len;

		//calculate bounds
		vMin = Vec3::Min(vMin, positions[i]);
		vMax = Vec3::Max(vMax, positions[i]);
	}

	//get safe move area
	vMin.Y = vMax.Y - 1;
	vMax.Y += 5;
	Box bounds = Box(vMin, vMax - vMin);

	Field* f = new Field(radius, radius*1.5f, pos + center, bounds);
	fields.push_back(f);
	return f;
}

void PotentialField::DeleteField(Field* f)
{
	for (unsigned int i = 0; i < fields.size(); i++)
	{
		if (f == fields[i])
		{
			fields.erase(fields.begin() + i);
			break;
		}
	}
	delete f;
}

float PotentialField::GetWeight(Vec3 pos)
{
	float weight = 0;
	for (unsigned int i = 0; i < fields.size(); i++)
		weight += fields[i]->GetWeight(pos);
	return weight;
}

Vec3 PotentialField::GetDir(Vec3 pos)
{
	Vec3 dir;
	float minWeight = 9999999;
	for (int i = 0; i < 15; i++)
	{
		Vec3 ranDir = Vec3((rand() % 200) / 100.0f - 1, (rand() % 200) / 100.0f - 1, 0.0f).Normalize();
		
		float weight = GetWeight(pos + ranDir);

		if (weight < minWeight)
		{
			//check if safe to walk
			bool safe = false;
			for (unsigned int i = 0; i < fields.size(); i++)
			{
				if (fields[i]->GetSafe().Test(pos + ranDir))
				{
					safe = true;
					break;
				}
			}

			if (safe)
			{
				minWeight = weight;
				dir = ranDir;
			}
		}
	}

	return dir;
}

std::vector<std::string> PotentialField::split(std::string line)
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