#include "LevelHandler.h"
#include "ScriptHandler.h"
#include <string>

// Must be included last!
#include "shared/debug.h"

char* levels[] = {
	"hubWorld",
	"testWorld",
	"testWorld2",
	"testExport",
	"subWorld1"
};

#define LEVEL_COUNT 5

LevelHandler *LevelHandler::instance = nullptr;

LevelHandler::LevelHandler()
{
	this->_entityManager = NULL;
	this->_current = 0;
	this->loading = false;
	this->queued = false;
	this->queueID = 0;
	this->skyflower = NULL;
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

	//get all entities to remove later
	std::vector<Entity*> old;
	int nrEntities = _entityManager->getNrOfEntities();
	for (int i = 0; i < nrEntities; i++)
	{
		Entity* remove = _entityManager->getEntityByIndex(0);
		old.push_back(remove);
		_entityManager->removeEntity(remove);
	}

	//load new entities
	std::string xmlfile = _levels.at(queueID)._path;
	xmlfile += ".xml";
	_entityManager->loadXML(xmlfile);
	_current = queueID;

	//remove old entities
	for (unsigned int i = 0; i < old.size(); i++)
		delete old[i];

	//load lua file
	std::string luafile = _levels.at(queueID)._path;
	luafile += ".lua";
	_entityManager->modules->script->Run(luafile);

	//run loaded function
	Event::entityManager = _entityManager;
	lua_getglobal(_entityManager->modules->script->L, "loaded");
	lua_pcall(_entityManager->modules->script->L, 0, 0, 0);

	loading = false;
}

bool LevelHandler::hasQueuedLevel()
{
	return queued;
}

bool LevelHandler::isLoading()
{
	return loading;
}