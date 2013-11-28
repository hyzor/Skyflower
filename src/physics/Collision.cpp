#include "Collision.h"

Collision* Collision::instance = nullptr;

Collision::Collision()
{
}
Collision::~Collision()
{
	for (std::map<std::string, CollisionModel*>::iterator it = models.begin(); it != models.end(); it++)
		delete it->second;
}
Collision* Collision::GetInstance()
{
	if (!instance)
		instance = new Collision();
	return instance;
}

CollisionInstance* Collision::CreateCollisionInstance(std::string file, Vec3 pos)
{
	CollisionInstance *ret;
	std::map<std::string, CollisionModel*>::iterator it = models.find(file);
	if (it != models.end())
		ret = new CollisionInstance(it->second, pos);
	else
	{
		CollisionModel* cm = new CollisionModel();
		cm->LoadObj(file);
		it = models.begin();
		models.insert(it, std::pair<std::string, CollisionModel*>(file, cm));
		ret = new CollisionInstance(cm, pos);
	}
	instances.push_back(ret);
	return ret;
}

void Collision::DeleteCollisionInstance(CollisionInstance* ci)
{
	CollisionModel* cm = ci->Model;

	bool found = false;
	int index = -1;
	for (unsigned int i = 0; i < instances.size(); i++)
	{
		if (ci == instances[i])
			index = i;
		else if (cm == instances[i]->Model)
				found = true;
	}

	if (index != -1)
	{
		instances.erase(instances.begin() + index);
		delete ci;
	}
	if (!found) //delete model if no other instance uses it
	{
		for (std::map<std::string, CollisionModel*>::iterator it = models.begin(); it != models.end(); it++)
		{
			if (it->second == cm)
			{
				models.erase(it);
				break;
			}
		}
		delete cm;
	}
}

std::vector<CollisionInstance*> Collision::GetCollisionInstances()
{
	return instances;
}