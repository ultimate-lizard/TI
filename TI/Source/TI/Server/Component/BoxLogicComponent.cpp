#include "BoxLogicComponent.h"

#include <TI/Server/Server.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/MeshComponent.h>

BoxLogicComponent::BoxLogicComponent(Server* server) :
	server(server)
{

}

void BoxLogicComponent::tick(float dt)
{
	if (!server || !entity)
	{
		return;
	}

	auto transform = entity->findComponent<TransformComponent>();
	if (!transform)
	{
		return;
	}

	for (const auto& pair : server->getEntities())
	{
		if (Entity* targetEntity = pair.second.get())
		{
			if (targetEntity->getName() == "Player")
			{
				if (auto entityTransform = targetEntity->findComponent<TransformComponent>())
				{
					if (glm::distance(entityTransform->getDerivedPosition(), transform->getDerivedPosition()) <= 0.8f)
					{
						// TODO: Implement destroy
						if (auto mesh = entity->findComponent<MeshComponent>())
						{
							mesh->setVisibility(false);
						}
					}
				}
			}
		}
	}
}

std::unique_ptr<Component> BoxLogicComponent::clone() const
{
	return std::make_unique<BoxLogicComponent>(*this);
}
