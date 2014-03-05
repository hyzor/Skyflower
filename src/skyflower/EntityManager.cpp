
#include "EntityManager.h"
#include "ComponentHeaders.h"

#include <iostream>
#include <cassert>
#include "shared/Vec3.h"

// Must be included last!
#include "shared/debug.h"

using namespace Cistron;
using namespace tinyxml2;

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
		if (entity && entity->getIsActive())
		{
			entity->update(deltaTime);
			entity->updatePos(entity->returnPos());
		}
	}
}

// generate a unique request id or return one if it already exists
RequestId EntityManager::getMessageRequestId(ComponentRequestType type, const string &name) {

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
	float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated, bool isSorted) {

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
	Entity *obj = new Entity(this, modules, id, relativeid, type, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated, isSorted);
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
					fRequiredComponents[(EntityId)i].push_back(req.name);
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

void EntityManager::sendGlobalMessage(string msg)
{
	for (auto it = fEntitys.begin(); it != fEntitys.end(); ++it)
	{
		(*it)->sendMessage(msg);
	}
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


void EntityManager::removeEntity(Entity* e) {

	
	int index = -1;
	for (int i = 0; i < (int)fEntitys.size(); i++)
	{
		if (fEntitys[i] == e)
		{
			index = i;
			break;
		}
	}

	list<Component*> comps = e->getComponents();
	for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it)
		destroyComponent(*it);

	if(index != -1)
		fEntitys.erase(fEntitys.begin() + index);

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
	comp->getOwner()->removeComponent(comp);

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
		comp->getOwner()->sendMessage(reqId, msg);

		// release the lock
		releaseLock(reqId);
	}

	// make it invalid
	delete comp;
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

		
		// The element is an entity.
		string entityName = GetStringAttribute(elem, "type", "", xmlFile);
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
		bool isSorted = GetBoolAttribute(elem, "isSorted", entityName, xmlFile);

		//Creating the entity and adding it to the entitymanager
		EntityId entity = this->createEntity(entityName, id, relativeid, xPos, yPos, zPos, xRot, yRot, zRot, xScale, yScale, zScale, model, isVisible, isCollidible, isAnimated, isSorted);

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
				bool toggle = false;
				attr = e->Attribute("toggle");
				if (attr != nullptr)
					toggle = e->FloatAttribute("toggle") == 1.0f;

				Button* btn = new Button(toggle);
				this->addComponent(entity, btn);
			}
			else if (componentName == "WallButton")
			{
				Vec3 dir;
				attr = e->Attribute("xDir");
				if (attr != nullptr)
					dir.X = e->FloatAttribute("xDir");
				attr = e->Attribute("zDir");
				if (attr != nullptr)
					dir.Z = e->FloatAttribute("zDir");

				WallButton* btn = new WallButton(dir);
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
				float minDist = 0;
				attr = e->Attribute("minDist");
				if (attr != nullptr)
					minDist = e->FloatAttribute("minDist");

				float speed = GetFloatAttribute(e, "speed", entityName, xmlFile, componentName);

				BoxComp* p = new BoxComp(speed, minDist);
				this->addComponent(entity, p);
			}
			else if (componentName == "Goal")
			{
				Goal* g = new Goal();
				this->addComponent(entity, g);
			}
			else if (componentName == "Throw")
			{
				bool haveAim = false;

				attr = e->Attribute("haveAim");
				if (attr != nullptr)
					haveAim = e->BoolAttribute("haveAim");

				Throw *t = new Throw(haveAim);
				this->addComponent(entity, t);
			}
			else if (componentName == "Throwable")
			{
				Throwable *t = new Throwable();
				this->addComponent(entity, t);
			}
			else if (componentName == "Touch")
			{
				Touch* t = new Touch();
				this->addComponent(entity, t);
			}
			else if (componentName == "PointLight")
			{
				Vec3 rpos;
				Vec3 color;
				float intensity = 1.0f;

				attr = e->Attribute("xPos");
				if (attr != nullptr)
					rpos.X = e->FloatAttribute("xPos");

				attr = e->Attribute("yPos");
				if (attr != nullptr)
					rpos.Y = e->FloatAttribute("yPos");

				attr = e->Attribute("zPos");
				if (attr != nullptr)
					rpos.Z = e->FloatAttribute("zPos");

				attr = e->Attribute("r");
				if (attr != nullptr)
					color.X = e->FloatAttribute("r");

				attr = e->Attribute("g");
				if (attr != nullptr)
					color.Y = e->FloatAttribute("g");

				attr = e->Attribute("b");
				if (attr != nullptr)
					color.Z = e->FloatAttribute("b");

				attr = e->Attribute("intensity");
				if (attr != nullptr)
					intensity = e->FloatAttribute("intensity");

				PointLightComp* plc = new PointLightComp(rpos, color, intensity);
				this->addComponent(entity, plc);
			}
			else if (componentName == "SpotLight")
			{
				Vec3 rpos;
				Vec3 color;
				Vec3 dir;
				float angle = 1.0f;

				attr = e->Attribute("xPos");
				if (attr != nullptr)
					rpos.X = e->FloatAttribute("xPos");

				attr = e->Attribute("yPos");
				if (attr != nullptr)
					rpos.Y = e->FloatAttribute("yPos");

				attr = e->Attribute("zPos");
				if (attr != nullptr)
					rpos.Z = e->FloatAttribute("zPos");
					


				attr = e->Attribute("xDir");
				if (attr != nullptr)
					dir.X = e->FloatAttribute("xDir");

				attr = e->Attribute("yDir");
				if (attr != nullptr)
					dir.Y = e->FloatAttribute("yDir");

				attr = e->Attribute("zDir");
				if (attr != nullptr)
					dir.Z = e->FloatAttribute("zDir");



				attr = e->Attribute("r");
				if (attr != nullptr)
					color.X = e->FloatAttribute("r");

				attr = e->Attribute("g");
				if (attr != nullptr)
					color.Y = e->FloatAttribute("g");

				attr = e->Attribute("b");
				if (attr != nullptr)
					color.Z = e->FloatAttribute("b");



				attr = e->Attribute("angle");
				if (attr != nullptr)
					angle = e->FloatAttribute("angle");



				SpotLightComp* slc = new SpotLightComp(rpos, color, angle, dir);
				this->addComponent(entity, slc);
			}
			else if (componentName == "DirectionalLight")
			{
				Vec3 color;
				Vec3 dir;
				float intensity = 1.0f;


				attr = e->Attribute("xDir");
				if (attr != nullptr)
					dir.X = e->FloatAttribute("xDir");

				attr = e->Attribute("yDir");
				if (attr != nullptr)
					dir.Y = e->FloatAttribute("yDir");

				attr = e->Attribute("zDir");
				if (attr != nullptr)
					dir.Z = e->FloatAttribute("zDir");



				attr = e->Attribute("r");
				if (attr != nullptr)
					color.X = e->FloatAttribute("r");

				attr = e->Attribute("g");
				if (attr != nullptr)
					color.Y = e->FloatAttribute("g");

				attr = e->Attribute("b");
				if (attr != nullptr)
					color.Z = e->FloatAttribute("b");

				DirectionalLightComp* dlc = new DirectionalLightComp(color, dir, intensity);
				this->addComponent(entity, dlc);
			}
			else if (componentName == "MorphAnimation")
			{
				string path = GetStringAttribute(e, "path", entityName, componentName);
				string file = GetStringAttribute(e, "file", entityName, componentName);

				MorphAnimation *m = new MorphAnimation(path, file);
				this->addComponent(entity, m);
			}
			else if (componentName == "Balloon")
			{
				Balloon* g = new Balloon();
				this->addComponent(entity, g);
			}
			else if (componentName == "HelpText")
			{
				string text = GetStringAttribute(e, "text", entityName, componentName);
				float range = GetFloatAttribute(e, "range", entityName, componentName);
				HelpText* h = new HelpText(text, range);
				this->addComponent(entity, h);
			}
			else if (componentName == "PortalEffect")
			{
				PortalEffectComponent *portalEffect = new PortalEffectComponent();
				this->addComponent(entity, portalEffect);
			}
			else if (componentName == "ParticleSystemComp")
			{
				string scriptName = GetStringAttribute(e, "script", entityName, componentName);
				UINT particleSystemID = GetIntAttribute(e, "particleSystemID", entityName, xmlFile, componentName);
				ParticleSystemComp* psc = new ParticleSystemComp(scriptName, particleSystemID);
				this->addComponent(entity, psc);
			}
			else
			{
				cout << xmlFile << ": Unknown component with name " << componentName << " in entity " << entityName << endl;
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

	/*stringstream ss;
	ss << "Failed to get Entity " << id << ": it does not exist!";
	error(ss);*/

	cout << "Failed to get Entity " << id << ": it does not exist!" << endl;

	return nullptr;
}

int EntityManager::getNrOfEntities()
{
	return (int)fEntitys.size();
}

EntityId EntityManager::getEntityId(int index)
{
	return fEntitys[index]->getEntityId();
}

Entity* EntityManager::getEntityByIndex(int index)
{
	return fEntitys[index];
}

void EntityManager::activateEntity(EntityId entityId)
{
	for (size_t j = 0; j < fEntitys.size(); j++)
	{
		if (this->fEntitys[j]->fId == entityId)
		{
			this->fEntitys[j]->setIsActive(true);
		}
	}
}

void EntityManager::deactivateEntity(EntityId entityId)
{
	for (size_t j = 0; j < fEntitys.size(); j++)
	{
		if (this->fEntitys[j]->fId == entityId)
		{
			this->fEntitys[j]->setIsActive(false);
		}
	}
}
