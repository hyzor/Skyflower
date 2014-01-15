#ifndef LEVELHANDLER_H
#define LEVELHANDLER_H

#include "EntityManager.h"
#include <vector>

#include <future>
#include <thread>

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
	void queue(int id); // Load a new map
	int completedCount() const; // Number of completed maps
	void levelCompleted(); // Completed level count
	int levelCount() const; // Number of available levels
	int currentLevel() const; // Current level
	bool hasQueuedLevel();
	void LoadQueued();
	bool isLoading();

private:
	
	bool queued; // If a level is queued it will load at the end of the frame
	int queueID; // Id of the queued level
	bool loading;;
	LevelHandler();
	void loadQueued(int id);
	
	static LevelHandler *instance;
	int _current; // Current level id
	EntityManager *_entityManager; // Pointer to the entities
	vector<Level> _levels; // Levels

};

#endif
