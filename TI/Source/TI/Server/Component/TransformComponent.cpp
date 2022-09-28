#include "TransformComponent.h"

#include <TI/Server/BlockGrid.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Server/Entity.h>
#include <TI/Server/LocalServer.h>
#include <TI/Server/Planet.h>
#include <TI/Server/Component/CameraComponent.h>

static bool firstTick = true;

TransformComponent::TransformComponent(Server* server) :
	Component(),
	SceneMultiNode(),
	server(nullptr),
	blockGrid(nullptr)
{
	this->server = dynamic_cast<LocalServer*>(server);
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	Component(other),
	SceneMultiNode(other),
	server(other.server),
	blockGrid(other.blockGrid)
{
}

void TransformComponent::tick(float dt)
{
	if (orientationInProgress)
	{
		setLocalOrientation(glm::mix(getLocalOrientation(targetCs), targetOrientation, dt * 15.0f), targetCs, true);

		float matching = glm::dot(getLocalOrientation(targetCs), targetOrientation);
		if (abs(matching - 1.0f) < 0.00001f) {
			orientationInProgress = false;
		}
	}

	//std::cout << "Derived position: " << getDerivedPosition(CoordinateSystem::Interplanetary).x << " " <<
	//	getDerivedPosition(CoordinateSystem::Interplanetary).y << " " << getDerivedPosition(CoordinateSystem::Interplanetary).z << std::endl;

	const CoordinateSystem planetaryCs = CoordinateSystem::Planetary;

	if (CelestialBody* primaryBody = getPrimaryBody())
	{
		if (auto coordinateSystem = getCoordinateSystem(planetaryCs); coordinateSystem.has_value())
		{
			// We're in Planetary coordinate system. We can now leave this system
			if (glm::distance(getDerivedPosition(CoordinateSystem::Interplanetary, false), primaryBody->getDerivedPosition(CoordinateSystem::Interplanetary, false)) > 1.0f)
			{
				setPrimaryBody(nullptr);

				leaveNode(planetaryCs);

				glm::vec3 pos = getDerivedPosition(CoordinateSystem::Interplanetary);
				const glm::quat rot = getDerivedOrientation(CoordinateSystem::Interplanetary);

				removeParent(CoordinateSystem::Interplanetary);

				setLocalPosition(pos, CoordinateSystem::Interplanetary);
				setLocalOrientation(rot, CoordinateSystem::Interplanetary);

				setCurrentBlockGrid(nullptr);
			}
		}
	}
	else
	{
		if (Planet* closestPlanet = server->findClosestPlanet(getDerivedPosition(CoordinateSystem::Interplanetary, false), CoordinateSystem::Interplanetary))
		{
			/*std::cout << "Closest planet: " << (closestPlanet->getSattelites().empty() ? "sattelite. " : "planet. ") << "Distance: " <<
				glm::distance(getDerivedPosition(CoordinateSystem::Interplanetary, false), closestPlanet->getDerivedPosition(CoordinateSystem::Interplanetary, false)) << std::endl;*/

			if (auto coordinateSystem = getCoordinateSystem(planetaryCs); !coordinateSystem.has_value())
			{
				if (glm::distance(getDerivedPosition(CoordinateSystem::Interplanetary, false), closestPlanet->getDerivedPosition(CoordinateSystem::Interplanetary, false)) <= 1.0f)
				{
					setPrimaryBody(closestPlanet);

					// We should preserve the player rotation, as they could've moved to a different planet side and rotated themselves
					const glm::quat correctOrientation = getLocalOrientation();

					enterNode(planetaryCs);

					// The players planetary coordinate system is not a child of planet coordinate system. This is to keep the illusion working
					if (auto coordSystem = getCoordinateSystem(planetaryCs); coordSystem.has_value())
					{
						coordSystem.value()->removeParent();
					}

					// This is probably correct logic. Checked in ogre3d
					const glm::vec3 newPosition = glm::inverse(closestPlanet->getLocalOrientation(CoordinateSystem::Interplanetary)) * (getLocalPosition(CoordinateSystem::Interplanetary) - closestPlanet->getDerivedPosition(CoordinateSystem::Interplanetary, false));

					setParent(closestPlanet, CoordinateSystem::Interplanetary);

					setLocalPosition(newPosition, CoordinateSystem::Interplanetary);

					const glm::quat newRotation = glm::inverse(closestPlanet->getLocalOrientation(CoordinateSystem::Interplanetary)) * getLocalOrientation(CoordinateSystem::Interplanetary);
					setLocalOrientation(newRotation, CoordinateSystem::Planetary, true);

					glm::vec3 blockGridSize(0.0f);
					if (BlockGrid* bg = closestPlanet->getBlockGrid())
					{
						blockGridSize = { bg->getBlockGridSize().x * bg->getChunkSize(), bg->getBlockGridSize().y * bg->getChunkSize(), bg->getBlockGridSize().z * bg->getChunkSize() };
						setCurrentBlockGrid(bg);
					}

					const glm::vec3 offset = blockGridSize / 2.0f;

					setLocalPositionExclusive(
						((getDerivedPosition(CoordinateSystem::Interplanetary, false) * 1000.0f)
							-
							closestPlanet->getDerivedPosition(CoordinateSystem::Interplanetary, false) * 1000.0f + offset), planetaryCs);
				}
			}
		}
	}
}

void TransformComponent::setCurrentBlockGrid(BlockGrid* blockGrid)
{
	this->blockGrid = blockGrid;
}

BlockGrid* TransformComponent::getCurrentBlockGrid() const
{
	return blockGrid;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

void TransformComponent::setTargetLocalOrientation(const glm::quat& orientation, CoordinateSystem cs)
{
	if (!orientationInProgress)
	{
		orientationInProgress = true;

		targetOrientation = orientation;
		targetCs = cs;
	}
}

void TransformComponent::setPrimaryBody(CelestialBody* body)
{
	currentPrimaryBody = body;
}

CelestialBody* TransformComponent::getPrimaryBody() const
{
	return currentPrimaryBody;
}
