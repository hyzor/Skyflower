
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

#include <algorithm>

namespace Cistron {

using std::list;
using std::string;
using std::pair;
using std::stringstream;

// the Entity manager manages all Entity entities, and performs communication between them
class EntityManager {

	public:

		// constructor/destructor
		EntityManager(const std::string &resourceDir, const Modules *modules);
		virtual ~EntityManager();

		void update(float deltaTime);

		// create a new Entity
		EntityId createEntity(string type, int id, int relativeid, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
			float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated);

		// add a new component to an Entity
		void addComponent(EntityId, Component*);

		// destroy Entity
		void destroyEntity(EntityId);
		void removeEntity(Entity* e);

		// destroy a component
		void destroyComponent(Component*);

		/**
		 * REQUEST MESSAGES
		 */

		// register global requests
		void registerGlobalRequest(ComponentRequest, RegisteredComponent reg);

		// register a local request
		void registerLocalRequest(ComponentRequest, RegisteredComponent reg);

		/**
		 * SENDING MESSAGES
		 */

		void sendGlobalMessage(RequestId reqId, Message const & msg);

		void sendGlobalMessage(string message);

		// ask for a request id
		RequestId getMessageRequestId(ComponentRequestType, const string &name);

		/**
		 * LOGGING
		 */

		// get request name
		inline string getRequestById(ComponentRequestType type, RequestId reqId) {
			return fIdToRequest[type][reqId];
		}


		// Functions we made
		bool loadXML(string xmlFile);

		Vec3 getEntityPos(string type);
		Vec3 getEntityPos(EntityId ownerId);
		Vec3 getEntityRot(EntityId ownerId);
		Vec3 getEntityScale(EntityId ownerId);
		bool getEntityVisibility(EntityId ownerId);
		CollisionInstance* getEntityCollision(EntityId ownerId);
		Component* getComponent(string EntityName, string Componenet);
		Entity *getEntity(EntityId id);
		int getNrOfEntities();
		EntityId getEntityId(int index);
		Entity* getEntityByIndex(int index);


		void updateEntityPos(Vec3 pos, EntityId id);
		void updateEntityRot(Vec3 rot, EntityId id);
		void updateEntityScale(Vec3 scale, EntityId id);
		void updateEntityVisibility(bool isVisible, EntityId id);

		void updateSpheres();

		void activateEntity(EntityId eId);
		void deactivateEntity(EntityId eId);

		const Modules *modules;

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

		std::string m_resourceDir;
};

};


#endif
