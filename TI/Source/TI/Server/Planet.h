#pragma once

#include <memory>

#include <TI/Server/AstroBody.h>

class BlockGrid;

class Planet : public AstroBody
{
public:
	Planet(const glm::uvec3& size, size_t chunkSize);
	Planet(std::unique_ptr<BlockGrid> plane);
	~Planet();

	BlockGrid* getPlane();

private:
	std::unique_ptr<BlockGrid> plane;
};
