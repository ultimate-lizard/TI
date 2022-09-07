#include "Planet.h"

#include <TI/Server/Plane.h>

Planet::Planet(const glm::uvec3& size, size_t chunkSize) :
	AstroBody(CoordinateSystemScale::Interplanetary)
{
	plane = std::make_unique<Plane>(glm::uvec3(10), 16);
}

Planet::Planet(std::unique_ptr<Plane> plane) :
	AstroBody(CoordinateSystemScale::Interplanetary),
	plane(std::move(plane))
{

}

Planet::~Planet()
{

}

Plane* Planet::getPlane()
{
	return plane.get();
}
