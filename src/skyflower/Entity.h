
#ifndef INC_Entity
#define INC_Entity

#include "Cistron.h"
#include "Component.h"


#include <unordered_map>
#include <list>
#include <string>
#include <vector>

#include "graphics/GraphicsEngine.h"
#include "physics/PhysicsEngine.h"
#include "physics/Sphere.h"
#include "PotentialField.h"

namespace Cistron {

using std::vector;
using std::list;
using std::string;
using std::tr1::unordered_map;

struct Modules;

// an Entity is a container of components
class Entity {

	public:

		// constructor/destructor
		Entity(const Modules *modules, EntityId id, EntityId relativeid, string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
			 float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated);
		virtual ~Entity();
		void sendPosToComponent();
		void update(float deltaTime);

		const Modules *getModules();
		PhysicsEntity* getPhysics();
		string getType();

		template <typename T>
		T getComponent(string Type)
		{
			if (this->hasComponents(Type))
				return (T)fComponents[Type].front();
			else
				return NULL;
		}
		bool hasComponents(string name);

		Vec3 returnPos();
		Vec3 spawnpos;
		CollisionInstance* collInst;
		Field* field;

		//a pointer to the ground the entity stands on, if it stands on something
		Entity* ground; 
		//a pointer to the wall the entity is colliding with, if it is colliding with a wall
		Entity* wall;
		Sphere *sphere;

		EntityId relativeid;

		void updatePos(Vec3 pos);


		void changeRelative(Entity* ground);
		void updateRelativePos(Vec3 pos);
		Vec3 getRelativePos();
		AnimatedInstance *getAnimatedInstance();

		void updateRot(Vec3 rot);

		PhysicsEntity* mPhysicsEntity;
		EntityId fId;

		void ActiveteField(bool b);
		Vec3 returnRot();
		Vec3 returnScale();
		bool returnVisible();
		bool getIsActive();
		CollisionInstance* returnCollision();
		void updateScale(Vec3 scale);
		void updateVisible(bool isVisible);
		void setIsActive(bool status);



	private:

		string type;
		Vec3 pos;
		Vec3 rot;
		Vec3 scale;
		string model;
		bool isVisible;
		bool isActive;
		ModelInstance* modelInst;
		AnimatedInstance* AnimInst;
		bool isAnimated;
		bool isCollidible;

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

		// Functions we made
		void sendAMessageToAll(string message);
		void sendMessageToEntity(string message, EntityId id);

		EntityId getEntityId();



		const Modules *modules;
};


};


#endif
