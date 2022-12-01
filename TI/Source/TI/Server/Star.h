#pragma once

#include <memory>

#include <TI/Server/CelestialBody.h>

class Planet;

class Star : public CelestialBody
{
public:
	Star();
	virtual ~Star() = default;

	virtual CoordinateSystem getContainedCoordinateSystem() const override;

	// void initRandomStarSystem();
};
