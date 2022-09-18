#include "Star.h"

#include <time.h>
#include <iostream>

#include <TI/Server/Planet.h>

Star::Star() :
	AstroBody()
{
	// setPosition({ 0.0f, 200.0f, 0.0f }, CoordinateSystem::Interplanetary);
	setLocalScale(glm::vec3(100.0f), CoordinateSystem::Interplanetary);

	// Load info about primary planet
	auto body = std::make_unique<Planet>(glm::uvec3(10), 16);

	body->setLocalScale(glm::vec3(0.08f), CoordinateSystem::Interplanetary);
	body->rotate(25.0f, { 0.0f, 0.0f, 1.0f }, CoordinateSystem::Interplanetary);

	// body->setLocalPosition({ getPosition(CoordinateSystem::Interplanetary).x + 700.0f, getPosition(CoordinateSystem::Interplanetary).y, getPosition(CoordinateSystem::Interplanetary).z}, CoordinateSystem::Interplanetary);

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
