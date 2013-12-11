
#include "Cistron.h"

using namespace Cistron;
using namespace tinyxml2;
#include <iostream>
#include <cassert>
#include "shared/Vec3.h"

using std::cout;
using std::endl;


// constructor/destructor
EntityManager::EntityManager(GraphicsEngine* gEngine) : fIdCounter(0), fRequestIdCounter(0), fNLocks(0) {

	this->gEngine = gEngine;
	// because we start counting from 1 for request id's, we add an empty request lock in front
	fRequestLocks.push_back(RequestLock());
}
EntityManager::~EntityManager() {

	// delete all Entitys
	for (unsigned i = 0; i < fEntitys.size(); ++i) {

		// already destroyed earlier
		if (fEntitys[i] == 0) continue;

		// destroy every component in the Entity
		list<Component*> comps = fEntitys[i]->getComponents();
		for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
			destroyComponent(*it);
		}

		// destroy the Entity itself
		delete fEntitys[i];
	}
}


// generate a unique request id or return one if it already exists
RequestId EntityManager::getMessageRequestId(ComponentRequestType type, string name) {

	// ALL_COMPONENTS is changed to COMPONENT, it's the same in regards to the request id
	if (type == REQ_ALLCOMPONENTS) type = REQ_COMPONENT;

	// if it doesn't exist, create it
	if (fRequestToId[type].find(name) == fRequestToId[type].end()) {
		fRequestToId[type][name] = ++fRequestIdCounter;
		fIdToRequest[type][fRequestIdCounter] = name;
		fRequestLocks.push_back(RequestLock());
		return fRequestIdCounter;
	}

	// it exists, just return it
	return fRequestToId[type][name];
}


// return existing request id
RequestId EntityManager::getExistingRequestId(ComponentRequestType type, string name) {

	// if it doesn't exist we don't return it
	if (fRequestToId[type].find(name) == fRequestToId[type].end()) {
		return 0;
	}

	// if it does exist, but there are no global requests, we don't return it either
	RequestId id = fRequestToId[type][name];
	if ((int)fGlobalRequests.size() <= id) return 0;

	// we might have a global request - process it
	return id;
}



// create a new Entity
EntityId EntityManager::createEntity(string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
	float xScale, float yScale, float zScale, string model, bool isVisible) {

	// create a new Entity
	Entity *obj = new Entity(gEngine,fIdCounter, type, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible);
	//cout << "Created Entity " << fIdCounter << endl;
	++fIdCounter;

	// add it to the list
	fEntitys.push_back(obj);
	return fIdCounter-1;
}


// activate lock
void EntityManager::activateLock(RequestId reqId) {

	// if the lock is already activated, we have bounced against an infinite loop
	if (fRequestLocks[reqId].locked) {
		stringstream ss;
		ss << "Do not request a message or component in a callback function for the same message/component request or a function called by this callback function";
		error(ss);
	}

	// one more lofk
	++fNLocks;

	// activate the lock
	fRequestLocks[reqId].locked = true;
}


// release the lock, process the pending global and local requests
void EntityManager::releaseLock(RequestId reqId) {

	// lock
	RequestLock& lock = fRequestLocks[reqId];

	// must be locked!
	assert(lock.locked);

	// release the lock
	lock.locked = false;

	// one lock less
	--fNLocks;

	// copy the pending requests
	list<pair<ComponentRequest, RegisteredComponent> > pendingGlobal = lock.pendingGlobalRequests;
	list<pair<ComponentRequest, RegisteredComponent> > pendingLocal = lock.pendingLocalRequests;
	lock.pendingGlobalRequests = list<pair<ComponentRequest, RegisteredComponent> >();
	lock.pendingLocalRequests = list<pair<ComponentRequest, RegisteredComponent> >();

	// process the global requests
	for (list<pair<ComponentRequest, RegisteredComponent> >::iterator it = pendingGlobal.begin(); it != pendingGlobal.end(); ++it) {
		registerGlobalRequest(it->first, it->second);
	}

	// process the local requests
	for (list<pair<ComponentRequest, RegisteredComponent> >::iterator it = pendingLocal.begin(); it != pendingLocal.end(); ++it) {
		registerLocalRequest(it->first, it->second);
	}

	// if there are no more locks, destroy any pending components & Entitys
	if (fNLocks == 0) {

		// first components
		list<Component*> deadComponents = fDeadComponents;
		fDeadComponents.clear();
		for (list<Component*>::iterator it = deadComponents.begin(); it != deadComponents.end(); ++it) {
			destroyComponent(*it);
		}

		// then entire Entitys
		list<EntityId> deadEntitys = fDeadEntitys;
		fDeadEntitys.clear();
		for (list<EntityId>::iterator it = deadEntitys.begin(); it != deadEntitys.end(); ++it) {
			destroyEntity(*it);
		}
	}
}


// add a new component to an Entity
void EntityManager::addComponent(EntityId id, Component *component) {

	// make sure the Entity exists
	if (id < 0 || id >= (int)fEntitys.size() || fEntitys[id] == 0) {
		stringstream ss;
		ss << "Failed to add component " << component->toString() << " to Entity " << id << ": it does not exist!";
		error(ss);
	}

	// can only add once
	if (component->fEntityManager != 0) {
		stringstream ss;
		ss << "Component is already part of an EntityManager. You cannot add a component twice.";
		error(ss);
	}

	// we get the appropriate Entity
	Entity *obj = fEntitys[id];

	// set the Entity manager
	component->fEntityManager = this;

	// set the owner of the component
	component->setOwner(id);

	// we add the component
	if (!obj->addComponent(component)) {
		stringstream ss;
		ss << "Failed to add component " << component->toString() << " to Entity " << id;
		error(ss);
	}

	// put in log
	//if (fStream.is_open()) fStream << "CREATE  " << *component << endl;
	//cout << "CREATE " << *component << endl;

	// let the component know
	component->addedToEntity();

	// get request id for this component
	RequestId reqId = getExistingRequestId(REQ_COMPONENT, component->getName());

	// if there's no such request yet, we skip
	if (reqId == 0) return;

	// CREATE event
	Message msg(CREATE);
	msg.sender = component;

	// lock this request id
	activateLock(reqId);

	// look for requests and forward them
	for (list<RegisteredComponent>::iterator it = fGlobalRequests[reqId].begin(); it != fGlobalRequests[reqId].end(); ++it) {
		if (it->component->getId() != component->getId()) {
			if ((*it).trackMe) cout << it->component << " received component " << *component << " of type " << fIdToRequest[REQ_COMPONENT][reqId] << endl; 
			(*it).callback(msg);
		}
	}

	// forward to the Entity itself, so local requests are processed also
	obj->sendMessage(reqId, msg);

	// release the lock
	releaseLock(reqId);
}


// register a local request
// only COMPONENT requests can be local!!
void EntityManager::registerLocalRequest(ComponentRequest req, RegisteredComponent reg) {

	// we generate the request id (might be new)
	RequestId reqId = getMessageRequestId(req.type, req.name);

	// if this request is locked, postpone the processing
	if (fRequestLocks[reqId].locked) {
		fRequestLocks[reqId].pendingLocalRequests.push_back(pair<ComponentRequest, RegisteredComponent>(req, reg));
		return;
	}

	// forward to appropriate Entity
	fEntitys[reg.component->getOwnerId()]->registerRequest(reqId, reg);

	// put in log
	//if (fStream.is_open()) fStream << "DESTROY " << *comp << endl;

//cout << "Registered local request of " << (*reg.component) << " for " << req.name << endl;
	// if we want the previously created components as well, we process them
	if (req.type != REQ_COMPONENT) return;
	
	// lock this request id
	activateLock(reqId);

	// now look for existing components of this type
	Message msg(CREATE);

	// get component and forward it
	list<Component*> comps = fEntitys[reg.component->getOwnerId()]->getComponents(req.name);
	for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
		if ((*it)->isValid() && reg.component->getId() != (*it)->getId()) {
			msg.sender = (*it);
			if (reg.trackMe) cout << *reg.component << " received component " << (*msg.sender) << " of type " << req.name << endl;
			//cout << "Warning component " << (*reg.component) << " for the local existence of component " << (*msg.sender) << endl;
			reg.callback(msg);
		}
	}

	// release the lock
	releaseLock(reqId);
}


// register a global request
void EntityManager::registerGlobalRequest(ComponentRequest req, RegisteredComponent reg) {
	assert(reg.component->isValid());

	// first we request the id and create it if it doesn't exist
	RequestId reqId = getMessageRequestId(req.type, req.name);

	// we only really register component and message requests
	if (req.type != REQ_ALLCOMPONENTS) {

		// if this request is locked, postpone the processing
		if (fRequestLocks[reqId].locked) {
			fRequestLocks[reqId].pendingGlobalRequests.push_back(pair<ComponentRequest, RegisteredComponent>(req, reg));
			return;
		}

		// if the request list isn't large enough, we resize it
		if ((int)fGlobalRequests.size() <= reqId) {
			fGlobalRequests.resize(reqId+1);
		}

		// we add the request
		fGlobalRequests[reqId].push_back(reg);
	//cout << "Registered global request of " << (*reg.component) << " for " << req.name << endl;
		// we also add it locally if it is a message
		if (req.type == REQ_MESSAGE) {
			fEntitys[reg.component->getOwnerId()]->registerRequest(reqId, reg);
		}

		// we add it to the associative map (used at destruction of the component to look up all its requests)
		fRequestsByComponentId[reg.component->getId()].push_back(req);

		// if the request is required and the Entity isn't finalized yet, we add it to a special list
		EntityId objId = reg.component->getOwnerId();
		if (reg.required && !fEntitys[objId]->isFinalized()) {
			fRequiredComponents[objId].push_back(req.name);
		}
	}

	// if we want the previously created components as well, we process them
	if (req.type == REQ_MESSAGE) return;
	
	// activate the lock on this id
	activateLock(reqId);

	// now look for existing components of this type
	Message msg(CREATE);
	for (unsigned i = 0; i < fEntitys.size(); ++i) {

		// only process Entitys that still exist
		if (fEntitys[i] == 0) continue;

		// get component and forward it
		list<Component*> comps = fEntitys[i]->getComponents(req.name);
		for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
			if ((*it)->isValid() && reg.component->getId() != (*it)->getId()) {
				msg.sender = (*it);
				if (reg.trackMe) cout << (*reg.component) << " received component " << (**it) << " of type " << req.name << endl; 
				//cout << "Warning component " << (*reg.component) << " for the global existence of component " << (*msg.sender) << endl;
				reg.callback(msg);
			}
		}
	}

	// release the lock
	releaseLock(reqId);
}


// send a message to everyone
void EntityManager::sendGlobalMessage(RequestId reqId, Message const & msg) {

	// must be valid component
	assert(msg.sender->isValid());

	// activate the lock
	activateLock(reqId);

	// look for requests and forward them
	for (list<RegisteredComponent>::iterator it = fGlobalRequests[reqId].begin(); it != fGlobalRequests[reqId].end(); ++it) {
		if ((*it).trackMe) cout << it->component << " received message " << fIdToRequest[REQ_MESSAGE][reqId] << " from " << *msg.sender << endl; 
		(*it).callback(msg);
	}

	// release the lock
	releaseLock(reqId);
}


// error processing
void EntityManager::error(stringstream& err) {
	cout << err.str() << endl;
	assert(false);
}


// destroy Entity
void EntityManager::destroyEntity(EntityId id) {

	// if there's no lock, we delete the Entity immediately, otherwise, postpone
	if (fNLocks != 0) {
		fDeadEntitys.push_back(id);
		return;
	}

	// Entity doesn't exist
	if (id < 0 || id >= (int)fEntitys.size() || fEntitys[id] == 0) {
		stringstream ss;
		ss << "Failed to destroy Entity " << id << ": it does not exist!";
		error(ss);
	}

	// destroy every component in the Entity
	list<Component*> comps = fEntitys[id]->getComponents();
	for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
		destroyComponent(*it);
	}

	// delete the actual Entity
	//cout << "Destroyed Entity " << id << endl;
	delete fEntitys[id];
	fEntitys[id] = 0;
}



// destroy a component
void EntityManager::destroyComponent(Component *comp) {

	// invalid component shouldn't be possible
	if (!comp->isValid()) {
		stringstream ss;
		ss << "Error destroying component " << comp->toString() << ": component is not valid.";
		error(ss);
	}


	// see if there are any locks - if there are, postpone this destroyal
	if (fNLocks != 0) {
		fDeadComponents.push_back(comp);
		return;
	}
	

	// put in log
	//if (fStream.is_open()) fStream << "DESTROY " << *comp << endl;

	// remove its own requests
	list<ComponentRequest>& reqs = fRequestsByComponentId[comp->getId()];
	for (list<ComponentRequest>::iterator it = reqs.begin(); it != reqs.end(); ++it) {

		// get the appropriate id
		RequestId reqId = getExistingRequestId(it->type, it->name);

		// must exist!
		assert(reqId != 0);

		// look up the request and delete it
		for (list<RegisteredComponent>::iterator reg = fGlobalRequests[reqId].begin(); reg != fGlobalRequests[reqId].end();) {
			if (reg->component->getId() == comp->getId()) {
				reg = fGlobalRequests[reqId].erase(reg);
			}
			else ++reg;
		}
	}

	// remove its own local requests - only if the Entity itself wasn't removed yet
	fEntitys[comp->getOwnerId()]->removeComponent(comp);

	// CREATE event
	Message msg(DESTROY);
	msg.sender = comp;

	// get req id
	RequestId reqId = getExistingRequestId(REQ_COMPONENT, comp->getName());

	// if there exist some requests, we process them
	if (reqId != 0) {

		// activate lock
		activateLock(reqId);

		// look up the request and delete it
		for (list<RegisteredComponent>::iterator reg = fGlobalRequests[reqId].begin(); reg != fGlobalRequests[reqId].end(); ++reg) {
			reg->callback(msg);
		}

		// forward to the Entity itself, so local requests are processed also
		fEntitys[comp->getOwnerId()]->sendMessage(reqId, msg);

		// release the lock
		releaseLock(reqId);
	}

	// make it invalid
	delete comp;
}


// finalize an Entity
void EntityManager::finalizeEntity(EntityId id) {

	// Entity doesn't exist
	if (id < 0 || id >= (int)fEntitys.size() || fEntitys[id] == 0) {
		stringstream ss;
		ss << "Failed to destroy Entity " << id << ": it does not exist!";
		error(ss);
	}

	// finalize the Entity itself
	fEntitys[id]->finalize();

	// see if there are any requirements
	if (fRequiredComponents.find(id) == fRequiredComponents.end()) return;

	// there are, do the checklist
	list<string> requiredComponents = fRequiredComponents[id];
	bool destroyEntity = false;
	for (list<string>::iterator it = requiredComponents.begin(); it != requiredComponents.end(); ++it) {

		// get the components of this type
		list<Component*> comps = fEntitys[id]->getComponents(*it);

		// if there are none, we want this Entity dead!
		if (comps.size() == 0) destroyEntity = true;
	}
/*if (!destroyEntity) cout << "Finalized Entity " << id << " succesfully!" << endl;
else cout << "Finalize on Entity " << id << " failed, destroying..." << endl;*/
	// we destroy the Entity if we didn't find what we needed
	if (destroyEntity) this->destroyEntity(id);
}


// register a unique name for an Entity
void EntityManager::registerName(Component *comp, string name) {
	fComponentsByName[name].push_back(comp);
}
void EntityManager::unregisterName(Component *comp, string name) {

	// not found
	if (fComponentsByName.find(name) == fComponentsByName.end()) return;

	// unregister
	for (list<Component*>::iterator it = fComponentsByName[name].begin(); it != fComponentsByName[name].end(); ++it) {
		if ((*it)->getId() == comp->getId()) {
			it = fComponentsByName[name].erase(it);
			return;
		}
	}
}


// get the id based on the unique name identified
list<Component*> EntityManager::getComponentsByName(string name) {

	// see if the name doesn't exist yet
	if (fComponentsByName.find(name) == fComponentsByName.end()) {
		//error(format("Failed to acquire Entity id for unique name identifier %s because it doesn't exist!") % name);
		return list<Component*>();
	}

	// return id
	return fComponentsByName[name];
}


// logging
void EntityManager::trackRequest(RequestId reqId, bool local, Component *component) {

	// if global, find it
	if (!local) {

		// find in global request list
		for (list<RegisteredComponent>::iterator it = fGlobalRequests[reqId].begin(); it != fGlobalRequests[reqId].end(); ++it) {
			if ((*it).component->getId() == component->getId()) (*it).trackMe = true;
		}

		// also pass to local for extra check (MESSAGE messages are always local AND global)
		fEntitys[component->getOwnerId()]->trackRequest(reqId, component);


	}

	// if local, forward to Entity
	else {
		fEntitys[component->getOwnerId()]->trackRequest(reqId, component);
	}
}


//my own
void EntityManager::sendMessageToAllEntities(string message)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		this->fEntitys[i]->sendAMessageToAll(message);
	}
}

void EntityManager::sendMessageToEntity(string message, string entity)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getType() == entity)
		{
			this->fEntitys[i]->sendMessageToEntity(message, this->fEntitys[i]->fId);
		}
	}
}


bool EntityManager::loadXML(EntityManager *entityManager, string xmlFile)
{
	string path = "../../content/XML/" + xmlFile;

	XMLDocument doc;
	doc.LoadFile(path.c_str());

	//Try to open the file
	FILE *fp = fopen(path.c_str(), "r");
	if (!fp)
	{
		cout << "Error opening text file" << endl;
		return false;
	}

	//Find the root element
	XMLElement* root = doc.FirstChildElement();
	if (root == NULL)
	{
		cout << "Failed to load file: No root element." << endl;
		doc.Clear();
		return false;
	}

	if (xmlFile == "test1.xml")
	{

	}
	else if (xmlFile == "test3.xml")
	{
		cout << "3:an h�r!" << endl;
	}
	else if (xmlFile == "test2.xml" || xmlFile == "test22.xml")
	{
		//For every entity that is to be created in this EntityManager
		for (XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
		{
			string elemName = elem->Value();
			const char* attr;

			//if the entity type is Player
			if (elemName == "Player")
			{
				float xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale;
				string model = "";
				bool isVisible = false;

				attr = elem->Attribute("xPos");
				if (attr != NULL)
				{
					xPos = elem->FloatAttribute("xPos");
				}

				attr = elem->Attribute("yPos");
				if (attr != NULL)
				{
					yPos = elem->FloatAttribute("yPos");
				}

				attr = elem->Attribute("zPos");
				if (attr != NULL)
				{
					zPos = elem->FloatAttribute("zPos");
				}

				attr = elem->Attribute("xRot");
				if (attr != NULL)
				{
					xRot = elem->FloatAttribute("xRot");
				}

				attr = elem->Attribute("yRot");
				if (attr != NULL)
				{
					yRot = elem->FloatAttribute("yRot");
				}

				attr = elem->Attribute("zRot");
				if (attr != NULL)
				{
					zRot = elem->FloatAttribute("zRot");
				}

				attr = elem->Attribute("xScale");
				if (attr != NULL)
				{
					xScale = elem->FloatAttribute("xScale");
				}

				attr = elem->Attribute("yScale");
				if (attr != NULL)
				{
					yScale = elem->FloatAttribute("yScale");
				}

				attr = elem->Attribute("zScale");
				if (attr != NULL)
				{
					zScale = elem->FloatAttribute("zScale");
				}

				attr = elem->Attribute("model");
				if (attr != NULL)
				{
					model = elem->Attribute("model");
				}

				attr = elem->Attribute("isVisible");
				if (attr != NULL)
				{
					isVisible = elem->BoolAttribute("isVisible");
				}
				//Creating the Player entity and adding it to the entitymanager
				EntityId player = entityManager->createEntity("Player", xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible);


				//Looping through all the components for Player-entity.
				for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
				{
					string componentName = e->Value();
					if (componentName == "Monster")
					{
						attr = e->Attribute("name");

						string name = "";
						if (attr != NULL)
						{
							name = e->Attribute("name");
						}
						else
							return false;

						//create the Monster component and add that component to the Player entity
						Monster *m1 = new Monster(name);
						entityManager->addComponent(player, m1);
					}

					else if (componentName == "Person")
					{
						attr = e->Attribute("name");
						string name = "";
						if (attr != NULL)
						{
							name = e->Attribute("name");
						}
						else
							return false;

						attr = e->Attribute("age");
						int age = 0;
						if (attr != NULL)
						{
							int *age2 = 0;
							age = e->IntAttribute("age");
						}
						else
							return false;

						//create the Person component and add that component to the Player entity
						Person *p1 = new Person(name, age);
						entityManager->addComponent(player, p1);
					}

					else if (componentName == "Input")
					{
						Input* i = new Input();
						entityManager->addComponent(player, i);
					}

					else if (componentName == "Movement")
					{
						Movement* m = new Movement();
						entityManager->addComponent(player, m);
					}
				}
			}
			//if the entity type is Player2
			else if (elemName == "Player2")
			{
				float xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale;
				xPos = yPos = zPos = xRot = yRot = zRot = 0.0f; 
				xScale = yScale = zScale = 1.0f;
				string model = "";
				bool isVisible = false;

				//Creating the Player entity and adding it to the entityManager.
				EntityId player2 = entityManager->createEntity("Player2", xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible);

				//Looping through all the components for Player2-entity.
				for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
				{
					string componentName = e->Value();
					if (componentName == "Monster")
					{
						attr = e->Attribute("name");

						string name = "";
						if (attr != NULL)
						{
							name = e->Attribute("name");


						}
						else
							return false;

						//create the Monster component and add that component to the Player entity
						Monster *m1 = new Monster(name);
						entityManager->addComponent(player2, m1);

					}

					else if (componentName == "Person")
					{
						attr = e->Attribute("name");
						string name = "";
						if (attr != NULL)
						{
							name = e->Attribute("name");
						}
						else
							return false;

						attr = e->Attribute("age");
						int age = 0;
						if (attr != NULL)
						{
							int *age2 = 0;
							age = e->IntAttribute("age");
						}
						else
							return false;

						//create the Person component and add that component to the Player entity
						Person *p1 = new Person(name, age);
						entityManager->addComponent(player2, p1);

					}

					else if (componentName == "Movement")
					{
						Movement* m = new Movement();
						entityManager->addComponent(player2, m);
					}
				}
			}
		}
	}
	else if (xmlFile == "platform.xml" || xmlFile == "platform2.xml")
	{
		//For every entity that is to be created in this EntityManager
		for (XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
		{
			string elemName = elem->Value();
			const char* attr;

			//if the entity type is Player
			if (elemName == "Platform")
			{
				float xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale;
				string model = "";
				bool isVisible = false;

				attr = elem->Attribute("xPos");
				if (attr != NULL)
				{
					xPos = elem->FloatAttribute("xPos");
				}

				attr = elem->Attribute("yPos");
				if (attr != NULL)
				{
					yPos = elem->FloatAttribute("yPos");
				}

				attr = elem->Attribute("zPos");
				if (attr != NULL)
				{
					zPos = elem->FloatAttribute("zPos");
				}

				attr = elem->Attribute("xRot");
				if (attr != NULL)
				{
					xRot = elem->FloatAttribute("xRot");
				}

				attr = elem->Attribute("yRot");
				if (attr != NULL)
				{
					yRot = elem->FloatAttribute("yRot");
				}

				attr = elem->Attribute("zRot");
				if (attr != NULL)
				{
					zRot = elem->FloatAttribute("zRot");
				}

				attr = elem->Attribute("xScale");
				if (attr != NULL)
				{
					xScale = elem->FloatAttribute("xScale");
				}

				attr = elem->Attribute("yScale");
				if (attr != NULL)
				{
					yScale = elem->FloatAttribute("yScale");
				}

				attr = elem->Attribute("zScale");
				if (attr != NULL)
				{
					zScale = elem->FloatAttribute("zScale");
				}

				attr = elem->Attribute("model");
				if (attr != NULL)
				{
					model = elem->Attribute("model");
				}

				attr = elem->Attribute("isVisible");
				if (attr != NULL)
				{
					isVisible = elem->BoolAttribute("isVisible");
				}

				//Creating the Player entity and adding it to the entitymanager
				EntityId platform = entityManager->createEntity("Platform", xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible);


				//Looping through all the components for Player-entity.
				for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
				{
					string componentName = e->Value();
					if (componentName == "Platform")
					{
						bool isMovingUpDown = false;
						bool isMovingFrontBack = false;
						cout << "hittade en Platformkomponent!" << endl;

						attr = e->Attribute("isMovingUpDown");
						if (attr != NULL)
						{
							isMovingUpDown = e->BoolAttribute("isMovingUpDown");
						}

						attr = e->Attribute("isMovingFrontBack");
						if (attr != NULL)
						{
							isMovingFrontBack = e->BoolAttribute("isMovingFrontBack");
						}

						Platform* p = new Platform(isMovingUpDown, isMovingFrontBack);
						entityManager->addComponent(platform, p);
					}
					else if (componentName == "Movement")
					{
						Movement* m = new Movement();
						entityManager->addComponent(platform, m);
					}
					else if (componentName == "Collision")
					{
						Collision::GetInstance()->CreateCollisionInstance(model, Vec3(xPos, yPos, zPos));
					}
					else if (componentName == "Messenger")
					{
						Messenger *m = new Messenger();
						entityManager->addComponent(platform, m);
					}
				}
			}
		}
	}

	doc.Clear();

	return true;
}

Vec3 EntityManager::getEntityPos(EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			return this->fEntitys[i]->returnPos();
		}
	}

	return Vec3(0.0f, 0.0f, 0.0f);
}

Vec3 EntityManager::getEntityPos(string type)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getType() == type)
		{
			return this->fEntitys[i]->returnPos();
		}
	}

	return Vec3(0.0f, 0.0f, 0.0f);
}

Vec3 EntityManager::getEntityRot(EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			return this->fEntitys[i]->returnRot();
		}
	}
}
Vec3 EntityManager::getEntityScale(EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			return this->fEntitys[i]->returnScale();
		}
	}
}
bool EntityManager::getEntityVisibility(EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			return this->fEntitys[i]->returnVisible();
		}
	}
}

void EntityManager::updateEntityPos(Vec3 pos, EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updatePos(pos);
		}
	}
}

void EntityManager::updateEntityRot(Vec3 rot, EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updateRot(rot);
		}
	}
}
void EntityManager::updateEntityScale(Vec3 scale, EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updateScale(scale);
		}
	}
}
void EntityManager::updateEntityVisibility(bool isVisible, EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updateVisible(isVisible);
		}
	}
}



