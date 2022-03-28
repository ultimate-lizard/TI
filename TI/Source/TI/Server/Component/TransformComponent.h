#pragma once

#include <memory>
#include <optional>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>
#include <TI/Server/PlaneSide.h>

class Plane;

class TransformComponent : public Component, public SceneNode
{
public:
	TransformComponent();
	TransformComponent(const TransformComponent& other);
	TransformComponent(TransformComponent&&) = delete;

	void tick(float dt) override;

	void setPlane(Plane* plane);
	Plane* getPlane() const;

	std::unique_ptr<Component> clone() const override;

	std::optional<OrientationInfo> getOrientationInfo() const;

private:
	Plane* plane;
};
