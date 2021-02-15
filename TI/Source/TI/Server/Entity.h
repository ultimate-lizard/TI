#pragma once

#include <vector>
#include <memory>
#include <typeinfo>
#include <string>

#include <TI/Server/Component/Component.h>

class Entity
{
public:
	template <class AddedComponent, typename ... Args>
	inline void addComponent(Args&& ... args)
	{
		if (!findComponent<AddedComponent>())
		{
			auto component = std::make_unique<AddedComponent>(std::forward<Args>(args) ...);
			component->entity = this;
			components.push_back(std::move(component));
		}
	}

	template <class SearchedComponent>
	inline SearchedComponent* findComponent()
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

	inline void tick(float dt)
	{
		for (auto& component : components)
		{
			component->tick(dt);
		}
	}

	inline void setName(const std::string& name)
	{
		this->name = name;
	}

	inline std::string getName() const
	{
		return name;
	}

private:
	std::vector<std::unique_ptr<Component>> components;
	std::string name;
};
