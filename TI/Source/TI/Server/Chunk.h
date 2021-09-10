#pragma once

#include <vector>

#include <glm/glm.hpp>

static const unsigned int DEFAULT_CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(unsigned int size = DEFAULT_CHUNK_SIZE, const glm::vec3& pos = glm::vec3(0.0f));

	unsigned int getBlock(unsigned int index) const;
	unsigned int getBlock(const glm::vec3& pos) const;

	void setBlock(unsigned int index, unsigned int blockType);
	void setBlock(const glm::vec3& position, unsigned int blockType);

	const std::vector<unsigned int>& getBlocks() const;

	const glm::vec3& getPosition() const;
	unsigned int getSize() const;

	unsigned int planePositionToBlockIndex(const glm::vec3& position) const;

private:
	const unsigned int size;
	glm::vec3 pos;

	// Blocks
	std::vector<unsigned int> blocks;
};
