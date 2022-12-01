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

CoordinateSystem Planet::getContainedCoordinateSystem() const
{
	return CoordinateSystem::Planetary;
}
