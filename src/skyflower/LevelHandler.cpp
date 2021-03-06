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
	"subWorld1",
	"subworld2"
};

#define LEVEL_COUNT 6

LevelHandler *LevelHandler::instance = nullptr;

LevelHandler::LevelHandler()
{
	this->m_entityManager = NULL;
	this->m_current = 0;
	this->m_loading = false;
	this->m_hasQueued = false;
	this->m_queueID = 0;
}

LevelHandler::~LevelHandler(){}

void LevelHandler::init(EntityManager *entityManager)
{
	this->m_entityManager = entityManager;
	
	for (unsigned int i = 0; i < LEVEL_COUNT; i++)
	{
		Level curr(i, levels[i]);
		this->m_levels.push_back(curr);
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
	m_hasQueued = true;
	m_queueID = id;
}

bool LevelHandler::isCompleted(int id) const
{	
	return m_levels.at(id).completed;
}

void LevelHandler::levelCompleted()
{
	m_levels.at(m_current).completed = true;
}

int LevelHandler::completedCount() const
{
	int completed = 0;
	for (unsigned int i = 0; i < m_levels.size(); i++)
	{
		if (m_levels.at(i).completed)
		{
			completed++;
		}
	}
	return completed;
}

int LevelHandler::levelCount() const
{
	return (int)this->m_levels.size();
}

int LevelHandler::currentLevel() const
{
	return this->m_current;
}

void LevelHandler::loadQueued()
{
	m_hasQueued = false;

	//get all entities to remove later
	std::vector<Entity*> old;
	int nrEntities = m_entityManager->getNrOfEntities();
	for (int i = 0; i < nrEntities; i++)
	{
		Entity* remove = m_entityManager->getEntityByIndex(0);
		old.push_back(remove);
		m_entityManager->removeEntity(remove);
	}
	m_entityManager->modules->graphics->ClearLights();
	//load new entities
	std::string xmlfile = m_levels.at(m_queueID).path;
	xmlfile += ".xml";
	m_entityManager->loadXML(xmlfile);
	m_current = m_queueID;

	//load lights
	xmlfile = m_levels.at(m_queueID).path;
	xmlfile += "lights.xml";
	m_entityManager->loadXML(xmlfile);

	//remove old entities
	for (unsigned int i = 0; i < old.size(); i++)
		delete old[i];

	//load lua file
	std::string luafile = m_levels.at(m_queueID).path;
	luafile += ".lua";
	m_entityManager->modules->script->Run(luafile);

	//run loaded function
	Event::entityManager = m_entityManager;
	lua_getglobal(m_entityManager->modules->script->L, "loaded");
	lua_pcall(m_entityManager->modules->script->L, 0, 0, 0);

	m_loading = false;
}

bool LevelHandler::hasQueuedLevel() const
{
	return m_hasQueued;
}

bool LevelHandler::isLoading() const
{
	return m_loading;
}

void LevelHandler::LoadQueued(const std::string& xmlResourceDir)
{
	m_hasQueued = false;

	//get all entities to remove later
	std::vector<Entity*> old;
	int nrEntities = m_entityManager->getNrOfEntities();
	for (int i = 0; i < nrEntities; i++)
	{
		Entity* remove = m_entityManager->getEntityByIndex(0);
		old.push_back(remove);
		m_entityManager->removeEntity(remove);
	}
	m_entityManager->modules->graphics->ClearLights();

	//remove old entities
	for (unsigned int i = 0; i < old.size(); i++)
		delete old[i];

	m_entityManager->modules->graphics->ClearModelInstances();
	m_entityManager->modules->graphics->ClearLevelTextures();

	//load new entities
	std::string xmlfile = m_levels.at(m_queueID).path;
	std::string skyFileStr = xmlfile;
	skyFileStr += ".sky";
	xmlfile += ".xml";
	m_entityManager->loadXML(xmlfile);
	m_current = m_queueID;

	//load lights
	xmlfile = m_levels.at(m_queueID).path;
	xmlfile += "lights.xml";
	m_entityManager->loadXML(xmlfile);

	// Load sky
	ifstream skyFile;
	std::string skyFilePath = xmlResourceDir + skyFileStr;
	skyFile.open(skyFilePath.c_str(), ios::in);

	if (!skyFile.is_open())
	{
		std::stringstream ErrorStream;
		ErrorStream << "Failed to load sky file " << skyFileStr;
		std::string ErrorStreamStr = ErrorStream.str();
		LPCSTR Text = ErrorStreamStr.c_str();
		MessageBoxA(0, Text, 0, 0);
	}
	else
	{
		std::string texName;
		std::getline(skyFile, texName);

		m_entityManager->modules->graphics->SetSkyTexture(texName);

		skyFile.close();
	}

	//remove old entities
// 	for (unsigned int i = 0; i < old.size(); i++)
// 		delete old[i];

	//load lua file
	std::string luafile = m_levels.at(m_queueID).path;
	luafile += ".lua";
	m_entityManager->modules->script->Run(luafile);

	//run loaded function
	Event::entityManager = m_entityManager;
	lua_getglobal(m_entityManager->modules->script->L, "loaded");
	lua_pcall(m_entityManager->modules->script->L, 0, 0, 0);

	m_loading = false;
}
