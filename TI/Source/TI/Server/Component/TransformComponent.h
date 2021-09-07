#pragma once

#include <memory>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class Plane;

class TransformComponent : public Component, public SceneNode
{
public:
	TransformComponent();
	TransformComponent(Plane* plane);
	TransformComponent(const TransformComponent& other);

	std::unique_ptr<Component> clone() const override;

	void setPlane(Plane* plane);
	Plane* getPlane() const;

private:
	Plane* plane;
};
