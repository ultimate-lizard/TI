#include "TransformComponent.h"

#include <TI/Server/BlockGrid.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Server/Entity.h>

TransformComponent::TransformComponent() :
	Component(),
	SceneNode(),
	plane(nullptr)
{
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	Component(other),
	SceneNode(other),
	plane(other.plane)
{
}

void TransformComponent::tick(float dt)
{

}

void TransformComponent::setPlane(BlockGrid* plane)
{
	this->plane = plane;
}

BlockGrid* TransformComponent::getPlane() const
{
	return plane;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

std::optional<OrientationInfo> TransformComponent::getOrientationInfo() const
{
	std::optional<OrientationInfo> result;

	if (plane)
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
	if (BlockGrid* plane = getPlane())
	{
		float planetSize = plane->getSize().x * plane->getChunkSize();

		return orientationInfo.positive ?
			position[orientationInfo.sideAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] >= planetSize - position[orientationInfo.heightAxis] &&
			position[orientationInfo.frontAxis] <= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] >= planetSize - position[orientationInfo.heightAxis]
			:
			position[orientationInfo.sideAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.sideAxis] <= planetSize - position[orientationInfo.heightAxis] &&
			position[orientationInfo.frontAxis] >= position[orientationInfo.heightAxis] && position[orientationInfo.frontAxis] <= planetSize - position[orientationInfo.heightAxis];
	}

	return false;
}
