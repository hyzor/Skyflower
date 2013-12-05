
#ifndef INC_Entity
#define INC_Entity

#include "Component.h"


#include <unordered_map>
#include <list>
#include <string>
#include <vector>


namespace Cistron {

using std::vector;
using std::list;
using std::string;
using std::tr1::unordered_map;


// an Entity is a container of components
class Entity {

	public:

		// constructor/destructor
		Entity(EntityId id, string type);
		virtual ~Entity();

	
	private:

		// Entity id
		EntityId fId;
		string type;


		/**
		 * COMPONENT MANAGEMENT
		 */

		// component map
		unordered_map<string, list<Component*> > fComponents;

		// add a component
		bool addComponent(Component*);

		// get a component
		list<Component*> getComponents(string name);

		// get all components
		list<Component*> getComponents();

		// remove all requests for a given component
		void removeComponent(Component*);

		/**
		 * LOCAL REQUESTS
		 */

		// send a local message
		void sendMessage(RequestId, Message const &);

		// register a request
		void registerRequest(RequestId, RegisteredComponent);

		// list of local requests
		vector<vector<RegisteredComponent> > fLocalRequests;

		/**
		 * Entity MANAGEMENT
		 */

		// is the Entity finalized?
		bool fFinalized;

		// finalize the Entity
		void finalize();

		// is the Entity finalized?
		bool isFinalized();

		/**
		 * LOGGING
		 */

		// track a request
		void trackRequest(RequestId, Component*);


		// Entity manager is our friend
		friend class EntityManager;

		//my own
		void sendAMessageToAll(string message);
		void sendMessageToComponentType(string message, string type);
		void sendMessageToEntity(string message, EntityId id);

		bool getType(string type);


};


};


#endif
