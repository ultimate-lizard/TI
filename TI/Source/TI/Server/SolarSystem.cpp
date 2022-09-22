#include "SolarSystem.h"

#include <TI/Server/Star.h>

SolarSystem::SolarSystem(Star* primaryBody) :
	OrbitalSystem(primaryBody)
{
}

void SolarSystem::initRandom()
{
	if (!getPrimaryBody())
	{
		return;
	}

	getPrimaryBody()->setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar); // 100 km in size

	const float low = -100.0f;
	const float high = 100.0f;

	float x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

	getPrimaryBody()->setLocalPosition({ x, y, z }, CoordinateSystem::Interstellar);
}
