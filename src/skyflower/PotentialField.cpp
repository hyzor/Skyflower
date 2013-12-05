#include "potentialfield.h"


PotentialField::PotentialField()
{
	srand(time(0));
}

PotentialField::~PotentialField()
{
	for (unsigned int i = 0; i < fields.size(); i++)
		delete fields[i];
}

Field* PotentialField::CreateField(float weight, float size, Vec3 pos)
{
	Field* f = new Field(weight, size, pos);
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
			minWeight = weight;
			dir = ranDir;
		}
	}

	return dir;
}