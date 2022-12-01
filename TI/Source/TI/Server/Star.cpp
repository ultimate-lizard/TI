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

//void Star::initRandomStarSystem()
//{
//	
//}
