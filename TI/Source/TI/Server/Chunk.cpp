#include "Chunk.h"

Chunk::Chunk(unsigned int size, const glm::vec3& pos) :
	size(size),
	pos(pos)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, 1);
}

unsigned int Chunk::getBlock(const glm::uvec3& pos)
{
	return blocks[(pos.z * size * size) + (pos.y * size) + pos.x];
}

unsigned int Chunk::getBlock(unsigned int index)
{
	return blocks[index];
}

const std::vector<unsigned int>& Chunk::getBlocks() const
{
	return blocks;
}

const glm::vec3& Chunk::getPosition() const
{
	return pos;
}

unsigned int Chunk::getSize() const
{
	return size;
}