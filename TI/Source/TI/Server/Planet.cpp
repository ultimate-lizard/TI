#include "Planet.h"

Planet::Planet() :
	CelestialBody(nullptr)
{
}

Planet::Planet(BlockGrid* blockGrid) :
	CelestialBody(blockGrid)
{
}
