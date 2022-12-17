#include "TransformComponent.h"

#include <TI/Server/BlockGrid.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Server/Entity.h>
#include <TI/Server/LocalServer.h>
#include <TI/Server/Planet.h>
#include <TI/Server/Component/CameraComponent.h>

TransformComponent::TransformComponent(Server* server) :
	Component(),
	server(nullptr),
	blockGrid(nullptr)
{
	this->server = dynamic_cast<LocalServer*>(server);
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	Component(other),
	server(other.server),
	blockGrid(other.blockGrid)
{
}

void TransformComponent::tick(float dt)
{
	if (orientationInProgress)
	{
		setLocalOrientation(glm::mix(getLocalOrientation(), targetOrientation, dt * 15.0f));

		float matching = glm::dot(getLocalOrientation(), targetOrientation);
		if (abs(matching - 1.0f) < 0.00001f) {
			orientationInProgress = false;
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
