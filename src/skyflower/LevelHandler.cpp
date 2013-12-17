#include "LevelHandler.h"

LevelHandler::LevelHandler(vector<char*> levelXMLs, EntityManager *entityManager)
{
	this->_entityManager = entityManager;
	this->_current = 0;
	this->_init(levelXMLs);
}

LevelHandler::~LevelHandler(){}

void LevelHandler::_init(vector<char*> levelXMLs)
{
	for (int i = 0; i < levelXMLs.size(); i++)
	{
		Level curr(i, levelXMLs.at(i));
		this->_levels.push_back(curr);
	}
}

void LevelHandler::load(int id)
{
	_entityManager->loadXML(_entityManager,_levels.at(id)._path);
	_current = id;
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
	for (int i = 0; i < _levels.size(); i++)
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
	return this->_levels.size();
}