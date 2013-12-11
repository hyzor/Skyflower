
#ifndef INC_EntityManager
#define INC_EntityManager


#include "Entity.h"
#include "Cistron.h"
#include "tinyxml2.h"
#include "ComponentHeaders.h"

#include <unordered_map>
#include <list>
#include <string>
#include <sstream>

#include "graphics/GraphicsEngine.h"
#include "physics/Collision.h"


namespace Cistron {

using std::list;
using std::string;
using std::pair;
using std::stringstream;

// the Entity manager manages all Entity entities, and performs communication between them
class EntityManager {

	public:

		// constructor/destructor
		EntityManager(GraphicsEngine* gEngine);
		virtual ~EntityManager();

		// create a new Entity
		EntityId createEntity(string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
			float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible);

		// add a new component to an Entity
		void addComponent(EntityId, Component*);

		// destroy Entity
		void destroyEntity(EntityId);

		// destroy a component
		void destroyComponent(Component*);

		// finalize an Entity, resolving the required components
		void finalizeEntity(EntityId);


		// register a unique name for an Entity
		void registerName(Component*, string name);
		void unregisterName(Component*, string name);

		// get the id based on the unique name identified
		list<Component*> getComponentsByName(string name);



		/**
		 * REQUEST MESSAGES
		 */

		// register global requests
		void registerGlobalRequest(ComponentRequest, RegisteredComponent reg);

		// register a local request
		void registerLocalRequest(ComponentRequest, RegisteredComponent reg);

		// get all components of a given type in a given Entity
		list<Component*> getComponents(EntityId objId, string componentName) {
			return fEntitys[objId]->getComponents(componentName);
		}


		/**
		 * SENDING MESSAGES
		 */

		// send global messages
		inline void sendGlobalMessage(string msg, Component *component, Payload payload) {
			sendGlobalMessage(getExistingRequestId(REQ_MESSAGE, msg), Message(MESSAGE, component, payload));
		}
		inline void sendGlobalMessage(RequestId reqId, Component *component, Payload payload) {
			sendGlobalMessage(reqId, Message(MESSAGE, component, payload));
		}
		void sendGlobalMessage(RequestId reqId, Message const & msg);

		// send local messages to another Entity
		inline void sendMessageToEntity(string msg, Component *component, EntityId id, Payload payload) {
			fEntitys[id]->sendMessage(getMessageRequestId(REQ_MESSAGE, msg), Message(MESSAGE, component, payload));
		}
		inline void sendMessageToEntity(RequestId reqId, Component *component, EntityId id) {
			fEntitys[id]->sendMessage(reqId, Message(MESSAGE, component));
		}
		inline void sendMessageToEntity(RequestId reqId, Component *component, EntityId id, Payload payload) {
			fEntitys[id]->sendMessage(reqId, Message(MESSAGE, component, payload));
		}
		inline void sendMessageToEntity(string name, Message const & msg, EntityId id) {
			fEntitys[id]->sendMessage(getMessageRequestId(REQ_MESSAGE, name), msg);
		}
		inline void sendMessageToEntity(RequestId reqId, Message const & msg, EntityId id) {
			fEntitys[id]->sendMessage(reqId, msg);
		}

		// ask for a request id
		RequestId getMessageRequestId(ComponentRequestType, string name);

		/**
		 * LOGGING
		 */

		// track a request
		void trackRequest(RequestId, bool local, Component*);

		// get request name
		inline string getRequestById(ComponentRequestType type, RequestId reqId) {
			return fIdToRequest[type][reqId];
		}


		//my own
		void sendMessageToAllEntities(string message); //sends a message to all components in all entities in that manager
		void sendMessageToEntity(string message, string entity); //sends a message to a specific entity
		bool loadXML(EntityManager * entityManager, string xmlFile);

		Vec3 getEntityPos(string type);
		Vec3 getEntityPos(EntityId ownerId);
		Vec3 getEntityRot(EntityId ownerId);
		Vec3 getEntityScale(EntityId ownerId);
		bool getEntityVisibility(EntityId ownerId);
		CollisionInstance* getEntityCollision(EntityId ownerId);
		void updateEntityPos(Vec3 pos, EntityId id);
		void updateEntityRot(Vec3 rot, EntityId id);
		void updateEntityScale(Vec3 scale, EntityId id);
		void updateEntityVisibility(bool isVisible, EntityId id);
		Component* getComponent(string EntityName, string Componenet);

	private:

		/**
		 * REQUEST IDS
		 */

		// request id counter
		RequestId fRequestIdCounter;

		// mapping from a request to a unique id that identifies the request
		std::tr1::unordered_map<string, RequestId> fRequestToId[2];

		// mapping from request id to the original request name
		std::tr1::unordered_map<RequestId, string> fIdToRequest[2];

		// get an existing request id
		RequestId getExistingRequestId(ComponentRequestType, string name);


		/**
		 * LOCKING MECHANISM
		 */

		// activate/release request lock
		// locks are activated when the current request id is being iterated on, to avoid conflicts with iterators
		void activateLock(RequestId);
		void releaseLock(RequestId);

		// number of active locks
		int fNLocks;

		// a request lock contains whether this request is at the moment locked
		// a request is locked if currently, a message is being processed for this request
		struct RequestLock {
			bool locked;
			list<pair<ComponentRequest, RegisteredComponent> > pendingLocalRequests;
			list<pair<ComponentRequest, RegisteredComponent> > pendingGlobalRequests;
			RequestLock() : locked(false) {};
		};

		// list of locked request id's
		vector<RequestLock> fRequestLocks;

		// list of pending destroyals
		list<EntityId> fDeadEntitys;
		list<Component*> fDeadComponents;

		/**
		 * EntityS
		 */

		// id counter
		EntityId fIdCounter;

		// list of Entitys with their id's
		vector<Entity*> fEntitys;

		// mapping of Entitys to their unique name identified
		std::tr1::unordered_map<string, list<Component*> > fComponentsByName;

		/**
		 * REQUESTS
		 */


		// vector of global requests
		vector<list<RegisteredComponent> > fGlobalRequests;

		// list of required components which still need to be processed
		std::tr1::unordered_map<EntityId, list<string> > fRequiredComponents;

		// list of component requests, by component id
		std::tr1::unordered_map<ComponentId, list<ComponentRequest> > fRequestsByComponentId;


		/**
		 * ERROR PROCESSING
		 */

		// error function
		void error(stringstream& str);


		GraphicsEngine* gEngine;
};

};


#endif
