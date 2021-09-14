#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class TransformComponent;

struct BoundingPrimitive
{
	glm::vec3 minSize = glm::vec3(-0.5f);
	glm::vec3 maxSize = glm::vec3(0.5f);
};

class CollisionComponent : public Component
{
public:
	CollisionComponent(TransformComponent* transformComponent);
	CollisionComponent(const CollisionComponent& other);

	void setBoundingPrimitive(BoundingPrimitive boundingPrimitive);
	BoundingPrimitive getBoundingPrimitive() const;

	std::unique_ptr<Component> clone() const override;

private:
	TransformComponent* transformComponent;
	BoundingPrimitive boundingPrimitive;
};
