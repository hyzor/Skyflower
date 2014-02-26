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
		int id;
		bool completed;
		char* path;
		Level(int id, char* path) : id(id), path(path), completed(false) {}
	};
	static LevelHandler* GetInstance();
	virtual ~LevelHandler();
	void init(EntityManager* const entityManager); 

	// Check if map has been completed before.
	bool isCompleted(int id) const; 

	// Queue a new map. Call loadQueued() to load the map
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
	bool hasQueuedLevel() const;

	// Load a queued level. Call queue() to queue a level.
	void loadQueued();
	void LoadQueued(const std::string& xmlResourceDir);

	// Returns true if a level is currently loading. Useful when loading on a seperate thread.
	bool isLoading() const;

private:
	LevelHandler();

	static LevelHandler *instance;

	bool m_hasQueued; // If a level is queued it will load at the end of the frame
	int m_queueID; // Id of the queued level
	bool m_loading;;
	int m_current; // Current level id
	vector<Level>  m_levels; // Levels

	EntityManager *m_entityManager; // Pointer to the entities

};

#endif
