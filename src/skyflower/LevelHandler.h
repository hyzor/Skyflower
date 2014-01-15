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
	bool isCompleted(int id) const;
	void load(int id);
	int completedCount() const;
	void levelCompleted();
	int levelCount() const;
	int currentLevel() const;

	bool Check();
private:
	
	bool loadlevel;
	int levelid;

	LevelHandler();
	static LevelHandler *instance;
	int _current;
	EntityManager *_entityManager;
	vector<Level> _levels;

};

#endif