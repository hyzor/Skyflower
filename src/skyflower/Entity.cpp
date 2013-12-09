
#include "Cistron.h"


using namespace Cistron;



// constructor/destructor
Entity::Entity(EntityId id, string type) : fId(id), type(type), fFinalized(false)
{
	this->pos.X = 1;
	this->pos.Y = 2;
	this->pos.Z = 3;
}
Entity::~Entity() {

	// delete all components
	// deleted by the Entity manager
	/*for (map<string, list<Component*> >::iterator it = fComponents.begin(); it != fComponents.end(); ++it) {
		for (list<Component*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			delete (*it2);
		}
	}*/
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
	if (fLocalRequests.size() <= reqId) return;

	// just forward to the appropriate registered components
	vector<RegisteredComponent>& regs = fLocalRequests[reqId];
	int n = regs.size();
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
	if (fLocalRequests.size() <= reqId) {
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
	//this->fComponents["Monster"].front()->sendMessage("Hello");
	this->fComponents["Monster"].front()->sendMessage(message);
}

void Entity::sendMessageToEntity(string message, EntityId id)
{
	this->fComponents["Monster"].front()->sendMessageToEntity(id, message);
}

bool Entity::getType(string type)
{
	if (this->type == type)
	{
		return true;
	}
	else
	{
		return false;
	}
}

EntityId Entity::getEntityId()
{
	return this->fId;
}

Vec3 Entity::returnPos()
{
	return this->pos;
}

void Entity::updatePos(Vec3 pos)
{
	this->pos = pos;
	cout << this->pos.X << endl;
}