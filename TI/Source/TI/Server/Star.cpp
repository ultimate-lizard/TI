#include "Star.h"

#include <time.h>
#include <iostream>

#include <TI/Server/Planet.h>

Star::Star() :
	AstroBody()
{
	
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

void Star::initHomeStarSystem()
{
	setLocalScale(glm::vec3(100.0f), CoordinateSystem::Interplanetary);
	setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar);

	// Load info about primary planet
	auto body = std::make_unique<Planet>(glm::uvec3(10), 16);

	body->setLocalScale(glm::vec3(0.08f), CoordinateSystem::Interplanetary);
	body->rotate(25.0f, { 0.0f, 0.0f, 1.0f }, CoordinateSystem::Interplanetary);

	// body->setLocalPosition({ getPosition(CoordinateSystem::Interplanetary).x + 700.0f, getPosition(CoordinateSystem::Interplanetary).y, getPosition(CoordinateSystem::Interplanetary).z}, CoordinateSystem::Interplanetary);

	addPlanet(std::move(body));
}

void Star::initRandomStar()
{
	setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar); // 100 km in size

	const float LO = -100.0f;
	const float HI = 100.0f;

	float x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

	setLocalPosition({ x, y, z }, CoordinateSystem::Interstellar);
}
