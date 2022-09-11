#pragma once

#include <memory>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class Camera;

class CameraComponent : public Component, public SceneMultiNode
{
public:
	CameraComponent();
	CameraComponent(const CameraComponent& other);
	CameraComponent(CameraComponent&&) = delete;

	std::unique_ptr<Component> clone() const override;

	void setCamera(std::unique_ptr<Camera> camera);
	Camera* getCamera() const;

private:
	std::unique_ptr<Camera> camera;
};
