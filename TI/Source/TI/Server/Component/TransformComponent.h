#pragma once

#include <memory>
#include <optional>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class BlockGrid;

class TransformComponent : public Component, public SceneMultiNode
{
public:
	TransformComponent();
	TransformComponent(const TransformComponent& other);
	TransformComponent(TransformComponent&&) = delete;

	void tick(float dt) override;

	void setCurrentBlockGrid(BlockGrid* plane);
	BlockGrid* getCurrentBlockGrid() const;

	std::unique_ptr<Component> clone() const override;

	void setTargetLocalOrientation(const glm::quat& orientation, CoordinateSystem cs);

	glm::quat lastOrientation;

private:
	BlockGrid* blockGrid;

	bool orientationInProgress = false;
	glm::quat targetOrientation;
	CoordinateSystem targetCs = CoordinateSystem::Planetary;
};
