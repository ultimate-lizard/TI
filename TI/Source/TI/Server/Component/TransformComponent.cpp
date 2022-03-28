#include "TransformComponent.h"

#include <TI/Server/Plane.h>
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

void TransformComponent::setPlane(Plane* plane)
{
	this->plane = plane;
}

Plane* TransformComponent::getPlane() const
{
	return plane;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

std::optional<OrientationInfo> TransformComponent::getOrientationInfo() const
{
	if (plane)
	{
		if (const Chunk* chunk = plane->getChunk(plane->positionToChunkPosition(position)))
		{
			std::optional<OrientationInfo> orientationInfo = chunk->getOrientationInfo(plane->positionToChunkLocalPosition(position));
			if (orientationInfo.has_value())
			{
				return orientationInfo;
			}
		}
	}

	return OrientationInfo{ 0, 1, 2, true };
}
