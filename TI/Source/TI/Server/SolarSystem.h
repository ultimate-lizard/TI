#pragma once

#include <TI/Server/OrbitalSystem.h>

class Star;

class SolarSystem : public OrbitalSystem
{
public:
	SolarSystem(Star* primaryBody);

	void initRandom();
};
