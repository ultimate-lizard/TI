#pragma once

#include <vector>

class CelestialBody;

class OrbitalSystem
{
public:
	OrbitalSystem(CelestialBody* primaryBody);

	void addOrbitalSystem(OrbitalSystem* orbitalSystem);

	void tick(float dt);

	CelestialBody* getPrimaryBody();
	const std::vector<OrbitalSystem*>& getOrbitingSystems() const;

protected:
	CelestialBody* primaryBody;
	std::vector<OrbitalSystem*> orbitingSystems;
};
