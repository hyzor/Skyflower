#include "Entity.h"
#include "EntityManager.h"
#include "Cistron.h"
#include "Component.h"


using namespace Cistron;



// constructor/destructor
Entity::Entity(const Modules *modules, EntityId id, string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
	float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated) : fId(id), type(type), fFinalized(false)
{
	this->type = type;

	this->pos.X = xPos;
	this->pos.Y = yPos;
	this->pos.Z = zPos;

	this->rot.X = xRot;
	this->rot.Y = yRot;
	this->rot.Z = zRot;

	this->scale.X = xScale;
	this->scale.Y = yScale;
	this->scale.Z = zScale;

	this->model = model;
	this->isVisible = isVisible;
	this->physics = new Physics();

	this->modules = modules;

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

			// Cache animation keyframes
			this->AnimInst->CreateAnimation(0, 51 + 15, 51 + 15);
			this->AnimInst->CreateAnimation(1, 1, 24 + 7);
			this->AnimInst->CreateAnimation(2, 1, 24 + 7);
			this->AnimInst->CreateAnimation(3, 30 + 7, 49 + 10);
			this->AnimInst->CreateAnimation(4, 51 + 15, 75 + 20);
			this->AnimInst->CreateAnimation(5, 81 + 20, 105 + 25);

			this->AnimInst->SetAnimation(1);
		}
	}
	
	if (isCollidible && !isAnimated)
	{
		collInst = Collision::GetInstance()->CreateCollisionInstance(model, pos);
		collInst->SetScale(scale);
		collInst->SetRotation(rot);
		field = this->modules->potentialField->CreateField(model, pos);
	}
	else
	{
		collInst = nullptr;
		field = nullptr;
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

	//if (AnimInst)
		//this->modules->graphics->DeleteInstance(this->AnimInst);
	
	delete this->physics;
}

void Entity::update(float deltaTime)
{
	list<Component *> components = getComponents();

	for (auto iter = components.begin(); iter != components.end(); iter++)
	{
		if ((*iter) != NULL)
			(*iter)->update(deltaTime);
	}
}

const Modules *Entity::getModules()
{
	return this->modules;
}

Physics* Entity::getPhysics()
{
	return this->physics;
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

	// make sure there's no such component yet
	//if (fComponents.find(name) == fComponents.end()) 
		//return false;


	list<Component*> comps = getComponents();
	for (std::list<Component*>::iterator it = comps.begin(); it != comps.end(); it++)
	{
		if ((*it)->getName() == name)
			return true;
	}

	// return normally
	return false;
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


//my own
void Entity::sendAMessageToAll(string message)
{
	//loopar igenom alla komponentamn, och sedan alla komponenter med det namnet.
	//loop 1, går igenom alla komponentnamn
	//loop 2, går igenom alla komponenter med det namnet
	//for (auto iter = this->fComponents.begin(); iter != this->fComponents.end(); iter++)
	//{
	//	list<Component *> &components = (*iter).second;

	//	for (auto iter2 = components.begin(); iter2 != components.end(); iter2++)
	//	{
	//		Component *component = (*iter2);
	//		component->sendMessage("Hello");
	//	}
	//}


	////loopar igenom alla komponenten med namnet messanger
	//list<Component *> &components = this->fComponents["Monster"];

	//for (auto iter2 = components.begin(); iter2 != components.end(); iter2++)
	//{
	//	Component *component = (*iter2);
	//}

	//första komponenten med namnet messanger
	//this->fComponents["Monster"].front()->sendMessage(message);
	if (this->fComponents.count("Messenger") != 0)
	{
		this->fComponents["Messenger"].front()->sendMessage(message);
	}


	//if (this->fComponents["Messenger"].front() != NULL)
	//{
	//	this->fComponents["Messenger"].front()->sendMessage(message);
	//}

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
	return this->pos;
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

CollisionInstance* Entity::returnCollision()
{
	return this->collInst;
}

void Entity::updatePos(Vec3 pos)
{
	this->pos = pos;
	if(this->modelInst)
		this->modelInst->SetPosition(pos);
	if (this->AnimInst)
		this->AnimInst->SetPosition(pos);
	if (this->collInst)
		this->collInst->Position = pos;
	if (this->field)
		this->field->Move(pos);
	//cout << this->pos.X << endl;
}

void Entity::updateRot(Vec3 rot)
{
	this->rot = rot;
	if(this->modelInst)
		this->modelInst->SetRotation(rot);
	if (this->AnimInst)
		this->AnimInst->SetRotation(rot);
}

void Entity::updateScale(Vec3 scale)
{
	this->scale = scale;
	this->modelInst->SetScale(scale);
}

void Entity::updateVisible(bool isVisible)
{
	this->isVisible = isVisible;
	this->modelInst->SetVisibility(isVisible);
}
