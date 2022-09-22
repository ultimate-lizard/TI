#pragma once

#include <memory>

#include <TI/Server/SceneNode.h>

class BlockGrid;
class OrbitalSystem;

struct OrbitalProperties
{
	float radius { 0.0f }; 
	/* Orbital velocity in angles per second */
	float orbitalVelocity { 0.0f };
	/* Equatorial velocity in angles per second */
	float equatorialVelocity { 0.0f };
};

class CelestialBody : public SceneMultiNode
{
public:
	CelestialBody();
	CelestialBody(BlockGrid* blockGrid = nullptr);

	BlockGrid* getBlockGrid();

	void setOrbitalProperties(const OrbitalProperties& newProperties);
	const OrbitalProperties& getOrbitalProperties() const;

	void setOrbitalSystem(OrbitalSystem* newOrbitalSystem);
	OrbitalSystem* getOrbitalSystem();

protected:
	BlockGrid* blockGrid;
	OrbitalProperties orbitalProperties;
	OrbitalSystem* orbitalSystem;
};
