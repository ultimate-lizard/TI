#include "TransformComponent.h"

#include <TI/Server/BlockGrid.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Server/Entity.h>

TransformComponent::TransformComponent() :
	Component(),
	SceneMultiNode(),
	blockGrid(nullptr)
{
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	Component(other),
	SceneMultiNode(other),
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
