#include "CameraComponent.h"

#include <TI/Renderer/Camera.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

CameraComponent::CameraComponent() :
	Component(),
	SceneNode()
{
}

CameraComponent::CameraComponent(const CameraComponent& other) :
	Component(other),
	SceneNode(other),
	camera(std::make_unique<Camera>(*other.camera))
{
	camera->setParent(this);
}

std::unique_ptr<Component> CameraComponent::clone() const
{
	return std::make_unique<CameraComponent>(*this);
}

void CameraComponent::setCamera(std::unique_ptr<Camera> camera)
{
	this->camera = std::move(camera);
}

Camera* CameraComponent::getCamera() const
{
	return camera.get();
}
