#include "CelestialBody.h"

#include <SDL.h>

CelestialBody::CelestialBody(CoordinateSystem minimalCs, BlockGrid* blockGrid) :
	SceneMultiNode(minimalCs),
    blockGrid(blockGrid),
	orbitalParent(nullptr)
{

}

void CelestialBody::tick(float dt)
{
	OrbitalProperties orbitalProperties = getOrbitalProperties();

	if (orbitalParent)
	{
		const float radius = orbitalProperties.radius;
		const float orbitalSpeed = orbitalProperties.orbitalVelocity;

		float angles = SDL_GetTicks() / 1000.0f * orbitalSpeed;

		glm::vec3 newPosition(glm::cos(glm::radians(angles)) * radius, 0.0f, glm::sin(glm::radians(angles)) * radius);
		newPosition += orbitalParent->getDerivedPosition(CoordinateSystem::Interplanetary, false);

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
	sattelite->orbitalParent = this;

    sattelites.push_back(sattelite);
}

const std::vector<CelestialBody*>& CelestialBody::getSattelites() const
{
	return sattelites;
}

CelestialBody* CelestialBody::getOrbitalParent() const
{
	return orbitalParent;
}

CelestialBody* CelestialBody::getHierarchicalParent() const
{
	CelestialBody* parentBody = getOrbitalParent();
	
	while (parentBody && parentBody->getContainedCoordinateSystem() == getContainedCoordinateSystem())
	{
		parentBody = parentBody->getOrbitalParent();
	}

	return parentBody;
}
