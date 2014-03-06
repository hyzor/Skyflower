
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
void Component::requestMessage(const string &message, MessageFunction handler) {

	// construct registered component
	RegisteredComponent reg;
	reg.callback = handler;
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