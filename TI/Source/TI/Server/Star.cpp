#include "Star.h"

#include <iostream>

Star::Star() :
	CelestialBody(CoordinateSystem::Interstellar)
{
}

CoordinateSystem Star::getContainedCoordinateSystem() const
{
	return CoordinateSystem::Interplanetary;
}

void Star::initRandomStarSystem()
{
	setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar); // 100 km in size

	const float low = -100.0f;
	const float high = 100.0f;

	float x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

	setLocalPosition({ x, y, z }, CoordinateSystem::Interstellar);
}
