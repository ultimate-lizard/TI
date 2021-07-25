#pragma once

#include <memory>

class Entity;

class Component
{
	friend class Entity;

public:
	Component();
	Component(const Component&);
	virtual ~Component() = default;

	virtual void tick(float dt) {}

	virtual void setParentEntity(Entity* const);
	inline Entity* getParentEntity() { return entity; }

	virtual std::unique_ptr<Component> clone() const = 0;

protected:
	Entity* entity;
};
