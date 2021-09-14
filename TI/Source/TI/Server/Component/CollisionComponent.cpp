#include "CollisionComponent.h"

#include <TI/Server/Component/TransformComponent.h>

CollisionComponent::CollisionComponent(TransformComponent* transformComponent) :
    transformComponent(transformComponent)
{
}

CollisionComponent::CollisionComponent(const CollisionComponent& other)
{
    transformComponent = other.transformComponent;
}

void CollisionComponent::setBoundingPrimitive(BoundingPrimitive boundingPrimitive)
{
    this->boundingPrimitive = boundingPrimitive;
}

BoundingPrimitive CollisionComponent::getBoundingPrimitive() const
{
    glm::vec3 min = boundingPrimitive.minSize;
    glm::vec3 max = boundingPrimitive.maxSize;

    if (transformComponent)
    {
        glm::vec3 position = transformComponent->getPosition();
        min = position;
        min.x += boundingPrimitive.minSize.x;
        min.y += boundingPrimitive.minSize.y;
        min.z += boundingPrimitive.minSize.z;

        max = position;
        max.x += boundingPrimitive.maxSize.x;
        max.y += boundingPrimitive.maxSize.y;
        max.z += boundingPrimitive.maxSize.z;
    }

    return { min, max };
}

std::unique_ptr<Component> CollisionComponent::clone() const
{
    return std::make_unique<CollisionComponent>(*this);
}
