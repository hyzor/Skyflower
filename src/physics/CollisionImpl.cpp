#include "CollisionImpl.h"

// Must be included last!
#include "shared/debug.h"

DLL_API Collision *CreateCollision(const std::string &relativePath)
{
	CollisionImpl *collision = new CollisionImpl(relativePath);

	return (Collision *)collision;
}

DLL_API void DestroyCollision(Collision *collision)
{
	if (!collision)
		return;

	CollisionImpl *collisionImpl = (CollisionImpl *)collision;

	delete collisionImpl;
}

CollisionImpl::CollisionImpl(const std::string &relativePath)
{
	this->relativePath = relativePath;
}

CollisionImpl::~CollisionImpl()
{
	for (size_t i = 0; i < instances.size(); i++)
		delete instances[i];

	for (auto it = models.begin(); it != models.end(); it++)
		delete it->second;
}

CollisionInstance* CollisionImpl::CreateCollisionInstance(const std::string &file, Vec3 pos)
{
	CollisionInstance *ret;
	std::map<std::string, CollisionModel*>::iterator it = models.find(file);

	if (it != models.end())
	{
		ret = (CollisionInstance *)new CollisionInstanceImpl(it->second, pos);
	}
	else
	{
		CollisionModel* cm = new CollisionModel();
		cm->Load(relativePath + file);
		models[file] = cm;

		ret = (CollisionInstance *)new CollisionInstanceImpl(cm, pos);
	}

	instances.push_back(ret);
	return ret;
}

void CollisionImpl::DeleteCollisionInstance(CollisionInstance* ci)
{
	CollisionInstanceImpl *cii = (CollisionInstanceImpl *)ci;
	CollisionModel* cm = cii->GetModel();

	bool found = false;
	int index = -1;
	for (unsigned int i = 0; i < instances.size(); i++)
	{
		if (ci == instances[i])
			index = i;
		else if (cm == ((CollisionInstanceImpl *)instances[i])->GetModel())
			found = true;
	}

	if (index != -1)
		instances.erase(instances.begin() + index);

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

	delete ci;
}

std::vector<CollisionInstance*> CollisionImpl::GetCollisionInstances()
{
	return instances;
}