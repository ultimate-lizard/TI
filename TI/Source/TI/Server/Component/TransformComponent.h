#pragma once

#include <memory>
#include <optional>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>
#include <TI/Server/PlaneSide.h>

class BlockGrid;

class TransformComponent : public Component, public SceneNode
{
public:
	TransformComponent();
	TransformComponent(const TransformComponent& other);
	TransformComponent(TransformComponent&&) = delete;

	void tick(float dt) override;

	void setPlane(BlockGrid* plane);
	BlockGrid* getPlane() const;

	std::unique_ptr<Component> clone() const override;

	std::optional<OrientationInfo> getOrientationInfo() const;

private:
	bool isInCone(const glm::vec3& localPosition, const OrientationInfo& orientationInfo) const;

private:
	BlockGrid* plane;
};
