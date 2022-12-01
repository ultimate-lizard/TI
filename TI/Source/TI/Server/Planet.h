#pragma once

#include <memory>

#include <TI/Server/CelestialBody.h>

class BlockGrid;

class Planet : public CelestialBody
{
public:
	Planet();
	Planet(BlockGrid* blockGrid);

	Planet(const Planet&) = delete;
	Planet(Planet&&) = delete;

	virtual CoordinateSystem getContainedCoordinateSystem() const override;
};
