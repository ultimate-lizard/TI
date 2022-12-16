#include "Entity.h"

#include <map>

#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Renderer/Camera.h>

void Entity::initComponents()
{
	for (std::unique_ptr<Component>& component : components)
	{
		component->init();
	}
}

void Entity::addComponent(std::unique_ptr<Component> component)
{
	components.push_back(std::move(component));
}

std::unique_ptr<Entity> Entity::clone() const
{
	auto clonedEntity = std::make_unique<Entity>();
	clonedEntity->setName(name);

	// Key: pointer to original component. Value: component clone
	std::map<Component*, std::unique_ptr<Component>> clonedComponents;

	for (const std::unique_ptr<Component>& checkedComponent : components)
	{
		if (auto checkedNode = dynamic_cast<SceneNode*>(checkedComponent.get()))
		{
			std::unique_ptr<Component> clonedComponent;
			if (auto iter = clonedComponents.find(checkedComponent.get()); iter != clonedComponents.end())
			{
				clonedComponent = std::move(iter->second);
			}
			else
			{
				clonedComponent = checkedComponent->clone();
				clonedComponents.emplace(checkedComponent.get(), nullptr);
			}

			auto clonedSceneNode = dynamic_cast<SceneNode*>(clonedComponent.get());
			for (const std::unique_ptr<Component>& checkedInnerComponent : components)
			{
				if (auto checkedInnerNode = dynamic_cast<SceneNode*>(checkedInnerComponent.get()))
				{
					if (checkedInnerNode->isChildOf(checkedNode) && checkedComponent.get() != checkedInnerComponent.get())
					{
						std::unique_ptr<Component> clonedChild;
						if (auto iter = clonedComponents.find(checkedInnerComponent.get()); iter != clonedComponents.end())
						{
							clonedChild = std::move(iter->second);
						}
						else
						{
							clonedChild = checkedInnerComponent->clone();
							clonedComponents.emplace(checkedInnerComponent.get(), nullptr);
						}

						auto clonedChildSceneNode = dynamic_cast<SceneNode*>(clonedChild.get());
						// clonedSceneNode->addChild(clonedChildSceneNode);
						clonedChildSceneNode->setParent(clonedSceneNode);
						clonedComponents[checkedInnerComponent.get()] = std::move(clonedChild);
					}
				}
			}

			clonedComponents[checkedComponent.get()] = std::move(clonedComponent);
		}
		else
		{
			clonedEntity->addComponent(checkedComponent->clone());
		}
	}

	for (auto& pair : clonedComponents)
	{
		clonedEntity->addComponent(std::move(pair.second));
	}

	for (auto& c : clonedEntity->components)
	{
		c->entity = clonedEntity.get();
	}

	return clonedEntity;
}
