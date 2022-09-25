#pragma once

#include <vector>
#include <optional>

#include <glm/glm.hpp>

#include <TI/Server/PlaneSide.h>

class Chunk
{
public:
	Chunk(size_t size, const glm::vec3& position);

	unsigned int getBlock(size_t index) const;
	unsigned int getBlock(const glm::uvec3& position) const;

	void setBlock(size_t index, unsigned int blockType);
	void setBlock(const glm::uvec3& position, unsigned int blockType);

	const std::vector<unsigned int>& getBlocks() const;

	const glm::vec3& getPosition() const;
	size_t getSize() const;

	bool isInCore() const;
	bool isInCrust() const;
	bool isInTroposphere() const;

	glm::vec3 getUpDirection(const glm::vec3& localPosition) const;

private:
	glm::vec3 getSideNormal(const glm::vec3& position);

	const size_t size;
	glm::vec3 chunkPosition;

	// Blocks
	std::vector<unsigned int> blocks;

	// std::vector<OrientationInfo> orientations;
};
