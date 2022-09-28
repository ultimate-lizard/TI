#pragma once

#include <memory>

#include <TI/Server/CelestialBody.h>

class Planet;

class Star : public CelestialBody
{
public:
	Star();
	~Star();

	virtual void tick(float dt) override;

	void initRandomStarSystem();
};
