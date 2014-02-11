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

	// Check if map has been completed before.
	bool isCompleted(int id) const; 

	// Queue a new map. Call LoadQueued() to load the map
	void queue(int id); 

	// Number of completed levels.
	int completedCount() const;

	// Mark the level as completed
	void levelCompleted();

	// Number of available levels
	int levelCount() const; 

	// ID of the current loaded level.
	int currentLevel() const; 

	// Returns true if a level is currently queued.
	bool hasQueuedLevel();

	// Load a queued level. Call queue() to queue a level.
	void LoadQueued();

	// Returns true if a level is currently loading. Useful when loading on a seperate thread.
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
	MorphModelInstance* skyflower;

};

#endif
