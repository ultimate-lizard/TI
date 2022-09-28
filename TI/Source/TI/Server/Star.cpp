#include "Star.h"

#include <iostream>

Star::Star() :
	CelestialBody(nullptr)
{
}

Star::~Star()
{
}

void Star::tick(float dt)
{
	CelestialBody::tick(dt);

	// offset({ 0.0f, 1.0f * dt, 0.0f }, CoordinateSystem::Interplanetary);

	// std::cout << "Star position: " << getLocalPosition(CoordinateSystem::Interplanetary).x << " " << getLocalPosition(CoordinateSystem::Interplanetary).y << " " << getLocalPosition(CoordinateSystem::Interplanetary).z << std::endl;
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
