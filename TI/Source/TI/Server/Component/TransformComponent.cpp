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
	// drawDebugPoint(getPosition(), { 1.0f, 0.0f, 0.0f, 1.0f }, 50.0f, false);

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

//std::optional<OrientationInfo> TransformComponent::getOrientationInfo() const
//{
//	std::optional<OrientationInfo> result;
//
//	if (blockGrid)
//	{
//		const std::vector<OrientationInfo> orientations { Orientations::TOP, Orientations::BOTTOM, Orientations::RIGHT, Orientations::LEFT, Orientations::FRONT, Orientations::BACK };
//		for (const OrientationInfo& orientationInfo : orientations)
//		{
//			if (isInCone(getLocalPosition(), orientationInfo))
//			{
//				result = orientationInfo;
//			}
//		}
//	}
//
//	return result;
//}

void TransformComponent::setTargetLocalOrientation(const glm::quat& orientation, CoordinateSystem cs)
{
	if (!orientationInProgress)
	{
		orientationInProgress = true;

		targetOrientation = orientation;
		targetCs = cs;
	}
}

//bool TransformComponent::isInCone(const glm::vec3& position, const OrientationInfo& orientationInfo) const
//{
//	if (BlockGrid* plane = getCurrentBlockGrid())
//	{
//		float planetSize = plane->getBlockGridSize().x * plane->getChunkSize();
//
//		return orientationInfo.positive ?
//			position[orientationInfo.sideAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] >= planetSize - position[orientationInfo.heightAxis] &&
//			position[orientationInfo.frontAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] >= planetSize - position[orientationInfo.heightAxis]
//			:
//			position[orientationInfo.sideAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] <= planetSize - position[orientationInfo.heightAxis] &&
//			position[orientationInfo.frontAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] <= planetSize - position[orientationInfo.heightAxis];
//	}
//
//	return false;
//}
