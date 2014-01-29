
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


static void ReportMissingAttribute(const std::string &name, const std::string &entityName, const std::string &fileName, const std::string &componentName = "")
{
	if (componentName.length() > 0) {
		cout << fileName << ": attribute " << name << " is missing from component " << componentName << " in entity " << entityName << endl;
	}
	else {
		cout << fileName << ": attribute " << name << " is missing from entity " << entityName << endl;
	}
}

static std::string GetStringAttribute(XMLElement *element, const std::string &name, const std::string &entityName, const std::string &fileName, const std::string &componentName = "")
{
	if (element->Attribute(name.c_str()) == NULL) {
		ReportMissingAttribute(name, entityName, fileName, componentName);
		return "";
	}

	return element->Attribute(name.c_str());
}

static float GetFloatAttribute(XMLElement *element, const std::string &name, const std::string &entityName, const std::string &fileName, const std::string &componentName = "")
{
	if (element->Attribute(name.c_str()) == NULL) {
		ReportMissingAttribute(name, entityName, fileName, componentName);
		return 0.0f;
	}

	return element->FloatAttribute(name.c_str());
}

static int GetIntAttribute(XMLElement *element, const std::string &name, const std::string &entityName, const std::string &fileName, const std::string &componentName = "")
{
	if (element->Attribute(name.c_str()) == NULL) {
		ReportMissingAttribute(name, entityName, fileName, componentName);
		return 0;
	}

	return element->IntAttribute(name.c_str());
}

static bool GetBoolAttribute(XMLElement *element, const std::string &name, const std::string &entityName, const std::string &fileName, const std::string &componentName = "")
{
	if (element->Attribute(name.c_str()) == NULL) {
		ReportMissingAttribute(name, entityName, fileName, componentName);
		return false;
	}

	return element->BoolAttribute(name.c_str());
}


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
		Entity *entity = (*iter);
		if (entity != NULL && entity->getIsActive())
			entity->update(deltaTime);
	}
	handleCollision();

	for (auto iter = this->fEntitys.begin(); iter != this->fEntitys.end(); iter++)
	{
		Entity *entity = (*iter);
		if (entity != NULL && entity->getIsActive())
			entity->updatePos(entity->returnPos());
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
EntityId EntityManager::createEntity(string type, int id, int relativeid, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
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
	Entity *obj = new Entity(modules, id, relativeid, type, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated);
	//cout << "Created Entity " << fIdCounter << endl;
	//++fIdCounter;

	// add it to the list
	fEntitys.push_back(obj);
	return id; //fIdCounter-1;
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
	int index = -1;
	for (int i = 0; i < (int)fEntitys.size(); i++)
	{
		if (fEntitys[i]->getEntityId() == id)
		{
			index = i;
			break;
		}
	}
	if (index==-1) 
	{
		stringstream ss;
		ss << "Failed to add component " << component->toString() << " to Entity " << id << ": it does not exist!";
		error(ss);
	}
	else
	{

		// can only add once
		if (component->fEntityManager != 0) {
			stringstream ss;
			ss << "Component is already part of an EntityManager. You cannot add a component twice.";
			error(ss);
		}

		// we get the appropriate Entity
		Entity *obj = fEntitys[index];

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
			getEntity(reg.component->getOwnerId())->registerRequest(reqId, reg);
		}

		// we add it to the associative map (used at destruction of the component to look up all its requests)
		fRequestsByComponentId[reg.component->getId()].push_back(req);

		// if the request is required and the Entity isn't finalized yet, we add it to a special list
		EntityId objId = reg.component->getOwnerId();
		for (size_t i = 0; i < fEntitys.size(); i++)
		{
			if (fEntitys[i]->getEntityId() == objId)
			{
				if (reg.required && !fEntitys[i]->isFinalized()) {
					fRequiredComponents[i].push_back(req.name);
				}
				break;
			}
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

	int index = -1;
	for (int i = 0; i < (int)fEntitys.size(); i++)
	{
		if (fEntitys[i]->getEntityId() == id)
		{
			index = i;
			break;
		}
	}

	// Entity doesn't exist
	if (index == -1) 
	{
		stringstream ss;
		ss << "Failed to destroy Entity " << id << ": it does not exist!";
		error(ss);
	}
	else
	{
		// destroy every component in the Entity
		list<Component*> comps = fEntitys[index]->getComponents();
		for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
			destroyComponent(*it);
		}

		// delete the actual Entity
		//cout << "Destroyed Entity " << id << endl;
		delete fEntitys[index];
		fEntitys.erase(fEntitys.begin() + index);
		//fIdCounter--;
	}

	
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
	getEntity(comp->getOwnerId())->removeComponent(comp);

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
		getEntity(comp->getOwnerId())->sendMessage(reqId, msg);

		// release the lock
		releaseLock(reqId);
	}

	// make it invalid
	delete comp;
}


// finalize an Entity
void EntityManager::finalizeEntity(EntityId id) {

	int index = -1;
	for (int i = 0; i < (int)fEntitys.size(); i++)
	{
		if (fEntitys[i]->getEntityId() == id)
		{
			index = i;
			break;
		}
	}

	// Entity doesn't exist
	if (index == -1)
	{
		stringstream ss;
		ss << "Failed to destroy Entity " << id << ": it does not exist!";
		error(ss);
	}
	else
	{
		// finalize the Entity itself
		fEntitys[index]->finalize();

		// see if there are any requirements
		if (fRequiredComponents.find(id) == fRequiredComponents.end()) return;

		// there are, do the checklist
		list<string> requiredComponents = fRequiredComponents[index];
		bool destroyEntity = false;
		for (list<string>::iterator it = requiredComponents.begin(); it != requiredComponents.end(); ++it) {

			// get the components of this type
			list<Component*> comps = fEntitys[index]->getComponents(*it);

			// if there are none, we want this Entity dead!
			if (comps.size() == 0) destroyEntity = true;
		}
		/*if (!destroyEntity) cout << "Finalized Entity " << id << " succesfully!" << endl;
		else cout << "Finalize on Entity " << id << " failed, destroying..." << endl;*/
		// we destroy the Entity if we didn't find what we needed
		if (destroyEntity) this->destroyEntity(id);
	}
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
		getEntity(component->getOwnerId())->trackRequest(reqId, component);


	}

	// if local, forward to Entity
	else {
		getEntity(component->getOwnerId())->trackRequest(reqId, component);
	}
}


void EntityManager::sendMessageToAllEntities(string message)
{
	for (size_t i = 0; i < fEntitys.size(); i++)
	{
		this->fEntitys[i]->sendAMessageToAll(message);
	}
}

void EntityManager::sendMessageToEntity(string message, string entity)
{
	for (size_t i = 0; i < fEntitys.size(); i++)
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
		cout << "Failed to load file: " << xmlFile << ", no root element." << endl;
		doc.Clear();
		return false;
	}

	//For every entity that is to be created in this EntityManager
	for (XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string elemName = elem->Value();
		const char* attr;

		// Make the element name lower case.
		std::transform(elemName.begin(), elemName.end(), elemName.begin(), ::tolower);

		if (elemName == "light")
		{
			// The element is a light.

			for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
			{
				string componentName = e->Value();

				float xPos, yPos, zPos, dirx, diry, dirz, intensity, coneAngle;
				xPos = yPos = zPos = dirx = diry = dirz = intensity = coneAngle = 0;

				// Mandatory attribues.
				float r = GetFloatAttribute(e, "r", elemName, xmlFile);
				float g = GetFloatAttribute(e, "g", elemName, xmlFile);
				float b = GetFloatAttribute(e, "b", elemName, xmlFile);

				// Optional attribues depending on type of light.
				attr = e->Attribute("xPos");
				if (attr != NULL)
					xPos = e->FloatAttribute("xPos");

				attr = e->Attribute("yPos");
				if (attr != NULL)
					yPos = e->FloatAttribute("yPos");

				attr = e->Attribute("zPos");
				if (attr != NULL)
					zPos = e->FloatAttribute("zPos");

				attr = e->Attribute("dirx");
				if (attr != NULL)
					dirx = e->FloatAttribute("dirx");

				attr = e->Attribute("diry");
				if (attr != NULL)
					diry = e->FloatAttribute("diry");

				attr = e->Attribute("dirz");
				if (attr != NULL)
					dirz = e->FloatAttribute("dirz");

				attr = e->Attribute("intensity");
				if (attr != NULL)
					intensity = e->FloatAttribute("intensity");

				attr = e->Attribute("coneAngle");
				if (attr != NULL)
					coneAngle = e->FloatAttribute("coneAngle");

				if (componentName == "spotLight")
					modules->graphics->addSpotLight(Vec3(r, g, b), Vec3(dirx, diry, dirz), Vec3(xPos, yPos, zPos), coneAngle);
				else if (componentName == "dirLight")
					modules->graphics->addDirLight(Vec3(r, g, b), Vec3(dirx, diry, dirz), intensity);
				else if (componentName == "pointLight")
					modules->graphics->addPointLight(Vec3(r, g, b), Vec3(xPos, yPos, zPos), intensity);
			}
		}
		else
		{
			// The element is an entity.
			string entityName = GetStringAttribute(elem, "entityName", "", xmlFile);
			int id = GetIntAttribute(elem, "id", entityName, xmlFile);

			if (id == 0)
			{
				cout << "Error: id can not be 0 for entity " << entityName << " in file " << xmlFile << endl;
				continue;
			}
			else
			{
				for (size_t i = 0; i < fEntitys.size(); i++)
				{
					if (fEntitys[i]->getEntityId() == id)
					{
						cout << "Error: entity " << entityName << " is sharing id " << id << " with entity " << fEntitys[i]->getType() << " in file " << xmlFile << endl;
						continue;
					}
				}
			}
			
			int relativeid = 0;
			attr = elem->Attribute("relativeid");
			if (attr != NULL)
				relativeid = elem->IntAttribute("relativeid");

			float xPos = GetFloatAttribute(elem, "xPos", entityName, xmlFile);
			float yPos = GetFloatAttribute(elem, "yPos", entityName, xmlFile);
			float zPos = GetFloatAttribute(elem, "zPos", entityName, xmlFile);

			float xRot = GetFloatAttribute(elem, "xRot", entityName, xmlFile);
			float yRot = GetFloatAttribute(elem, "yRot", entityName, xmlFile);
			float zRot = GetFloatAttribute(elem, "zRot", entityName, xmlFile);

			float xScale = GetFloatAttribute(elem, "xScale", entityName, xmlFile);
			float yScale = GetFloatAttribute(elem, "yScale", entityName, xmlFile);
			float zScale = GetFloatAttribute(elem, "zScale", entityName, xmlFile);

			string model = GetStringAttribute(elem, "model", entityName, xmlFile);
			bool isVisible = GetBoolAttribute(elem, "isVisible", entityName, xmlFile);
			bool isCollidible = GetBoolAttribute(elem, "isCollidible", entityName, xmlFile);
			bool isAnimated = GetBoolAttribute(elem, "isAnimated", entityName, xmlFile);

			//Creating the entity and adding it to the entitymanager
			EntityId entity = this->createEntity(entityName, id, relativeid, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated);

			//Looping through all the components for the entity.
			for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
			{
				string componentName = e->Value();

				if (componentName == "MoveTarget")
				{
					float targetPosX = GetFloatAttribute(e, "targetPosX", entityName, xmlFile, componentName);
					float targetPosY = GetFloatAttribute(e, "targetPosY", entityName, xmlFile, componentName);
					float targetPosZ = GetFloatAttribute(e, "targetPosZ", entityName, xmlFile, componentName);
					float duration = GetFloatAttribute(e, "duration", entityName, xmlFile, componentName);
					float easingPower = GetFloatAttribute(e, "easingPower", entityName, xmlFile, componentName);
					bool continuous = GetBoolAttribute(e, "continuous", entityName, xmlFile, componentName);

					MoveTargetComponent *component = new MoveTargetComponent(Vec3(xPos, yPos, zPos), Vec3(targetPosX, targetPosY, targetPosZ), duration, easingPower, continuous);
					this->addComponent(entity, component);
				}
				else if (componentName == "Rotating")
				{
					float yawSpeed = GetFloatAttribute(e, "yawSpeed", entityName, xmlFile, componentName);
					float pitchSpeed = GetFloatAttribute(e, "pitchSpeed", entityName, xmlFile, componentName);
					float rollSpeed = GetFloatAttribute(e, "rollSpeed", entityName, xmlFile, componentName);

					RotatingComponent *component = new RotatingComponent(yawSpeed, pitchSpeed, rollSpeed);
					this->addComponent(entity, component);
				}
				else if (componentName == "Movement")
				{
					float speed = GetFloatAttribute(e, "speed", entityName, xmlFile, componentName);

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
					int maxHP = 0;

					attr = e->Attribute("maxHP");
					if (attr != NULL)
						maxHP = e->IntAttribute("maxHP");

					Health* hp = new Health(maxHP);
					this->addComponent(entity, hp);
				}
				else if (componentName == "Event")
				{
					std::string script = GetStringAttribute(e, "script", entityName, xmlFile, componentName);

					Event* ev = new Event(script);
					this->addComponent(entity, ev);
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
				else if (componentName == "Box")
				{
					float speed = GetFloatAttribute(e, "speed", entityName, xmlFile, componentName);

					BoxComp* p = new BoxComp(speed);
					this->addComponent(entity, p);
				}
				else if (componentName == "Goal")
				{
					Goal *g = new Goal();
					this->addComponent(entity, g);
				}
				else
				{
					cout << xmlFile << ": Unknown component with name " << componentName << " in entity " << entityName << endl;
				}
			}
		}
	}


	for (size_t i = 0; i < fEntitys.size(); i++)
	{
		if (fEntitys[i]->relativeid != 0)
		{
			fEntitys[i]->changeRelative(getEntity(fEntitys[i]->relativeid));// = getEntity(fEntitys[i]->relativeid);
		}
	}


	return true;
}

Vec3 EntityManager::getEntityPos(EntityId ownerId)
{
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < this->fEntitys.size(); i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updatePos(pos);
		}
	}
}

void EntityManager::updateEntityRot(Vec3 rot, EntityId ownerId)
{
	for (size_t i = 0; i < this->fEntitys.size(); i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updateRot(rot);
		}
	}
}
void EntityManager::updateEntityScale(Vec3 scale, EntityId ownerId)
{
	for (size_t i = 0; i < this->fEntitys.size(); i++)
	{
		if (this->fEntitys[i]->getEntityId() == ownerId)
		{
			this->fEntitys[i]->updateScale(scale);
		}
	}
}
void EntityManager::updateEntityVisibility(bool isVisible, EntityId ownerId)
{
	for (size_t i = 0; i < this->fEntitys.size(); i++)
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
	for (size_t i = 0; i < fEntitys.size(); i++)
	{
		if (fEntitys.at(i)->getEntityId() == id)
		{
			return fEntitys.at(i);
		}
	}

	stringstream ss;
	ss << "Failed to get Entity " << id << ": it does not exist!";
	error(ss);
	return nullptr;
}

EntityId EntityManager::getNrOfEntities()
{
	return fEntitys.size();
}

EntityId EntityManager::getEntityId(int index)
{
	return fEntitys[index]->getEntityId();
}

void EntityManager::handleCollision()
{
	for (int i = 0; i < (int)fEntitys.size(); i++)
	{		
		fEntitys[i]->wall = nullptr;
		if (fEntitys[i]->hasComponents("Gravity"))
		{
			vector<Ray> groundRays;
			vector<Ray> wallRays;

			if (fEntitys[i]->collInst)
			{
				Box bounds = fEntitys[i]->collInst->GetBox();
				
				Box small = bounds;
				small.Position += Vec3(2, 2, 2);
				small.Size -= Vec3(2, 2, 2) * 2;

				//ground rays
				int amountX = (int)(small.Size.X / 1.5f);
				for (int kx = 1; kx <= amountX; kx++)
				{
					int amountZ = (int)(small.Size.Z / 1.5f);
					for (int kz = 1; kz <= amountZ; kz++)
					{
						Vec3 p = small.Position;
						p.Y = bounds.Size.Y;
						p.X = -small.Size.X/2 + (small.Size.X / (amountX + 1))*kx;
						p.Z = -small.Size.Z/2 + (small.Size.Z / (amountZ + 1))*kz;

						groundRays.push_back(Ray(p, Vec3(0, -bounds.Size.Y, 0)));
					}
				}

				//wall rays
				int amountY = (int)(small.Size.Y / 2);
				for (int ky = 1; ky <= amountY; ky++)
				{
					float pY = small.Position.Y + (small.Size.Y / (amountY + 1))*ky;

					int amountX = (int)(small.Size.X / 2);
					for (int kx = 1; kx <= amountX; kx++)
					{
						float pX = -small.Size.X / 2 + (small.Size.X / (amountX + 1))*kx;
						float pZ = -bounds.Size.Z / 2;

						wallRays.push_back(Ray(Vec3(pX, pY, pZ), Vec3(0, 0, bounds.Size.Z)));
					}

					int amountZ = (int)(small.Size.Z / 2);
					for (int kz = 1; kz <= amountZ; kz++)
					{
						float pZ = -small.Size.Z / 2 + (small.Size.Z / (amountZ + 1))*kz;
						float pX = -bounds.Size.X / 2;

						wallRays.push_back(Ray(Vec3(pX, pY, pZ), Vec3(bounds.Size.X, 0, 0)));
					}
				}
			}
			else
			{
				//body
				groundRays.push_back(Ray(Vec3(0, 15, 0), Vec3(0, -15, 0)));

				//feet
				wallRays.push_back(Ray(Vec3(-3, 3, 0), Vec3(6, 0, 0))); // test left and right at feet
				wallRays.push_back(Ray(Vec3(0, 3, -3), Vec3(0, 0, 6))); // test front and back at feet
				wallRays.push_back(Ray(Vec3(-3 * 0.71f, 3, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
				wallRays.push_back(Ray(Vec3(-3 * 0.71f, 3, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test

				//head
				wallRays.push_back(Ray(Vec3(-3, 13, 0), Vec3(6, 0, 0))); // test left and right at head
				wallRays.push_back(Ray(Vec3(0, 13, -3), Vec3(0, 0, 6))); // test front and back at head
				wallRays.push_back(Ray(Vec3(-3 * 0.71f, 13, -3 * 0.71f), Vec3(6 * 0.71f, 0, 6 * 0.71f))); // extra test
				wallRays.push_back(Ray(Vec3(-3 * 0.71f, 13, 3 * 0.71f), Vec3(6 * 0.71f, 0, -6 * 0.71f))); // extra test
			}

			Entity *ground = nullptr;
			for (int k = 0; k < (int)groundRays.size(); k++)
			{
				float t = testMove(groundRays[k], fEntitys[i], ground); //test feet and head
				//reset jump
				if (t == -1)
				{
					Vec3 vel = fEntitys[i]->mPhysicsEntity->getVelocity();
					if (vel.Y < 0)
						vel.Y = 0;
					fEntitys[i]->mPhysicsEntity->setVelocity(vel);
					fEntitys[i]->mPhysicsEntity->setJumping(false);

				}
				if (t == 1)
				{
					fEntitys[i]->mPhysicsEntity->setVelocity(Vec3());
					ground = nullptr;
				}
			}
			fEntitys[i]->changeRelative(ground);


			for (size_t k = 0; k < wallRays.size(); k++)
				testMove(wallRays[k], fEntitys[i], fEntitys[i]->wall);
			


			//activate event for ground
			if (fEntitys[i]->ground)
			{
				fEntitys[i]->ground->sendMessageToEntity("Ground", fEntitys[i]->ground->fId);
				//so that you can't jump while falling from something
				fEntitys[i]->sendMessageToEntity("notInAir", this->fEntitys[i]->getEntityId());
			}
			else
			{
				fEntitys[i]->sendMessageToEntity("inAir", this->fEntitys[i]->getEntityId());
			}

			//activate event for wall
			if (fEntitys[i]->wall)
				fEntitys[i]->wall->sendMessageToEntity("Wall", fEntitys[i]->wall->fId);



			
		}
	}
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
	for (size_t j = 0; j < fEntitys.size(); j++)
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
	for (size_t i = 0; i < fEntitys.size(); i++)
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

void EntityManager::activateEntity(int entityIndex)
{
	this->fEntitys[entityIndex]->setIsActive(true);
}

void EntityManager::deactivateEntity(int entityIndex)
{
	this->fEntitys[entityIndex]->setIsActive(false);
}
