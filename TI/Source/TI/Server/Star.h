#pragma once

#include <memory>

#include <TI/Server/AstroBody.h>

class Planet;

class Star : public AstroBody
{
public:
	Star();
	~Star();

	void addPlanet(std::unique_ptr<Planet> planet);
	const std::vector<std::unique_ptr<Planet>>& getPlanets();

	// TODO: Remove this
	void initHomeStarSystem();
	void initRandomStar();

protected:
	std::vector<std::unique_ptr<Planet>> orbitingPlanets;
};
