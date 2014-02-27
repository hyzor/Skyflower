#include "potentialfield.h"

// Must be included last!
#include "shared/debug.h"

#include "Physics/Sphere.h"

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

Field* PotentialField::CreateField(CollisionInstance* collinst, Vec3 pos, float scale)
{
	Sphere s = collinst->GetSphere();
	s.Position -= collinst->GetPosition();
	Box b = collinst->GetBox();
	b.Position -= collinst->GetPosition();

	s.Radius *= scale;
	s.Position *= scale;
	b.Size *= scale;
	b.Position *= scale;

	float size = s.Radius*1.5f;
	if (b.Size.Y <= 2 || b.Size.X > 50 || b.Size.Z > 50) //platformarna räknas inte in
		size = 0;
	Field* f = new Field(size*0.9f, s.Radius*1.2f, s.Position, pos, b);
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