#include "Entity.h"
#include "EntityManager.h"
#include "Cistron.h"
#include "Component.h"

#include <iostream>
#include <fstream>


using namespace Cistron;



// constructor/destructor
Entity::Entity(const Modules *modules, EntityId id, EntityId relativeid, string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
	float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated) : fId(id), type(type), fFinalized(false)
{

	this->relativeid = relativeid;

	this->isActive = true;
	this->isCollidible = isCollidible;

	this->type = type;

	this->pos.X = xPos;
	this->pos.Y = yPos;
	this->pos.Z = zPos;
	spawnpos = pos;

	this->rot.X = xRot;
	this->rot.Y = yRot;
	this->rot.Z = zRot;

	this->scale.X = xScale;
	this->scale.Y = yScale;
	this->scale.Z = zScale;

	this->model = model;
	this->isVisible = isVisible;
	this->isAnimated = isAnimated;

	this->modules = modules;
	this->mPhysicsEntity = this->modules->physicsEngine->CreateEntity();

	modelInst = nullptr;
	AnimInst = nullptr;

	ground = nullptr;
	wall = nullptr;

	if (isVisible)
	{
		if (!isAnimated)
		{
			this->modelInst = this->modules->graphics->CreateInstance(this->model, Vec3(this->pos.X, this->pos.Y, this->pos.Z));
			this->modelInst->SetRotation(this->rot);
			this->modelInst->SetScale(this->scale);
			this->modelInst->SetVisibility(this->isVisible);
		}
		else
		{
			this->AnimInst = this->modules->graphics->CreateAnimatedInstance(this->model);
			this->AnimInst->SetPosition(Vec3(this->pos.X, this->pos.Y, this->pos.Z));
			this->AnimInst->SetRotation(this->rot);
			this->AnimInst->SetScale(this->scale);
			this->AnimInst->SetVisibility(this->isVisible);

			std::string keyPath = "../../content/" + model + ".key";

			// Open corresponding keyframes file
			ifstream keyFramesFile;
			keyFramesFile.open(keyPath.c_str(), ios::in);

			if (!keyFramesFile.is_open())
			{
				std::stringstream ErrorStream;
				ErrorStream << "Failed to load keyframes file " << keyPath;
				std::string ErrorStreamStr = ErrorStream.str();
				LPCSTR Text = ErrorStreamStr.c_str();
				MessageBoxA(0, Text, 0, 0);
			}
			else
			{
				int index, start, end, playForwards;

				while (keyFramesFile >> index >> start >> end >> playForwards)
				{
					if (end - start > 1)
					{
						if (end > 1)
							end = end - 1;
					}

					this->AnimInst->CreateAnimation(index, start, end, playForwards != 0);
				}

				keyFramesFile.close();
			}

			this->AnimInst->SetAnimation(0);
		}
	}
	
	if (this->isCollidible && !isAnimated)
	{
		collInst = modules->collision->CreateCollisionInstance(model, pos);
		collInst->SetScale(scale);
		collInst->SetRotation(rot);
		field = this->modules->potentialField->CreateField(model, pos, scale);
	}
	else
	{
		collInst = nullptr;
		field = nullptr;
	}

	this->sphere = NULL;
	if (this->type == "player" || this->type == "AI")
	{
		cout << "creating sphere!" << this->type << endl;
		this->sphere = new Sphere(pos.X, pos.Y, pos.Z, 4);
	}

}
Entity::~Entity() {

	// delete all components
	// deleted by the Entity manager
	/*for (map<string, list<Component*> >::iterator it = fComponents.begin(); it != fComponents.end(); ++it) {
		for (list<Component*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			delete (*it2);
		}
	}*/
	if (modelInst)
		this->modules->graphics->DeleteInstance(this->modelInst);

	if (AnimInst)
		this->modules->graphics->DeleteInstance(this->AnimInst);

	if (field)
		this->modules->potentialField->DeleteField(field);
	
	if (mPhysicsEntity)
		this->modules->physicsEngine->DestroyEntity(mPhysicsEntity);
}

void Entity::update(float deltaTime)
{
	std::list<Component *> *componentList;

	for (auto nameIter = fComponents.begin(); nameIter != fComponents.end(); nameIter++)
	{
		componentList = &nameIter->second;

		for (auto componentIter = componentList->begin(); componentIter != componentList->end(); componentIter++)
		{
			(*componentIter)->update(deltaTime);
		}
	}
}

const Modules *Entity::getModules()
{
	return this->modules;
}

PhysicsEntity* Entity::getPhysics()
{
	return this->mPhysicsEntity;
}

// is the Entity finalized
bool Entity::isFinalized() {
	return fFinalized;
}


// finalize the Entity
void Entity::finalize() {

	

	// we're done
	fFinalized = true;
}


// add a component
bool Entity::addComponent(Component *comp) {

	// component must be valid
	if (!comp->isValid()) return false;

	// make sure there's no such component yet
	//if (fComponents.find(comp->getName()) != fComponents.end()) return false;

	// just add to the right list
	fComponents[comp->getName()].push_back(comp);
	return true;
}


// get a component
list<Component*> Entity::getComponents(string name) {

	// make sure there's no such component yet
	if (fComponents.find(name) == fComponents.end()) return list<Component*>();

	// return normally
	return fComponents[name];
}

bool Entity::hasComponents(string name) {

	return fComponents.count(name) > 0;
}

// get all components
list<Component*> Entity::getComponents() {

	// append them all
	list<Component*> comps;
	for (unordered_map<string, list<Component*> >::iterator it = fComponents.begin(); it != fComponents.end(); ++it) {
		comps.insert(comps.end(), it->second.begin(), it->second.end());
	}

	// return normally
	return comps;
}

// remove a component
void Entity::removeComponent(Component *comp) {

	// look for the right component ant delete it
	list<Component*>& comps = fComponents[comp->getName()];
	for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
		if ((*it)->getId() == comp->getId()) {
			comps.erase(it);
			break;
		}
	}

	// now we remove all local requests
	for (unsigned i = 0; i < fLocalRequests.size(); ++i) {
		vector<RegisteredComponent>& regs = fLocalRequests[i];
		for (unsigned j = 0; j < regs.size(); ++j) {
			if (regs[j].component->getId() == comp->getId()) {
				
				// overwrite
				for (unsigned k = j+1; k < regs.size(); ++k) {
					regs[k-1] = regs[k];
				}
				regs.pop_back();
				break;
			}
		}
	}
}


// send a local message
void Entity::sendMessage(RequestId reqId, Message const & msg) {

	// if there are no registered components, we just skip
	if ((int)fLocalRequests.size() <= reqId) return;

	// just forward to the appropriate registered components
	vector<RegisteredComponent>& regs = fLocalRequests[reqId];
	unsigned n = (unsigned)regs.size();
	for (unsigned i = 0; i < n; ++i) {
		RegisteredComponent& comp = regs[i];
		if (comp.trackMe) {
			string name;
			if (msg.type == MESSAGE) {
				name = comp.component->getEntityManager()->getRequestById(REQ_MESSAGE, reqId);
			}
			else {
				name = comp.component->getEntityManager()->getRequestById(REQ_COMPONENT, reqId);
			}
		}
		comp.callback(msg);
	}
}

// register a request
void Entity::registerRequest(RequestId reqId, RegisteredComponent reg) {

	// TODO OPTIMIZATION

	// if it doesn't exist yet, create it
	if ((int)fLocalRequests.size() <= reqId) {
		fLocalRequests.resize(reqId+1);
	}
	fLocalRequests[reqId].push_back(reg);
}


// track a local request
void Entity::trackRequest(RequestId reqId, Component *component) {

	// find in global request list
	for (unsigned i = 0; i < fLocalRequests[reqId].size(); ++i) {
		if (fLocalRequests[reqId][i].component->getId() == component->getId()) fLocalRequests[reqId][i].trackMe = true;
	}
}


void Entity::sendAMessageToAll(string message)
{
	//fins the first component in the entity with the name "Messenger"
	if (this->fComponents.count("Messenger") != 0)
	{
		this->fComponents["Messenger"].front()->sendMessage(message);
	}
}

void Entity::sendMessageToEntity(string message, EntityId id)
{
	if (this->fComponents.count("Messenger") != 0)
	{
		this->fComponents["Messenger"].front()->sendMessageToEntity(id, message);
	}
}

string Entity::getType()
{
	return this->type;
}

EntityId Entity::getEntityId()
{
	return this->fId;
}

Vec3 Entity::returnPos()
{
	if (ground == nullptr)
		return this->pos;
	else
		return ground->returnPos() + this->pos;
}

Vec3 Entity::returnRot()
{
	return this->rot;
}

Vec3 Entity::returnScale()
{
	return this->scale;
}

bool Entity::returnVisible()
{
	return this->isVisible;
}

bool Entity::getIsActive()
{
	return this->isActive;
}

CollisionInstance* Entity::returnCollision()
{
	return this->collInst;
}

void Entity::updatePos(Vec3 pos)
{
	this->pos = pos;

	if (!ground)
		this->pos = pos;
	else
		this->pos = pos - ground->returnPos();

	if(this->modelInst)
		this->modelInst->SetPosition(returnPos());
	if (this->AnimInst)
		this->AnimInst->SetPosition(returnPos());
	if (this->collInst)
		this->collInst->SetPosition(returnPos());
	if (this->field)
		this->field->Move(returnPos());

	if (this->sphere != NULL)
	{
		this->sphere->Position = returnPos();
	}
}

void Entity::changeRelative(Entity* ground)
{
	if (ground != this->ground)
	{
		if (ground)
			this->pos = returnPos() - ground->returnPos();
		else
			this->pos = returnPos();
		this->ground = ground;
	}
}

void Entity::updateRot(Vec3 rot)
{
	this->rot = rot;
	if(this->modelInst)
		this->modelInst->SetRotation(rot);
	if (this->AnimInst)
		this->AnimInst->SetRotation(rot);
	//if (this->collInst)
		//this->collInst->SetRotation(rot);
}

void Entity::updateScale(Vec3 scale)
{
	this->scale = scale;
	if(this->modelInst)
		this->modelInst->SetScale(scale);
	if (this->collInst)
		this->collInst->SetScale(scale);
}

void Entity::updateVisible(bool isVisible)
{
	this->isVisible = isVisible;
	if (this->modelInst)
		this->modelInst->SetVisibility(isVisible);
}

void Entity::setIsActive(bool status)
{
	this->isActive = status;
	if (this->isVisible)
	{
		this->modelInst->SetVisibility(status);
		if (this->isAnimated)
		{
			this->AnimInst->SetVisibility(status);
		}
		if (this->isCollidible)
		{
			this->collInst->setIsActive(status);
		}
	}

	for (auto iter1 = this->fComponents.begin(); iter1 != this->fComponents.end(); iter1++)
	{
		for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++)
		{					
			Component *component = (*iter2);
			component->setActive(status);
		}
	}
}

void Entity::updateRelativePos(Vec3 pos)
{
	this->pos = pos;
}
Vec3 Entity::getRelativePos()
{
	return this->pos;
}

AnimatedInstance *Entity::getAnimatedInstance()
{
	return this->AnimInst;
}
