#pragma once

#include <glm/glm.hpp>

#include <vector>

static const unsigned int DEFAULT_CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(unsigned int size = DEFAULT_CHUNK_SIZE, const glm::vec3& pos = glm::vec3(0.0f));

	unsigned int getBlock(const glm::uvec3& pos);
	unsigned int getBlock(unsigned int index);

	const std::vector<unsigned int>& getBlocks() const;

	const glm::vec3& getPosition() const;
	unsigned int getSize() const;

	void setBlock(unsigned int index, unsigned int blockType);
	void setBlock(const glm::uvec3& position, unsigned int blockType);

private:
	const unsigned int size;
	glm::vec3 pos;

	// Blocks
	std::vector<unsigned int> blocks;
};
