#pragma once

#include <memory>
#include <optional>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class BlockGrid;
class Server;
class LocalServer;
class CelestialBody;

class TransformComponent : public Component, public SceneNode
{
public:
	TransformComponent(Server* server);
	TransformComponent(const TransformComponent& other);
	TransformComponent(TransformComponent&&) = delete;

	void tick(float dt) override;

	void setCurrentBlockGrid(BlockGrid* plane);
	BlockGrid* getCurrentBlockGrid() const;

	std::unique_ptr<Component> clone() const override;

	void setTargetLocalOrientation(const glm::quat& orientation, CoordinateSystem cs);

	glm::quat lastOrientation;

	void setPrimaryBody(CelestialBody* body);
	CelestialBody* getPrimaryBody() const;

private:
	LocalServer* server;

	BlockGrid* blockGrid;

	bool orientationInProgress = false;
	glm::quat targetOrientation;
	CoordinateSystem targetCs = CoordinateSystem::Planetary;

	CelestialBody* primaryBody;
};
