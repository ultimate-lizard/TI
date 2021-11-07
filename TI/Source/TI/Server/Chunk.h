#pragma once

#include <vector>

#include <glm/glm.hpp>

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

	// unsigned int planePositionToBlockIndex(const glm::vec3& position) const;

private:
	const size_t size;
	glm::vec3 position;

	// Blocks
	std::vector<unsigned int> blocks;
};
