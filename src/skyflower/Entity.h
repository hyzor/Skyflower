
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
		Entity(EntityManager *entityManager, const Modules *modules, EntityId id, EntityId relativeid, string type, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot,
			 float xScale, float yScale, float zScale, string model, bool isVisible, bool isCollidible, bool isAnimated, bool isSorted);
		virtual ~Entity();
		
		Entity* ground;				//a pointer to the ground the entity stands on, if it stands on something		
		Entity* wall;				//a pointer to the wall the entity is colliding with, if it is colliding with a wall
		Entity* roof;

		EntityId relativeid;
		EntityId fId;
		Sphere *sphere;				
		Vec3 spawnpos;
		CollisionInstance* collInst;
		Field* field;
		PhysicsEntity* mPhysicsEntity;


		void updatePos(Vec3 pos);
		void update(float deltaTime);
		void changeRelative(Entity* ground);
		void updateRelativePos(Vec3 pos);
		void updateRot(Vec3 rot);
		void ActiveteField(bool b);
		void updateScale(Vec3 scale);
		void updateVisible(bool isVisible);
		void setIsActive(bool status);

		bool returnVisible();
		bool getIsActive();
		bool hasComponents(const string &name);

		bool IsAnimated();
		void SetAnimation(int index, bool loop);
		void SetAnimation(int index, bool loop, bool lower, bool upper);
		bool IsAnimationDone();
		bool IsAnimationDone(bool lower, bool upper);
		void SetAnimationSpeed(int index, float speed);
		void SetAnimationSpeed(int index, float speed, bool lower, bool upper);
		bool IsPlayingAnimation(int index);
		bool IsPlayingAnimation(int index, bool lower, bool upper);


		Vec3 returnPos();
		Vec3 returnRot();
		Vec3 returnScale();
		Vec3 getRelativePos();

		const Modules *getModules();
		PhysicsEntity* getPhysics();
		string getType();
		CollisionInstance* returnCollision();

		template <typename T>
		T getComponent(const string &Type)
		{
			if (this->hasComponents(Type))
				return (T)fComponents[Type].front();
			else
				return NULL;
		}

		void sendMessage(const string &message, Component *component = NULL);

	private:

		string type;
		Vec3 pos;
		Vec3 rot;
		Vec3 scale;
		string model;
		bool isVisible;
		bool isActive;
		bool isAnimated;
		bool isCollidible;
		ModelInstance* modelInst;
		AnimatedInstance* AnimInst;
		SortedAnimatedInstance* SortInst;
		const Modules *modules;

		EntityManager *entityManager;

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

		// Entity manager is our friend
		friend class EntityManager;

		// Private functions we made
		EntityId getEntityId();
};


};


#endif
