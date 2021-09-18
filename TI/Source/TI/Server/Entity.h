#pragma once

#include <vector>
#include <memory>
#include <typeinfo>
#include <string>
#include <iostream>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class TransformComponent;

class Entity
{
public:
	Entity() = default;
	Entity(const Entity&) = delete;
	Entity(Entity&& otherEntity) = delete;

	void initComponents();

	template <class AddedComponent, typename ... Args>
	inline AddedComponent* addComponent(Args&& ... args)
	{
		if (!findComponent<AddedComponent>())
		{
			auto component = std::make_unique<AddedComponent>(std::forward<Args>(args) ...);
			component->entity = this;
			components.push_back(std::move(component));

			return static_cast<AddedComponent*>(components.back().get());
		}

		return nullptr;
	}

	void addComponent(std::unique_ptr<Component> component);

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

	inline void setName(const std::string& name) { this->name = name; }
	inline std::string getName() const { return name; }
	inline void setId(const std::string& id) { this->id = id; }
	inline std::string getId() const { return id; }

	std::unique_ptr<Entity> clone() const;

private:
	std::vector<std::unique_ptr<Component>> components;
	std::string name;
	std::string id;
};
