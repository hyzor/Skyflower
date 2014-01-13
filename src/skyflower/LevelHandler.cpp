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
void LevelHandler::load(int id)
{
	loadlevel = true;
	levelid = id;
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

bool LevelHandler::Check()
{
	if (loadlevel)
	{
		loadlevel = false;
		for (int i = 1; _entityManager->getNrOfEntities() != 1; i++)
		{
			_entityManager->destroyEntity(i);
		}
		_entityManager->loadXML2(_levels.at(levelid)._path);
		_current = levelid;
		return true;
	}
	return false;
}