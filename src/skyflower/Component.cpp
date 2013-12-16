
#include "Component.h"
#include "EntityManager.h"
#include "Cistron.h"

using namespace Cistron;


#include <sstream>
#include <iostream>

using std::stringstream;
using namespace std;


// constructor/destructor
Component::Component(string name) : fOwnerId(-1), fName(name), fDestroyed(false), fTrack(false), fEntityManager(0) {
	static ComponentId IdCounter = 0;
	fId = ++IdCounter;
}
Component::~Component() {
}


// track activity in the log
/*void Component::trackMe(track) {
	fTrack = track;
}*/


// check for validity of the Entity
bool Component::isValid() {
	return fOwnerId >= 0 && fName.size() > 0 && !fDestroyed;
}



// get owner
EntityId Component::getOwnerId() {
	return fOwnerId;
}

Entity *Component::getOwner()
{
	return fOwner;
}

void Component::setOwnerId(EntityId id) {

	// set the owner
	fOwnerId = id;

	// respond to ping requests
	requestMessage("ping", &Component::processPing);
}

void Component::setOwner(Entity *owner) {

	// set the owner
	fOwner = owner;

	// respond to ping requests
	requestMessage("ping", &Component::processPing);
}

void Component::setDestroyed() {
	fDestroyed = true;
}
bool Component::isDestroyed() {
	return fDestroyed;
}


// get id
ComponentId Component::getId() {
	return fId;
}
string Component::getName() {
	return fName;
}


/**
 * Entity/COMPONENT MODIFICATION FUNCTIONS
 */


// add another component to this Entity
void Component::addLocalComponent(Component *c) {
	fEntityManager->addComponent(getOwnerId(), c);
}


// add another component to another Entity
void Component::addComponent(EntityId objId, Component *c) {
	fEntityManager->addComponent(objId, c);
}


// create an Entity
EntityId Component::createEntity(string type) {
	return fEntityManager->createEntity(type, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "", false, false, false);
}

// finalize an Entity
void Component::finalizeEntity(EntityId objId) {
	fEntityManager->finalizeEntity(objId);
}

// destroy Entity
void Component::destroyEntity(EntityId objId) {
	fEntityManager->destroyEntity(objId);
}



/**
 * REQUEST FUNCTIONS
 */

// message request function
void Component::requestMessage(string message, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_MESSAGE;
	req.name = message;

	// forward to Entity manager
	fEntityManager->registerGlobalRequest(req, reg);
}

// require a component in this Entity
void Component::requireComponent(string name, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = true;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_COMPONENT;
	req.name = name;

	// forward to Entity manager
	fEntityManager->registerLocalRequest(req, reg);
}

// register a component request
void Component::requestComponent(string name, MessageFunction f, bool local) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_COMPONENT;
	req.name = name;

	// forward to Entity manager
	if (local) fEntityManager->registerLocalRequest(req, reg);
	else fEntityManager->registerGlobalRequest(req, reg);
}

// request all components of one type
void Component::requestAllExistingComponents(string name, MessageFunction f) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = f;
	reg.required = false;
	reg.component = this;
	reg.trackMe = false;

	// construct component request
	ComponentRequest req;
	req.type = REQ_ALLCOMPONENTS;
	req.name = name;

	// forward to Entity manager
	fEntityManager->registerGlobalRequest(req, reg);
}



// get a request id
RequestId Component::getMessageRequestId(string name) {
	return fEntityManager->getMessageRequestId(REQ_MESSAGE, name);
}


// request all components of a given type in a given Entity
list<Component*> Component::getComponents(string name, EntityId id) {
	if (id == -1) return fEntityManager->getComponents(getOwnerId(), name);
	else return fEntityManager->getComponents(id, name);
}


/**
 * MESSAGING FUNCTIONS
 */

// send a message
void Component::sendMessage(string msg, Payload payload) {
	fEntityManager->sendGlobalMessage(msg, this, payload);
}
void Component::sendMessage(RequestId reqId, Payload payload) {
	fEntityManager->sendGlobalMessage(reqId, this, payload);
}
void Component::sendLocalMessage(string msg, Payload payload) {
	fEntityManager->sendMessageToEntity(msg, this, fOwnerId, payload);
}
void Component::sendLocalMessage(RequestId reqId, Payload payload) {
	fEntityManager->sendMessageToEntity(reqId, this, fOwnerId, payload);
}
void Component::sendLocalMessage(RequestId reqId, Message const & msg) {
	fEntityManager->sendMessageToEntity(reqId, msg, fOwnerId);
}
void Component::sendMessageToEntity(EntityId id, string msg, Payload payload) {
	fEntityManager->sendMessageToEntity(msg, this, id, payload);
}
void Component::sendMessageToEntity(EntityId id, RequestId reqId, Payload payload) {
	fEntityManager->sendMessageToEntity(reqId, this, id, payload);
}
void Component::sendMessageToEntity(EntityId id, RequestId reqId, Message const & msg) {
	fEntityManager->sendMessageToEntity(reqId, msg, id);
}



// called when added to an Entity
void Component::addedToEntity() {
	// does nothing by default
}

void Component::removeFromEntity()
{
}

// register a unique name
void Component::registerName(string s) {
	fEntityManager->registerName(this, s);
}
void Component::unregisterName(string s) {
	fEntityManager->unregisterName(this, s);
}
list<Component*> Component::getComponentsByName(string name) {
	return fEntityManager->getComponentsByName(name);
}


// destroy this component
void Component::destroy() {
	fEntityManager->destroyComponent(this);
}


// output
ostream& operator<<(ostream& s, Component &v) {
	return s << "Component[" << v.getId() << "][" << v.getName() << "] owned by [" << v.getOwnerId() << "]";
};


// to string
string Component::toString() {
	stringstream ss;
	ss << (*this);
	return ss.str();
}


/**
 * PING & LOGGING
 */


// process ping
void Component::processPing(Message const & msg) {
	cout << *this << " PING" << endl;
}


// track a component request
void Component::trackComponentRequest(string name, bool local) {

	// get request id
	RequestId reqId = fEntityManager->getMessageRequestId(REQ_COMPONENT, name);

	// start tracking
	fEntityManager->trackRequest(reqId, local, this);
}


// track a message request
void Component::trackMessageRequest(string message) {

	// get request id
	RequestId reqId = fEntityManager->getMessageRequestId(REQ_MESSAGE, message);

	// start tracking
	fEntityManager->trackRequest(reqId, false, this);
}

Vec3 Component::getEntityPos()
{
	return this->fEntityManager->getEntityPos(this->fOwnerId);
}

Vec3 Component::getEntityRot()
{
	return this->fEntityManager->getEntityRot(this->fOwnerId);
}
Vec3 Component::getEntityScale()
{
	return this->fEntityManager->getEntityScale(this->fOwnerId);
}
bool Component::getEntityVisibility()
{
	return this->fEntityManager->getEntityVisibility(this->fOwnerId);
}
CollisionInstance* Component::getEntityCollision()
{
	return this->fEntityManager->getEntityCollision(this->fOwnerId);
}

void Component::updateEntityPos(Vec3 pos)
{
	this->fEntityManager->updateEntityPos(pos, this->fOwnerId);
}

void Component::updateEntityRot(Vec3 rot)
{
	this->fEntityManager->updateEntityRot(rot, this->fOwnerId);
}
void Component::updateEntityScale(Vec3 scale)
{
	this->fEntityManager->updateEntityScale(scale, this->fOwnerId);
}
void Component::updateEntityVisibility(bool isVisible)
{
	this->fEntityManager->updateEntityVisibility(isVisible, this->fOwnerId);
}