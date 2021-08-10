#pragma once

#include <memory>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class TransformComponent : public Component, public SceneNode
{
public:
	TransformComponent();
	TransformComponent(const TransformComponent& other);

	std::unique_ptr<Component> clone() const override;

private:

};
