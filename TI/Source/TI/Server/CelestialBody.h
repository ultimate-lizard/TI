#pragma once

#include <memory>

#include <TI/Server/SceneNode.h>

class BlockGrid;

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
	virtual ~CelestialBody() = default;

	virtual void tick(float dt);

	BlockGrid* getBlockGrid();

	void setOrbitalProperties(const OrbitalProperties& newProperties);
	const OrbitalProperties& getOrbitalProperties() const;

	void addSattelite(CelestialBody* orbitalSystem);
	const std::vector<CelestialBody*>& getSattelites() const;

	CelestialBody* getOrbitalParent() const;
	CelestialBody* getHierarchicalParent() const;

	virtual CoordinateSystem getContainedCoordinateSystem() const = 0;

protected:
	CelestialBody(CoordinateSystem minimalCs, BlockGrid* blockGrid = nullptr);

	BlockGrid* blockGrid;
	OrbitalProperties orbitalProperties;
	std::vector<CelestialBody*> sattelites;
	CelestialBody* orbitalParent;
};
