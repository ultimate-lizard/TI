#include "Star.h"

#include <time.h>
#include <iostream>

#include <TI/Server/Planet.h>

Star::Star() :
	AstroBody(CoordinateSystemScale::Interplanetary)
{
	// setPosition({ 0.0f, 200.0f, 0.0f }, getCoordinateSystemScale());
	setScale(glm::vec3(100.0f), getCoordinateSystemScale());

	// Load info about planet
	auto body = std::make_unique<Planet>(glm::uvec3(10), 16);

	body->setScale(glm::vec3(13.0f), coordinateSystem);

	body->setPosition({ getPosition(coordinateSystem).x, getPosition(coordinateSystem).y, getPosition(coordinateSystem).z }, coordinateSystem);

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
