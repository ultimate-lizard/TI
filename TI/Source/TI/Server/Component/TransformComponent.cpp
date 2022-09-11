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

std::optional<OrientationInfo> TransformComponent::getOrientationInfo() const
{
	std::optional<OrientationInfo> result;

	if (blockGrid)
	{
		const std::vector<OrientationInfo> orientations { Orientations::TOP, Orientations::BOTTOM, Orientations::RIGHT, Orientations::LEFT, Orientations::FRONT, Orientations::BACK };
		for (const OrientationInfo& orientationInfo : orientations)
		{
			if (isInCone(getPosition(), orientationInfo))
			{
				result = orientationInfo;
			}
		}
	}

	return result;
}

bool TransformComponent::isInCone(const glm::vec3& position, const OrientationInfo& orientationInfo) const
{
	if (BlockGrid* plane = getCurrentBlockGrid())
	{
		float planetSize = plane->getBlockGridSize().x * plane->getChunkSize();

		return orientationInfo.positive ?
			position[orientationInfo.sideAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] >= planetSize - position[orientationInfo.heightAxis] &&
			position[orientationInfo.frontAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] >= planetSize - position[orientationInfo.heightAxis]
			:
			position[orientationInfo.sideAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] <= planetSize - position[orientationInfo.heightAxis] &&
			position[orientationInfo.frontAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] <= planetSize - position[orientationInfo.heightAxis];
	}

	return false;
}
