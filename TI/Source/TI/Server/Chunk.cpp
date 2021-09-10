#include "Chunk.h"

Chunk::Chunk(unsigned int size, const glm::vec3& pos) :
	size(size),
	pos(pos)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, 1);
}

unsigned int Chunk::getBlock(const glm::vec3& pos) const
{
	return blocks[planePositionToBlockIndex(pos)];
}

unsigned int Chunk::getBlock(unsigned int index) const
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

unsigned int Chunk::planePositionToBlockIndex(const glm::vec3& position) const
{
	glm::uvec3 iPosition = position;

	iPosition.x %= size;
	iPosition.y %= size;
	iPosition.z %= size;

	return (iPosition.z * size * size) + (iPosition.y * size) + iPosition.x;
}

void Chunk::setBlock(unsigned int index, unsigned int blockType)
{
	blocks[index] = blockType; 
}

void Chunk::setBlock(const glm::vec3& position, unsigned int blockType)
{
	blocks[planePositionToBlockIndex(position)] = blockType;
}
