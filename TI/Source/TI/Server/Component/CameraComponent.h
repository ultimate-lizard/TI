#pragma once

#include <memory>

#include <TI/Server/Component/Component.h>

class Camera;

class CameraComponent : public Component
{
public:
	void setCamera(std::unique_ptr<Camera> camera);
	Camera* getCamera() const;

private:
	std::unique_ptr<Camera> camera;
};
