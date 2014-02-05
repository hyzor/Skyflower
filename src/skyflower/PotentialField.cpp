#include "potentialfield.h"

// Must be included last!
#include "shared/debug.h"

PotentialField::PotentialField()
{
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

Field* PotentialField::CreateField(CollisionInstance* collinst, Vec3 pos)
{
	/*//load obj
	std::stringstream ss;
	ss << "../../content/" << file << ".obj";
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
				Vec3 v = Vec3((float)atof(elements.at(1).c_str()), (float)atof(elements.at(2).c_str()), (float)atof(elements.at(3).c_str()))*scale;
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
	float radius = 0;
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		//calculate sphere
		Vec3 p = positions[i];
		p.Y = center.Y; // skipp height
		float len = (p - center).Length();
		if (len > radius)
			radius = len;

		//calculate bounds
		vMin = Vec3::Min(vMin, positions[i]);
		vMax = Vec3::Max(vMax, positions[i]);
	}

	Box bounds = Box(vMin, vMax - vMin);


	float size = radius*1.5f;
	if (bounds.Size.Y <= 2)
		size = 0;

	Field* f = new Field(size, radius*1.5f, pos + center, bounds);
	fields.push_back(f);


	
	return f;*/

	return nullptr;
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
	{
		weight += fields[i]->GetWeight(pos);
	}
	return weight;
}

Vec3 PotentialField::GetDir(Vec3 pos)
{
	Vec3 dir;
	float minWeight = 9999999;
	for (int i = 0; i < 15; i++)
	{
		Vec3 ranDir = Vec3((rand() % 200) / 100.0f - 1, 0.0f, (rand() % 200) / 100.0f - 1).Normalize();
		
		float weight = GetWeight(pos + ranDir);

		if (weight < minWeight)
		{
			//check if safe to walk
			//if (!standon)
			//{
				minWeight = weight;
				dir = ranDir;
			//}
			/*else//( standon->Test())
			{
				for (int i = 0; i < Collision::GetInstance()->GetCollisionInstances().size(); i++)
				{
					if (Collision::GetInstance()->GetCollisionInstances()[i]->Test(Ray(pos + ranDir * 3 + Vec3(0, 15, 0), Vec3(0, -30, 0))) > 0.0f)
					{
						minWeight = weight;
						dir = ranDir;
						break;
					}
				}
			}*/
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