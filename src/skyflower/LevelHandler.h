#ifndef LEVELHANDLER_H
#define LEVELHANDLER_H

#include "EntityManager.h"
#include <vector>

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
	LevelHandler(vector<char*> LevelXMLs, EntityManager *entityManager);
	virtual ~LevelHandler();
	bool isCompleted(int id) const;
	void load(int id);
	int completedCount() const;
	void levelCompleted();
	int levelCount() const;

private:
	void _init(vector<char*> LevelXMLs);

	int _current;
	EntityManager *_entityManager;
	vector<Level> _levels;

};

#endif