#include "Planet.h"

#include <TI/Server/BlockGrid.h>

Planet::Planet(const glm::uvec3& size, size_t chunkSize) :
	AstroBody(CoordinateSystemScale::Interplanetary)
{
	plane = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
}

Planet::Planet(std::unique_ptr<BlockGrid> plane) :
	AstroBody(CoordinateSystemScale::Interplanetary),
	plane(std::move(plane))
{

}

Planet::~Planet()
{

}

BlockGrid* Planet::getPlane()
{
	return plane.get();
}
