#include "Planet.h"

#include <iostream>

Planet::Planet() :
	CelestialBody(nullptr)
{
}

Planet::Planet(BlockGrid* blockGrid) :
	CelestialBody(blockGrid)
{
}

void Planet::tick(float dt)
{
	CelestialBody::tick(dt);
}
