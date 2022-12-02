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

	static const float INTERPLANETARY_LEAVE_DISTANCE = 1.0f;
	static const float INTERSTELLAR_LEAVE_DISTANCE = 1.0f;
	static const float INTERGALACTICAL_LEAVE_DISTANCE = 1.0f;

	// Variant 1:
	// if (farther than getLeaveDistance(currentCoordinateSystem))
	// {
	//		leave
	// }
	// else if (closer than enter distance of findNearestObject(currentCooridnateSystem))
	// {
	// }

	//const CoordinateSystem planetaryCs = CoordinateSystem::Planetary;

	//// Delete Planetary CS if there is no currentPrimaryBody
	//if (auto coordinateSystem = getCoordinateSystem(CoordinateSystem::Planetary); coordinateSystem.has_value())
	//{
	//	if (!primaryBody)
	//	{
	//		leaveNode(planetaryCs);
	//	}
	//}

	if (!primaryBody)
	{
		while (getCurrentCoordinateSystem() != CoordinateSystem::Interstellar)
		{
			leaveNode(getCurrentCoordinateSystem());
		}
	}

	// TODO: This code should be in the current state until all CSs are implemented

	const CoordinateSystem currentCoordinateSystem = getCurrentCoordinateSystem();

	if (CelestialBody* primaryBody = getPrimaryBody())
	{
		const CoordinateSystem csToEnter = primaryBody->getCurrentCoordinateSystem();
		const CoordinateSystem currentCs = getCurrentCoordinateSystem(); // or primaryBody->getContainedCoordinateSystem();

		const float scale = primaryBody->getScale(csToEnter).x;
		// We're in Planetary coordinate system. We can now leave this system
		if (float dist = glm::distance(getDerivedPosition(csToEnter, false), primaryBody->getDerivedPosition(csToEnter, false)); dist > 10.0f * scale + 1.0f)
		{
			std::cout << "Dist was " << dist << ". Leaving " << currentCs << std::endl;
			leaveNode(currentCoordinateSystem);

			glm::vec3 pos = getDerivedPosition(csToEnter);
			const glm::quat rot = getDerivedOrientation(csToEnter);

			removeParent(csToEnter);

			setLocalPosition(pos, csToEnter);
			setLocalOrientation(rot, csToEnter, true);

			setPrimaryBody(primaryBody->getHierarchicalParent());
			setCurrentBlockGrid(nullptr);
		}
	}

	{
		const CoordinateSystem currentCs = getCurrentCoordinateSystem(); // or primaryBody->getContainedCoordinateSystem();

		if (CelestialBody* closestCelestialBody = server->findClosestCelestialBody(currentCs, getDerivedPosition(currentCs, false)))
		{
			const CoordinateSystem csToEnter = closestCelestialBody->getContainedCoordinateSystem();

			if (auto csToEnterOptional = getCoordinateSystem(csToEnter); !csToEnterOptional.has_value())
			{
				const float scale = closestCelestialBody->getScale(currentCs).x;
				if (float dist = glm::distance(getDerivedPosition(currentCs, false), closestCelestialBody->getDerivedPosition(currentCs, false)); dist <= 5.0f * scale + 1.0f)
				{
					std::cout << "Dist was " << dist << ". Entering " << csToEnter << std::endl;
					setPrimaryBody(closestCelestialBody);

					// We should preserve the player rotation, as they could've moved to a different planet side and rotated themselves
					const glm::quat correctOrientation = getLocalOrientation();

					enterNode(csToEnter);

					// The players planetary coordinate system is not a child of planet coordinate system. This is to keep the illusion working
					if (auto csNodeToEnter = getCoordinateSystem(csToEnter); csNodeToEnter.has_value())
					{
						csNodeToEnter.value()->removeParent();
					}

					// This is probably correct logic. Checked in ogre3d
					const glm::vec3 newPosition = glm::inverse(closestCelestialBody->getLocalOrientation(currentCs)) * (getLocalPosition(currentCs) - closestCelestialBody->getDerivedPosition(currentCs, false));

					setParent(closestCelestialBody, currentCs);

					setLocalPosition(newPosition, currentCs);

					const glm::quat newRotation = glm::inverse(closestCelestialBody->getLocalOrientation(currentCs)) * getLocalOrientation(currentCs);
					setLocalOrientation(newRotation, csToEnter, true);

					glm::vec3 blockGridSize(0.0f);
					if (BlockGrid* bg = closestCelestialBody->getBlockGrid())
					{
						blockGridSize = { bg->getBlockGridSize().x * bg->getChunkSize(), bg->getBlockGridSize().y * bg->getChunkSize(), bg->getBlockGridSize().z * bg->getChunkSize() };
						setCurrentBlockGrid(bg);
					}

					const glm::vec3 offset = blockGridSize / 2.0f;

					setLocalPosition(
						((getDerivedPosition(currentCs, false) * 1000.0f)
							-
							closestCelestialBody->getDerivedPosition(currentCs, false) * 1000.0f + offset), csToEnter, false);
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
	primaryBody = body;
}

CelestialBody* TransformComponent::getPrimaryBody() const
{
	return primaryBody;
}
