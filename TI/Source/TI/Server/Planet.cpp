#include "Planet.h"

#include <iostream>

Planet::Planet() :
	CelestialBody(CoordinateSystem::Interplanetary, nullptr)
{
}

Planet::Planet(BlockGrid* blockGrid) :
	CelestialBody(CoordinateSystem::Interplanetary, blockGrid)
{
}

void Planet::tick(float dt)
{
	CelestialBody::tick(dt);
}
