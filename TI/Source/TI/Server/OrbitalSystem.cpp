#include "OrbitalSystem.h"

#include <SDL.h>

#include <TI/Server/CelestialBody.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

OrbitalSystem::OrbitalSystem(CelestialBody* primaryBody) :
	primaryBody(primaryBody)
{
	if (primaryBody)
	{
		primaryBody->setOrbitalSystem(this);
	}
}

void OrbitalSystem::addOrbitalSystem(OrbitalSystem* orbitalSystem)
{
	orbitingSystems.push_back(orbitalSystem);

	if (orbitalSystem)
	{
		if (CelestialBody* body = orbitalSystem->getPrimaryBody())
		{
			body->setOrbitalSystem(this);
		}
	}
}

void OrbitalSystem::tick(float dt)
{
	for (OrbitalSystem* orbitalSystem : orbitingSystems)
	{
		if (!orbitalSystem)
		{
			continue;
		}

		if (CelestialBody* body = orbitalSystem->getPrimaryBody())
		{
			OrbitalProperties orbitalProperties = body->getOrbitalProperties();
			const float radius = orbitalProperties.radius;
			const float orbitalSpeed = orbitalProperties.orbitalVelocity;
			const float rotationVelocity = orbitalProperties.equatorialVelocity;

			float angles = SDL_GetTicks() / 1000.0f * orbitalSpeed;

			glm::vec3 newPosition(glm::cos(glm::radians(angles)) * radius, 0.0f, glm::sin(glm::radians(angles)) * radius);

			glm::vec3 offset = newPosition - body->getLocalPosition(CoordinateSystem::Interplanetary);
			body->offset(offset, CoordinateSystem::Interplanetary);
			body->rotate(rotationVelocity * dt, { 0.0f, 1.0f, 0.0f }, CoordinateSystem::Interplanetary);

			// float accum = rotationVelocity

			// std::cout << 
			// TODO: For each player on the server
			// LEAVING PLANET BEGIN
			/*if (Entity* player = findEntity("Player"))
			{
				if (auto playerTransform = player->findComponent<TransformComponent>())
				{
					if (playerTransform->isChildOf(body, CoordinateSystem::Interplanetary))
					{
						const float playerToPlanetDist = glm::distance(playerTransform->getDerivedPosition(CoordinateSystem::Interplanetary), body->getDerivedPosition(CoordinateSystem::Interplanetary));
						if (playerToPlanetDist > 2.0f)
						{
							playerTransform->setLocalPosition(playerTransform->getDerivedPosition(CoordinateSystem::Planetary), CoordinateSystem::Planetary);
							playerTransform->setLocalPosition(playerTransform->getDerivedPosition(CoordinateSystem::Interplanetary), CoordinateSystem::Interplanetary);
							playerTransform->setLocalPosition(playerTransform->getDerivedPosition(CoordinateSystem::Interstellar), CoordinateSystem::Interstellar);
							playerTransform->setLocalOrientation(playerTransform->getDerivedOrientation(CoordinateSystem::Interplanetary), CoordinateSystem::Interplanetary, true);

							playerTransform->removeParent();
						}
					}
				}
			}*/
			// LEAVING PLANET END
		}
	}
}

CelestialBody* OrbitalSystem::getPrimaryBody()
{
	return primaryBody;
}

const std::vector<OrbitalSystem*>& OrbitalSystem::getOrbitingSystems() const
{
	return orbitingSystems;
}
