#include "Planet.h"

#include <TI/Server/BlockGrid.h>

Planet::Planet(const glm::uvec3& size, size_t chunkSize) :
	AstroBody()
{
	blockGrid = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
}

Planet::Planet(std::unique_ptr<BlockGrid> plane) :
	AstroBody(),
	blockGrid(std::move(plane))
{

}

Planet::~Planet()
{

}

BlockGrid* Planet::getBlockGrid()
{
	return blockGrid.get();
}
