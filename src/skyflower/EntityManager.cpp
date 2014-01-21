
#include "Cistron.h"
#include "EntityManager.h"
#include "ComponentHeaders.h"

using namespace Cistron;
using namespace tinyxml2;
#include <iostream>
#include <cassert>
#include "shared/Vec3.h"

using std::cout;
using std::endl;


// constructor/destructor
EntityManager::EntityManager(const std::string &resourceDir, const Modules *modules) : fIdCounter(0), fRequestIdCounter(0), fNLocks(0) {

	m_resourceDir = resourceDir;
	this->modules = modules;
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

void EntityManager::update(float deltaTime)
{
	for (auto iter = this->fEntitys.begin(); iter != this->fEntitys.end(); iter++)
	{
		if ((*iter) != NULL)
			(*iter)->update(deltaTime);
	}
	handleCollision();
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
	float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated) {

	vector<Entity*> temp;
	for (unsigned int i = 0; i < fEntitys.size(); i++)
	{
		if (fEntitys.at(i) != NULL)
		{
			temp.push_back(fEntitys.at(i));
		}
	}
	fEntitys = temp;
	// create a new Entity
	Entity *obj = new Entity(modules, fIdCounter, type, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated);
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
	component->setOwnerId(id);
	component->setOwner(obj);

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
	fIdCounter--;
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

	comp->removeFromEntity();
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

void EntityManager::sendMessageToEntity(string message, EntityId entity)
{
	Entity* e = getEntity(entity);
	e->sendMessageToEntity(message, entity);
}


bool EntityManager::loadXML(string xmlFile)
{
	string path = m_resourceDir + xmlFile;
	XMLDocument doc;

	if (doc.LoadFile(path.c_str()) != XML_NO_ERROR)
	{
		cout << "Error reading/parsing file: " << xmlFile << endl;
		return false;
	}

	//Find the root element
	XMLElement* root = doc.FirstChildElement();
	if (root == NULL)
	{
		cout << "Failed to load file: " << xmlFile << " No root element." << endl;
		doc.Clear();
		return false;
	}

	//For every entity that is to be created in this EntityManager
	for (XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string elemName = elem->Value();
		const char* attr;

		float xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale;
		xPos = yPos = zPos = xRot = yRot = zRot = xScale = yScale = zScale = 0.0f;
		string model = "";
		string entityName = "";
		bool isVisible = false;
		bool isCollidible = false;
		bool isAnimated = false;

		//get all the attributes for the entity
		attr = elem->Attribute("entityName");
		if (attr != NULL)
		{
			entityName = elem->Attribute("entityName");
		}
		else
		{
			cout << "failed loading attribute for entityName in file " << xmlFile << endl;
		}

		attr = elem->Attribute("xPos");
		if (attr != NULL)
		{
			xPos = elem->FloatAttribute("xPos");
		}
		else
		{
			cout << "failed loading attribute for xPos for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("yPos");
		if (attr != NULL)
		{
			yPos = elem->FloatAttribute("yPos");
		}
		else
		{
			cout << "failed loading attribute for yPos for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("zPos");
		if (attr != NULL)
		{
			zPos = elem->FloatAttribute("zPos");
		}
		else
		{
			cout << "failed loading attribute for zPos for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("xRot");
		if (attr != NULL)
		{
			xRot = elem->FloatAttribute("xRot");
		}
		else
		{
			cout << "failed loading attribute for xRot for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("yRot");
		if (attr != NULL)
		{
			yRot = elem->FloatAttribute("yRot");
		}
		else
		{
			cout << "failed loading attribute for yRot for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("zRot");
		if (attr != NULL)
		{
			zRot = elem->FloatAttribute("zRot");
		}
		else
		{
			cout << "failed loading attribute for zRot for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("xScale");
		if (attr != NULL)
		{
			xScale = elem->FloatAttribute("xScale");
		}
		else
		{
			cout << "failed loading attribute for xScale for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("yScale");
		if (attr != NULL)
		{
			yScale = elem->FloatAttribute("yScale");
		}
		else
		{
			cout << "failed loading attribute for yScale for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("zScale");
		if (attr != NULL)
		{
			zScale = elem->FloatAttribute("zScale");
		}
		else
		{
			cout << "failed loading attribute for zScale for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("model");
		if (attr != NULL)
		{
			model = elem->Attribute("model");
		}
		else
		{
			cout << "failed loading attribute for model for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("isVisible");
		if (attr != NULL)
		{
			isVisible = elem->BoolAttribute("isVisible");
		}
		else
		{
			cout << "failed loading attribute for isVisible for entity " << entityName << " in file " << xmlFile << endl;
		}

		attr = elem->Attribute("isCollidible");
		if (attr != NULL)
		{
			isCollidible = elem->BoolAttribute("isCollidible");
		}
		else
		{
			cout << "failed loading attribute for isCollidible for entity " << entityName << " in file " << xmlFile << endl;
		}
		
		attr = elem->Attribute("isAnimated");
		if (attr != NULL)
		{
			isAnimated = elem->BoolAttribute("isAnimated");
		}
		else
		{
			cout << "failed loading attribute for isAnimated for entity " << entityName << " in file " << xmlFile << endl;
		}

		//Creating the entity and adding it to the entitymanager
		EntityId entity = this->createEntity(entityName, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated);

		//Looping through all the components for the entity.
		for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
		{
			string componentName = e->Value();

			if (componentName == "OscillatePosition")
			{
				float xDir = 0.0f;
				float yDir = 0.0f;
				float zDir = 0.0f;
				float speed = 0.0f;
				float travelDistance = 0.0f;

				attr = e->Attribute("xDir");
				if (attr != NULL)
					xDir = e->FloatAttribute("xDir");
				else
					cout << "failed loading attribute for xDir for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("yDir");
				if (attr != NULL)
					yDir = e->FloatAttribute("yDir");
				else
					cout << "failed loading attribute for yDir for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("zDir");
				if (attr != NULL)
					zDir = e->FloatAttribute("zDir");
				else
					cout << "failed loading attribute for zDir for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("speed");
				if (attr != NULL)
					speed = e->FloatAttribute("speed");
				else
					cout << "failed loading attribute for speed for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("travelDistance");
				if (attr != NULL)
					travelDistance = e->FloatAttribute("travelDistance");
				else
					cout << "failed loading attribute for speed for entity " << entityName << " in file " << xmlFile << endl;

				OscillatePositionComponent *component = new OscillatePositionComponent(Vec3(xDir, yDir, zDir), speed, travelDistance);
				this->addComponent(entity, component);
			}
			else if (componentName == "Rotating")
			{
				float yawSpeed = 0.0f;
				float pitchSpeed = 0.0f;
				float rollSpeed = 0.0f;

				attr = e->Attribute("yawSpeed");
				if (attr != NULL)
					yawSpeed = e->FloatAttribute("yawSpeed");
				else
					cout << "failed loading attribute for yawSpeed for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("pitchSpeed");
				if (attr != NULL)
					pitchSpeed = e->FloatAttribute("pitchSpeed");
				else
					cout << "failed loading attribute for pitchSpeed for entity " << entityName << " in file " << xmlFile << endl;

				attr = e->Attribute("rollSpeed");
				if (attr != NULL)
					rollSpeed = e->FloatAttribute("rollSpeed");
				else
					cout << "failed loading attribute for rollSpeed for entity " << entityName << " in file " << xmlFile << endl;

				RotatingComponent *component = new RotatingComponent(yawSpeed, pitchSpeed, rollSpeed);
				this->addComponent(entity, component);
			}
			else if (componentName == "Movement")
			{
				float speed = 50;
				attr = e->Attribute("speed");
				if (attr)
					speed = e->FloatAttribute("speed");
				Movement* m = new Movement(speed);
				this->addComponent(entity, m);
			}
			else if (componentName == "Gravity")
			{
				GravityComponent* m = new GravityComponent();
				this->addComponent(entity, m);
			}
			else if (componentName == "AI")
			{
				AI* m = new AI();
				this->addComponent(entity, m);
			}
			else if (componentName == "Listener")
			{
				ListenerComponent* m = new ListenerComponent();
				this->addComponent(entity, m);
			}
			else if (componentName == "Footsteps")
			{
				FootstepsComponent* m = new FootstepsComponent();
				this->addComponent(entity, m);
			}
			else if (componentName == "Messenger")
			{
				Messenger *m = new Messenger();
				this->addComponent(entity, m);
			}
			else if (componentName == "Input")
			{
				Input* i = new Input();
				this->addComponent(entity, i);
			}
			else if (componentName == "Health")
			{
				attr = e->Attribute("maxHP");
				int maxHP = 0;

				if (attr != NULL)
				{
					maxHP = e->IntAttribute("maxHP");
				}
				Health* hp = new Health(maxHP);
				this->addComponent(entity, hp);
			}
			else if (componentName == "Event")
			{
				attr = e->Attribute("script");
				if (attr != nullptr)
				{
					Event* ev = new Event(attr);
					this->addComponent(entity, ev);
				}
			}
			else if (componentName == "Button")
			{
				Button* btn = new Button();
				this->addComponent(entity, btn);
			}
			else if (componentName == "Checkpoint")
			{
				Vec3 spawnpoint = Vec3(xPos, yPos, zPos);

				attr = e->Attribute("xPos");
				if (attr != nullptr)
					spawnpoint.X = e->FloatAttribute("xPos");
				attr = e->Attribute("yPos");
				if (attr != nullptr)
					spawnpoint.Y = e->FloatAttribute("yPos");
				attr = e->Attribute("zPos");
				if (attr != nullptr)
					spawnpoint.Z = e->FloatAttribute("zPos");

				Checkpoint* cp = new Checkpoint(spawnpoint);
				this->addComponent(entity, cp);
			}
			else if (componentName.find("Light") != string::npos)
			{
				float intensity, r, g, b, dirx, diry, dirz, xPos, yPos, zPos, coneAngle;
				intensity = r = g = b = dirx = diry = dirz = xPos = yPos = zPos = coneAngle = 0;

				attr = e->Attribute("intensity");
				if (attr != NULL)
				{
					intensity = e->FloatAttribute("intensity");
				}
				attr = e->Attribute("r");
				if (attr != NULL)
				{
					r = e->FloatAttribute("r");
				}
				attr = e->Attribute("g");
				if (attr != NULL)
				{
					g = e->FloatAttribute("g");
				}
				attr = e->Attribute("b");
				if (attr != NULL)
				{
					b = e->FloatAttribute("b");
				}
				attr = e->Attribute("dirx");
				if (attr != NULL)
				{
					dirx = e->FloatAttribute("dirx");
				}
				attr = e->Attribute("diry");
				if (attr != NULL)
				{
					diry = e->FloatAttribute("diry");
				}
				attr = e->Attribute("dirz");
				if (attr != NULL)
				{
					dirz = e->FloatAttribute("dirz");
				}
				if (attr != NULL)
				{
					coneAngle = e->FloatAttribute("coneAngle");
				}
				attr = e->Attribute("xPos");
				if (attr != NULL)
				{
					xPos = e->FloatAttribute("xPos");
				}
				attr = e->Attribute("yPos");
				if (attr != NULL)
				{
					yPos = e->FloatAttribute("yPos");
				}
				attr = e->Attribute("zPos");
				if (attr != NULL)
				{
					zPos = e->FloatAttribute("zPos");
				}
				if (componentName == "spotLight")
					modules->graphics->addSpotLight(Vec3(r, g, b), Vec3(dirx, diry, dirz), Vec3(xPos, yPos, zPos), coneAngle);
				else if (componentName == "dirLight")
					modules->graphics->addDirLight(Vec3(r, g, b), Vec3(dirx, diry, dirz), intensity);
				else if (componentName == "pointLight")
					modules->graphics->addPointLight(Vec3(r, g, b), Vec3(xPos, yPos, zPos), intensity);
			}
			else if (componentName == "Push")
			{
				Push * p = new Push();
				this->addComponent(entity, p);
			}
			else if (componentName == "Pushable")
			{
				Pushable * p = new Pushable();
				this->addComponent(entity, p);
			}
			else
			{
				cout << "Unknown component with name " << componentName << " in entity " << entityName << " in file " << xmlFile << endl;
			}
		}
	}

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

	return Vec3(0.0f, 0.0f, 0.0f);
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

	return Vec3(1.0f, 1.0f, 1.0f);
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

	return false;
}

CollisionInstance* EntityManager::getEntityCollision(EntityId ownerId)
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			return this->fEntitys[i]->returnCollision();
		}
	}

	return NULL;
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

Component* EntityManager::getComponent(string EntityName, string Component)
{
	for (size_t i = 0; i < fEntitys.size(); i++)
	{
		if (fEntitys.at(i)->getType() == EntityName)
		{
			return fEntitys.at(i)->getComponents(Component).front();
		}
	}
	return NULL;
}

Entity *EntityManager::getEntity(EntityId id)
{
	// make sure the Entity exists
	if (id < 0 || id >= (int)fEntitys.size() || fEntitys[id] == 0) {
		stringstream ss;
		ss << "Failed to get Entity " << id << ": it does not exist!";
		error(ss);
	}

	return fEntitys[id];
}

EntityId EntityManager::getNrOfEntities()
{
	return this->fIdCounter;
}

void EntityManager::handleCollision()
{
	for (int i = 0; i < this->fIdCounter; i++)
	{		
		fEntitys[i]->ground = nullptr;
		fEntitys[i]->wall = nullptr;
		if (fEntitys[i]->hasComponents("Gravity"))
		{
			float t = testMove(Ray(Vec3(0, 15, 0), Vec3(0, -15, 0)), fEntitys[i], fEntitys[i]->ground); //test feet and head
			//reset jump
			if (t == -1)
			{
				fEntitys[i]->physics->setVelocity(Vec3());
				fEntitys[i]->physics->setJumping(false);
			}
			else if (t == 1)
			{
				fEntitys[i]->physics->setVelocity(Vec3());
				fEntitys[i]->ground = nullptr;
			}

			//feet
			testMove(Ray(Vec3(-3, 3, 0), Vec3(6, 0, 0)), fEntitys[i], fEntitys[i]->wall); // test left and right at feet
			testMove(Ray(Vec3(0, 3, -3), Vec3(0, 0, 6)), fEntitys[i], fEntitys[i]->wall); // test front and back at feet
			testMove(Ray(Vec3(-3 * 0.71f, 3, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f)), fEntitys[i], fEntitys[i]->wall); // extra test
			testMove(Ray(Vec3(-3 * 0.71f, 3, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f)), fEntitys[i], fEntitys[i]->wall); // extra test
			
			//head
			testMove(Ray(Vec3(-3, 13, 0), Vec3(6, 0, 0)), fEntitys[i], fEntitys[i]->wall); // test left and right at head
			testMove(Ray(Vec3(0, 13, -3), Vec3(0, 0, 6)), fEntitys[i], fEntitys[i]->wall); // test front and back at head
			testMove(Ray(Vec3(-3 * 0.71f, 13, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f)), fEntitys[i], fEntitys[i]->wall); // extra test
			testMove(Ray(Vec3(-3 * 0.71f, 13, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f)), fEntitys[i], fEntitys[i]->wall); // extra test


			//activate event for wall
			if (fEntitys[i]->ground)
				fEntitys[i]->ground->sendMessageToEntity("Ground", fEntitys[i]->ground->fId);
			if (fEntitys[i]->wall)
				fEntitys[i]->wall->sendMessageToEntity("Wall", fEntitys[i]->wall->fId);


			//collision and pushing between two entities
			for (int j = i + 1; j < this->fIdCounter; j++)
			{
				if (this->fEntitys[i]->sphere != NULL && this->fEntitys[j]->sphere != NULL)
				{
					bool collide;
					collide = this->fEntitys[i]->sphere->Test(this->fEntitys[i]->sphere, this->fEntitys[j]->sphere);

					//are two entities colliding?
					if (collide)
					{
						Vec3 dir;
						dir.X = this->fEntitys[j]->pos.X - this->fEntitys[i]->pos.X;
						dir.Y = this->fEntitys[j]->pos.Y - this->fEntitys[i]->pos.Y;
						dir.Z = this->fEntitys[j]->pos.Z - this->fEntitys[i]->pos.Z;

						if (this->fEntitys[i]->getType() == "player" || this->fEntitys[j]->getType() == "player")
						{
							//if "i" is moving and can push, and that "j" is pushable
							if (this->fEntitys[i]->physics->getIsMoving() && this->fEntitys[i]->hasComponents("Push") && this->fEntitys[j]->hasComponents("Pushable"))
							{
									pushEntity(j, dir);
							}
							//if "j" is moving and can push, and that "i" is pushable
							if (this->fEntitys[j]->physics->getIsMoving() && this->fEntitys[i]->hasComponents("Push") && this->fEntitys[i]->hasComponents("Pushable"))
							{
								dir = dir*-1;
								pushEntity(i, dir);
							}
						}
					}
				}
			}
		}
	}
}

void EntityManager::pushEntity(int entityIndex, Vec3 direction)
{
	this->fEntitys[entityIndex]->physics->setPushDirection(direction);
	this->fEntitys[entityIndex]->fComponents["Messenger"].front()->sendMessageToEntity(this->fEntitys[entityIndex]->getEntityId(), "beingPushed");
}


float EntityManager::testMove(Ray r, Entity* e)
{
	Entity* col;
	return testMove(r, e, col);
}

float EntityManager::testMove(Ray r, Entity* e, Entity* &out)
{
	//test ray relative to entity
	Vec3 pos = e->returnPos();
	r.Pos += pos;

	//test collision for other collidible entitis
	float col = 0;
	for (int j = 0; j < this->fIdCounter; j++)
	{
		if (fEntitys[j]->collInst && fEntitys[j] != e)
		{
			float t = fEntitys[j]->collInst->Test(r);
			if (t > 0)
			{
				if (col == 0 || t < col)
				{
					col = t;
					out = fEntitys[j];
				}
			}
		}
	}

	//collision detected
	float dir = 0;
	if (col > 0.5f) //feet
	{
		e->updatePos(pos - r.Dir*(1 - col));
		dir = -1;
	}
	else if (col > 0) //head
	{
		e->updatePos(pos + r.Dir*(col));
		dir = 1;
	}

	return dir;
}

//used for push-collisions
void EntityManager::createSphereOnEntities()
{
	for (int i = 0; i < this->fIdCounter; i++)
	{
		if (this->fEntitys[i]->getType() == "player")
		{
			Vec3 temp = getEntityPos("player");
			Sphere *s = new Sphere(temp.X, temp.Y, temp.Z);
		}
		else if (this->fEntitys[i]->getType() == "AI")
		{
			Vec3 temp = getEntityPos("AI");
			Sphere *s = new Sphere(temp.X, temp.Y, temp.Z);
		}
	}
}
