#pragma once

#include <memory>

#include <TI/Server/Component/Component.h>

class Camera;

class CameraComponent : public Component
{
public:
	CameraComponent() = default;
	CameraComponent(const CameraComponent& otherCameraComp);

	void setCamera(std::unique_ptr<Camera> camera);
	Camera* getCamera() const;

	void operator=(const CameraComponent& otherCameraComp);

	std::unique_ptr<Component> clone() const override;

private:
	std::unique_ptr<Camera> camera;
};
