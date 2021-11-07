#include "Chunk.h"

#include <TI/Util/Utils.h>

Chunk::Chunk(size_t size, const glm::vec3& position) :
	size(size),
	position(position)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, 1);
}

unsigned int Chunk::getBlock(const glm::uvec3& pos) const
{
	return blocks[utils::positionToIndex(pos, {size, size, size})];
}

unsigned int Chunk::getBlock(size_t index) const
{
	return blocks[index];
}

const std::vector<unsigned int>& Chunk::getBlocks() const
{
	return blocks;
}

const glm::vec3& Chunk::getPosition() const
{
	return position;
}

size_t Chunk::getSize() const
{
	return size;
}

//unsigned int Chunk::planePositionToBlockIndex(const glm::vec3& position) const
//{
//	glm::uvec3 iPosition = position;
//
//	iPosition.x %= size;
//	iPosition.y %= size;
//	iPosition.z %= size;
//
//	return (iPosition.z * size * size) + (iPosition.y * size) + iPosition.x;
//}

void Chunk::setBlock(size_t index, unsigned int blockType)
{
	blocks[index] = blockType; 
}

void Chunk::setBlock(const glm::uvec3& position, unsigned int blockType)
{
	blocks[utils::positionToIndex(position, { size, size, size })] = blockType;
}
