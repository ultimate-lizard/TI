#include "CameraComponent.h"

#include <TI/Renderer/Camera.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

CameraComponent::CameraComponent() :
	Component(),
	SceneMultiNode(),
	camera(std::make_unique<Camera>())
{
	camera->setParent(this);
}

CameraComponent::CameraComponent(const CameraComponent& other) :
	Component(other),
	SceneMultiNode(other),
	camera(std::make_unique<Camera>(*other.camera))
{
	camera->setParent(this);
}

std::unique_ptr<Component> CameraComponent::clone() const
{
	return std::make_unique<CameraComponent>(*this);
}

//void CameraComponent::setCamera(std::unique_ptr<Camera> camera)
//{
//	this->camera = std::move(camera);
//	// camera->setParent(this);
//}

Camera* CameraComponent::getCamera() const
{
	return camera.get();
}
