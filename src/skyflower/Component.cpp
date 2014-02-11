
#include "Component.h"
#include "EntityManager.h"
#include "Cistron.h"

#include <sstream>
#include <iostream>

// Must be included last!
#include "shared/debug.h"

using namespace Cistron;
using std::stringstream;
using namespace std;


// constructor/destructor
Component::Component(string name) : fOwnerId(-1), fName(name), fDestroyed(false), fTrack(false), fEntityManager(0) {
	static ComponentId IdCounter = 0;
	fId = ++IdCounter;
	//this->active = false;
}
Component::~Component() {
}

// check for validity of the Entity
bool Component::isValid() {
	return fOwnerId >= 0 && fName.size() > 0 && !fDestroyed;
}

//return if the component is active or not
bool Component::isActive()
{
	return this->active;
}

void Component::setActive(bool status)
{
	this->active = status;
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
void Component::sendLocalMessage(string msg, Payload payload) {
	fEntityManager->sendMessageToEntity(msg, this, fOwnerId, payload);
}
void Component::sendMessageToEntity(EntityId id, string msg, Payload payload) {
	fEntityManager->sendMessageToEntity(msg, this, id, payload);
}


// called when added to an Entity
void Component::addedToEntity() {
	// does nothing by default
}

void Component::removeFromEntity()
{
	// does nothing by default
}

void Component::update(float deltaTime)
{
	// does nothing by default
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