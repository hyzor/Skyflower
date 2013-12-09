
#ifndef INC_COMPONENT
#define INC_COMPONENT


#include <string>
#include <map>
#include <list>
#include <functional>
#include "Cistron.h"
//#include <boost/any.hpp>
#include <ostream>
#include "shared/Vec3.h"


namespace Cistron {

using std::list;
using std::map;
using std::string;
using std::ostream;

using namespace std::placeholders;


// Entity & component id
typedef int EntityId;
typedef int ComponentId;


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


// a generic component
class Component {

	public:

		// constructor/destructor
		Component(string name);
		virtual ~Component();

		// function called when the component is added to an Entity
		virtual void addedToEntity();

		// register a unique name for the Entity
		void registerName(string s);
		void unregisterName(string s);

		// get components
		list<Component*> getComponentsByName(string name);


		/**
		 * Entity/COMPONENT MODIFICATION FUNCTIONS
		 */

		// add another component to this Entity
		void addLocalComponent(Component*);

		// add a component to another Entity
		void addComponent(EntityId, Component*);

		// create an Entity
		EntityId createEntity(string type);

		// finalize an Entity
		void finalizeEntity(EntityId);

		// destroy Entity
		void destroyEntity(EntityId);


		/**
		 * REQUEST FUNCTIONS
		 */

		// message request function
		void requestMessage(string message, MessageFunction);

		// require a component in this Entity
		void requireComponent(string name, MessageFunction);

		// register a component request
		void requestComponent(string name, MessageFunction, bool local = false);

		// request all components of one type
		void requestAllExistingComponents(string name, MessageFunction);

		// request a request id of a message
		RequestId getMessageRequestId(string name);

		// request all components of a given type in a given Entity
		list<Component*> getComponents(string name, EntityId id = -1);

		/**
		 * FANCY TEMPLATED REQUEST FUNCTIONS
		 */

		// message request function
		template<class T>
		void requestMessage(string message, void (T::*f)(Message const &));

		// require a component in this Entity
		template<class T>
		void requireComponent(string name, void (T::*f)(Message const &));

		// register a component request
		template<class T>
		void requestComponent(string name, void (T::*f)(Message const &), bool local = false);

		// request all components of one type
		template<class T>
		void requestAllExistingComponents(string name, void (T::*f)(Message const &));

		// request all components of a given type and dynamic_cast to a particular class
		template<class T>
		list<T*> getComponents(string name, EntityId id = -1);

		/**
		 * MESSAGING FUNCTIONS
		 */

		// send a message
		void sendMessage(string msg, Payload  payload = 0);
		void sendMessage(RequestId id, Payload  payload = 0);
		void sendMessageToEntity(EntityId id, string msg, Payload payload = 0);
		void sendMessageToEntity(EntityId id, RequestId reqId, Payload payload = 0);
		void sendMessageToEntity(EntityId id, RequestId reqId, Message const & msg);
		void sendLocalMessage(string msg, Payload payload = 0);
		void sendLocalMessage(RequestId reqId, Payload payload = 0);
		void sendLocalMessage(RequestId reqId, Message const & msg);

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

		// get id
		ComponentId getId();

		// destroy this component
		void destroy();
		bool isDestroyed();

		// valid component?
		bool isValid();

		// get the name of the component
		string getName();

		// to string
		string toString();

		Vec3 getEntityPos();
		void updateEntityPos(Vec3 pos);

	private:
		// set owner
		void setOwner(EntityId id);

		// set destroyed
		void setDestroyed();

		// Entity id
		EntityId fOwnerId;

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

};


/**
 * TEMPLATED REQUEST FUNCTIONS
 */

// request all components of a given type in a given Entity
template<class T>
list<T*> Component::getComponents(string name, EntityId id) {
	list<Component*> comps;
	if (id == -1) comps = fEntityManager->getComponents(getOwnerId(), name);
	else comps = fEntityManager->getComponents(id, name);
	list<T*> tcomps;
	for (list<Component*>::iterator it = comps.begin(); it != comps.end(); ++it) {
		tcomps.push_back(dynamic_cast<T*>(*it));
	}
	return tcomps;
}

// message request function
template<class T>
void Component::requestMessage(string message, void (T::*f)(Message const &)) {
	requestMessage(message, std::tr1::bind(f, (T*)(this), _1));
}

// require a component in this Entity
template<class T>
void Component::requireComponent(string name, void (T::*f)(Message const &)) {
	requireComponent(name, std::tr1::bind(f, (T*)(this), _1));
}

// register a component request
template<class T>
void Component::requestComponent(string name, void (T::*f)(Message const &), bool local) {
	requestComponent(name, std::tr1::bind(f, (T*)(this), _1), local);
}

// request all components of one type
template<class T>
void Component::requestAllExistingComponents(string name, void (T::*f)(Message const &)) {
	requestAllExistingComponents(name, std::tr1::bind(f, (T*)(this), _1));
}


};


// output
std::ostream& operator<<(std::ostream& s, Cistron::Component &v);


#endif
