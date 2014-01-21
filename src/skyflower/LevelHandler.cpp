#include "LevelHandler.h"
#include "ScriptHandler.h"

char* levels[] = {
	"testWorld.xml",
	"testWorld2.xml"
};

#define LEVEL_COUNT 2

LevelHandler *LevelHandler::instance = nullptr;

LevelHandler::LevelHandler()
{
	this->_entityManager = NULL;
	this->_current = 0;
	this->loading = false;
	this->queued = false;
	this->queueID = 0;
}

LevelHandler::~LevelHandler(){}

void LevelHandler::init(EntityManager *entityManager)
{
	this->_entityManager = entityManager;
	
	for (unsigned int i = 0; i < LEVEL_COUNT; i++)
	{
		Level curr(i, levels[i]);
		this->_levels.push_back(curr);
	}
}
LevelHandler* LevelHandler::GetInstance()
{
	if (!instance)
		instance = new LevelHandler();
	return instance;
}
void LevelHandler::queue(int id)
{
	/*
	// Removes all entities except for the player
	for (int i = 0; _entityManager->getNrOfEntities() != 1; i++) 
	{
		if (_entityManager->getEntity(i)->getType() != "Player")
			_entityManager->destroyEntity(i);
	}
	_entityManager->loadXML(_levels.at(id)._path);
	_current = id;
	*/

	queued = true;
	queueID = id;
}

bool LevelHandler::isCompleted(int id) const
{	
	return _levels.at(id)._completed;
}

void LevelHandler::levelCompleted()
{
	_levels.at(_current)._completed = true;
}

int LevelHandler::completedCount() const
{
	int completed = 0;
	for (unsigned int i = 0; i < _levels.size(); i++)
	{
		if (_levels.at(i)._completed)
		{
			completed++;
		}
	}
	return completed;
}

int LevelHandler::levelCount() const
{
	return (int)this->_levels.size();
}

int LevelHandler::currentLevel() const
{
	return this->_current;
}

void LevelHandler::LoadQueued()
{
	loading = true;
	loadQueued(queueID);
}

void LevelHandler::loadQueued(int id)
{
	queued = false;
	printf("Thread started\n");
	for (int i = 0; _entityManager->getNrOfEntities() != 1; i++)
	{
		if (_entityManager->getEntity(i)->getType() != "player")
			_entityManager->destroyEntity(i);
	}
	_entityManager->loadXML(_levels.at(queueID)._path);
	_current = queueID;
	loading = false;
}

bool LevelHandler::hasQueuedLevel()
{
	return queued || (loading == true);
}

bool LevelHandler::isLoading()
{
	return loading;
}