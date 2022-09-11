#include "Star.h"

#include <time.h>
#include <iostream>

#include <TI/Server/Planet.h>

Star::Star() :
	AstroBody()
{
	setPosition({ 0.0f, 200.0f, 0.0f }, CoordinateSystem::Interplanetary);
	setScale(glm::vec3(100.0f), CoordinateSystem::Interplanetary);

	// Load info about primary planet
	auto body = std::make_unique<Planet>(glm::uvec3(10), 16);

	body->setScale(glm::vec3(10.0f), CoordinateSystem::Interplanetary);

	body->setPosition({ getPosition(CoordinateSystem::Interplanetary).x + 200.0f, getPosition(CoordinateSystem::Interplanetary).y, getPosition(CoordinateSystem::Interplanetary).z}, CoordinateSystem::Interplanetary);

	addPlanet(std::move(body));
}

Star::~Star()
{

}

void Star::addPlanet(std::unique_ptr<Planet> planet)
{
	orbitingPlanets.push_back(std::move(planet));
}

const std::vector<std::unique_ptr<Planet>>& Star::getPlanets()
{
	return orbitingPlanets;
}
