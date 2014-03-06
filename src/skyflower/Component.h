
#ifndef INC_COMPONENT
#define INC_COMPONENT


#include <string>
#include <map>
#include <list>
#include <functional>
//#include <boost/any.hpp>
#include <ostream>
#include "Cistron.h"
#include "physics/Collision.h"
#include "shared/Vec3.h"


namespace Cistron {

using std::list;
using std::map;
using std::string;
using std::ostream;

using namespace std::placeholders;


// a request ID
typedef int RequestId;

// type of component requests
enum ComponentRequestType {
	REQ_COMPONENT = 0,
	REQ_MESSAGE = 1,
	REQ_ALLCOMPONENTS = 2
};

// a request
struct ComponentRequest {
	ComponentRequestType type;
	string name;
	ComponentRequest(ComponentRequestType t, string n) : type(t), name(n) {};
	ComponentRequest() {};
};




// event type
enum MessageType {
	CREATE,
	DESTROY,
	MESSAGE
};


// an event
class Component;
struct Message {
	MessageType type;
	Component *sender;
	Payload p;
	Message(MessageType t) : type(t) {};
	Message(MessageType t, Component *c) : type(t), sender(c) {};
	Message(MessageType t, Component *c, Payload payload) : type(t), sender(c), p(payload) {};
};




// component function
typedef std::tr1::function<void(Message const &)> MessageFunction;


// a component registered for an event (message or component creation/destruction)
struct RegisteredComponent {
	Component *component;
	MessageFunction callback;
	bool required;
	bool trackMe;
	RegisteredComponent *next;
};

// Entity manager
class EntityManager;
class Entity;

// a generic component
class Component {

	public:

		// constructor/destructor
		Component(string name);
		virtual ~Component();

		// function called when the component is added to an Entity
		virtual void addedToEntity();
		virtual void removeFromEntity();
		virtual void update(float deltaTime);

		// get components
		list<Component*> getComponentsByName(string name);

		/**
		 * REQUEST FUNCTIONS
		 */

		// message request function
		void requestMessage(const string &message, MessageFunction handler);

		// require a component in this Entity
		void requireComponent(string name, MessageFunction);

		// register a component request
		void requestComponent(string name, MessageFunction, bool local = false);

		// request all components of one type
		void requestAllExistingComponents(string name, MessageFunction);

		// request all components of a given type in a given Entity
		list<Component*> getComponents(string name, EntityId id = -1);

		/**
		 * FANCY TEMPLATED REQUEST FUNCTIONS
		 */

		// message request function
		template<class T>
		void requestMessage(const string &message, void (T::*f)(Message const &));

		/**
		 * IMPLEMENTED REQUESTS & LOGGING
		 */
		void processPing(Message const &);

		void trackComponentRequest(string name, bool local = false);
		void trackMessageRequest(string message);

		// get Entity manager this component belongs to
		inline EntityManager* getEntityManager() {
			return fEntityManager;
		}

		// get owner
		EntityId getOwnerId();
		Entity *getOwner();

		// get id
		ComponentId getId();

		// destroy this component
		void destroy();
		bool isDestroyed();

		// valid component?
		bool isValid();

		bool isActive();
		void setActive(bool status);

		// get the name of the component
		string getName();

		// to string
		string toString();

	private:
		// set owner
		void setOwnerId(EntityId id);
		void setOwner(Entity *owner);

		// set destroyed
		void setDestroyed();

		// Entity id
		EntityId fOwnerId;
		Entity *fOwner;

		// Entity manager
		EntityManager *fEntityManager;

		// component it
		ComponentId fId;

		// name of the component
		string fName;
		string type;

		// destroyed
		bool fDestroyed;

		// track this component in the log
		bool fTrack;

		// Entity manager is our friend
		friend class EntityManager;

		bool active;

};

/**
 * TEMPLATED REQUEST FUNCTIONS
 */

// message request function
template<class T>
void Component::requestMessage(const string &message, void (T::*f)(Message const &)) {
	requestMessage(message, std::tr1::bind(f, (T*)(this), _1));
}

};


// output
std::ostream& operator<<(std::ostream& s, Cistron::Component &v);


#endif
