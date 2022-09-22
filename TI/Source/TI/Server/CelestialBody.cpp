#include "CelestialBody.h"

CelestialBody::CelestialBody() :
    blockGrid(nullptr),
    orbitalSystem(nullptr)
{

}

CelestialBody::CelestialBody(BlockGrid* blockGrid) :
    blockGrid(blockGrid),
    orbitalSystem(nullptr)
{

}

BlockGrid* CelestialBody::getBlockGrid()
{
    return blockGrid;
}

void CelestialBody::setOrbitalProperties(const OrbitalProperties& newProperties)
{
    orbitalProperties = newProperties;
}

const OrbitalProperties& CelestialBody::getOrbitalProperties() const
{
    return orbitalProperties;
}

void CelestialBody::setOrbitalSystem(OrbitalSystem* newOrbitalSystem)
{
    orbitalSystem = newOrbitalSystem;
}

OrbitalSystem* CelestialBody::getOrbitalSystem()
{
    return orbitalSystem;
}
