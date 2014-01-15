#ifndef LEVELHANDLER_H
#define LEVELHANDLER_H

#include "EntityManager.h"
#include <vector>

#define levelHandler LevelHandler::GetInstance() 

class LevelHandler
{
	
public:
	struct Level
	{
		int _id;
		bool _completed;
		char* _path;
		Level(int id, char* path) : _id(id), _path(path), _completed(false) {}
	};
	static LevelHandler* GetInstance();
	virtual ~LevelHandler();
	void init(EntityManager *entityManager); 
	bool isCompleted(int id) const; // Check if map has been completed before
	void load(int id); // Load a new map
	int completedCount() const; // Number of completed maps
	void levelCompleted(); // Completed level count
	int levelCount() const; // Number of available levels
	int currentLevel() const; // Current level

	bool Check();

private:
	
	bool loadlevel;
	int levelid;

	LevelHandler();
	static LevelHandler *instance;
	int _current; // Current level id
	EntityManager *_entityManager; // Pointer to the entities
	vector<Level> _levels; // Levels

};

#endif
