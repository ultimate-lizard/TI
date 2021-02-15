#pragma once

class Entity;

class Component
{
	friend class Entity;

public:
	virtual ~Component() = default;

	virtual void tick(float dt) {}

	inline Entity* getEntity() { return entity; }

protected:
	Entity* entity;
};

