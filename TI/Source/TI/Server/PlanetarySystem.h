#pragma once

#include <TI/Server/OrbitalSystem.h>

class Planet;

class PlanetarySystem : public OrbitalSystem
{
public:
	PlanetarySystem(Planet* primaryBody);
};