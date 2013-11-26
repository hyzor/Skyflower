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

CollisionInstance Collision::CreateCollisionInstance(std::string file, Vec3 pos)
{
	std::map<std::string, CollisionModel*>::iterator it = models.find(file);
	if (it != models.end())
		return CollisionInstance(it->second, pos);
	else
	{
		CollisionModel* cm = new CollisionModel();
		cm->LoadObj(file);
		it = models.begin();
		models.insert(it, std::pair<std::string, CollisionModel*>(file, cm));
		return CollisionInstance(cm, pos);
	}
}