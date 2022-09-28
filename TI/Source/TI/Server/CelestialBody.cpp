#include "CelestialBody.h"

#include <SDL.h>

CelestialBody::CelestialBody() :
    blockGrid(nullptr)
{
}

CelestialBody::CelestialBody(BlockGrid* blockGrid) :
    blockGrid(blockGrid)
{

}

void CelestialBody::tick(float dt)
{
	OrbitalProperties orbitalProperties = getOrbitalProperties();

	if (primaryBody)
	{
		const float radius = orbitalProperties.radius;
		const float orbitalSpeed = orbitalProperties.orbitalVelocity;

		float angles = SDL_GetTicks() / 1000.0f * orbitalSpeed;

		glm::vec3 newPosition(glm::cos(glm::radians(angles)) * radius, 0.0f, glm::sin(glm::radians(angles)) * radius);
		newPosition += primaryBody->getDerivedPosition(CoordinateSystem::Interplanetary, false);

		glm::vec3 newOffset = newPosition - getLocalPosition(CoordinateSystem::Interplanetary);
		offset(newOffset, CoordinateSystem::Interplanetary);
	}

	const float rotationVelocity = orbitalProperties.equatorialVelocity;
	rotate(rotationVelocity * dt, { 0.0f, 1.0f, 0.0f });
}

BlockGrid* CelestialBody::getBlockGrid()
{
    return blockGrid;
}

void CelestialBody::setOrbitalProperties(const OrbitalProperties& newProperties)
{
    orbitalProperties = newProperties;
}

const OrbitalProperties& CelestialBody::getOrbitalProperties() const
{
    return orbitalProperties;
}

void CelestialBody::addSattelite(CelestialBody* sattelite)
{
	sattelite->primaryBody = this;

    sattelites.push_back(sattelite);
}

const std::vector<CelestialBody*>& CelestialBody::getSattelites() const
{
	return sattelites;
}