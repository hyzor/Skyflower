
#ifndef INC_Entity
#define INC_Entity

#include "Component.h"


#include <unordered_map>
#include <list>
#include <string>
#include <vector>

#include "graphics/GraphicsEngine.h"

namespace Cistron {

using std::vector;
using std::list;
using std::string;
using std::tr1::unordered_map;


// an Entity is a container of components
class Entity {

	public:

		// constructor/destructor
		Entity(GraphicsEngine* gEngine, EntityId id, string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
			 float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible);
		virtual ~Entity();
		void sendPosToComponent();
	
	private:

		// Entity id
		EntityId fId;
		string type;
		Vec3 pos;
		Vec3 rot;
		Vec3 scale;
		string model;
		bool isVisible;
		ModelInstance* modelInst;
		CollisionInstance* collInst;

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
		void sendMessageToEntity(string message, EntityId id);

		string getType();
		EntityId getEntityId();

		Vec3 returnPos();
		Vec3 returnRot();
		Vec3 returnScale();
		bool returnVisible();
		CollisionInstance* returnCollision();
		void updatePos(Vec3 pos);
		void updateRot(Vec3 rot);
		void updateScale(Vec3 scale);
		void updateVisible(bool isVisible);



		GraphicsEngine* gEngine;
};


};


#endif
