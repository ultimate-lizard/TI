#pragma once

class Entity;

class Component
{
public:
	Component(Entity* entity) : entity(entity) {}
	virtual ~Component() = default;

	virtual void tick(float dt) {}

	inline Entity* getEntity() { return entity; }

protected:
	Entity* entity;
};

