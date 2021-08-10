#include "CameraComponent.h"

#include <TI/Renderer/Camera.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

CameraComponent::CameraComponent()
{
}

CameraComponent::CameraComponent(const CameraComponent& otherCameraComponent) :
	Component(otherCameraComponent)
{
	camera = std::make_unique<Camera>(*otherCameraComponent.camera);
}

void CameraComponent::setCamera(std::unique_ptr<Camera> camera)
{
	this->camera = std::move(camera);
}

Camera* CameraComponent::getCamera() const
{
	return camera.get();
}

void CameraComponent::setParentEntity(Entity* const entity)
{
	Component::setParentEntity(entity);
	if (auto transformComponent = entity->findComponent<TransformComponent>())
	{
		camera->setParentNode(transformComponent);
	}
}

std::unique_ptr<Component> CameraComponent::clone() const
{
	return std::make_unique<CameraComponent>(*this);
}

void CameraComponent::operator=(const CameraComponent& otherCameraComp)
{
	camera = std::make_unique<Camera>(*otherCameraComp.camera);
}
