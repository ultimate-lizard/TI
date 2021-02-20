#include "CameraComponent.h"

#include <TI/Renderer/Camera.h>

CameraComponent::CameraComponent(const CameraComponent& otherCameraComp)
{
	camera = std::make_unique<Camera>(*otherCameraComp.camera);
}

void CameraComponent::setCamera(std::unique_ptr<Camera> camera)
{
	this->camera = std::move(camera);
}

Camera* CameraComponent::getCamera() const
{
	return camera.get();
}

std::unique_ptr<Component> CameraComponent::clone() const
{
	return std::make_unique<CameraComponent>(*this);
}

void CameraComponent::operator=(const CameraComponent& otherCameraComp)
{
	camera = std::make_unique<Camera>(*otherCameraComp.camera);
}
