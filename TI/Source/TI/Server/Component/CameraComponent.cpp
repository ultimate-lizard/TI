#include "CameraComponent.h"

#include <TI/Renderer/Camera.h>

CameraComponent::CameraComponent(Entity* entity) :
	Component(entity)
{

}

void CameraComponent::setCamera(std::unique_ptr<Camera> camera)
{
	this->camera = std::move(camera);
}

Camera* CameraComponent::getCamera() const
{
	return camera.get();
}
