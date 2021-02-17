#include "Server.h"

int Server::clientId = 0;

Entity* const Server::findEntity(const std::string& name)
{
	auto iter = spawnedEntities.find(name);
	if (iter != spawnedEntities.end())
	{
		return iter->second.get();
	}

	return nullptr;
}

const std::map<std::string, std::unique_ptr<Entity>>& Server::getEntities() const
{
	return spawnedEntities;
}
