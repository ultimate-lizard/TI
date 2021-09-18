#pragma once

#include <memory>
#include <vector>

class Entity;

class Component
{
	friend class Entity;

public:
	Component();
	Component(const Component&);
	Component(Component&&) = delete;
	virtual ~Component() = default;

	virtual void init() {}
	virtual void tick(float dt) {}

	virtual std::unique_ptr<Component> clone() const = 0;

protected:
	Entity* entity;
};
