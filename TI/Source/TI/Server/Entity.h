#pragma once

#include <vector>
#include <memory>
#include <typeinfo>

#include <TI/Server/Component/Component.h>

class Entity
{
public:
	template <class AddedComponent, typename ... Args>
	void addComponent(Args&& ... args)
	{
		if (!findComponent<AddedComponent>())
		{
			auto component = std::make_unique<AddedComponent>(std::forward<Args>(args) ...);
			component->entity = this;
			components.push_back(std::move(component));
		}
	}

	template <class SearchedComponent>
	SearchedComponent* findComponent()
	{
		for (auto& component : components)
		{
			auto castedComponent = dynamic_cast<SearchedComponent*>(component.get());
			if (castedComponent)
			{
				return castedComponent;
			}
		}

		return nullptr;
	}

	void tick(float dt)
	{
		for (auto& component : components)
		{
			component->tick(dt);
		}
	}

private:
	std::vector<std::unique_ptr<Component>> components;
};
