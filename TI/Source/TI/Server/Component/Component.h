#pragma once

#include <memory>

class Entity;

class Component
{
	friend class Entity;

public:
	virtual ~Component() = default;

	virtual void tick(float dt) {}

	inline Entity* getEntity() { return entity; }

	virtual std::unique_ptr<Component> clone() const = 0;

protected:
	Entity* entity;
};
