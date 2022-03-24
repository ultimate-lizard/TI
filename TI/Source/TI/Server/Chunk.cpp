#include "Chunk.h"

#include <TI/Util/Utils.h>

Chunk::Chunk(size_t size, const glm::vec3& position) :
	size(size),
	position(position)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, position.y == 0.0f ? 1 : 0);
	//if (position.y == 0)
	//{
	//	for (int i = 0; i < 4096; ++i)
	//	{
	//		blocks[rand() % 4096] = 0;
	//	}
	//}
	//blocks.resize(longSize * longSize * longSize, 1);
	//blocks[utils::positionToIndex({ 4, 5, 1 }, glm::uvec3(size))] = 0;
	//blocks[utils::positionToIndex({ 3, 5, 2 }, glm::uvec3(size))] = 0;
	//blocks[utils::positionToIndex({ 4, 5, 2 }, glm::uvec3(size))] = 0;
	//blocks[utils::positionToIndex({ 0, 5, 5 }, glm::uvec3(size))] = 0;
	//blocks[utils::positionToIndex({ 2, 5, 5 }, glm::uvec3(size))] = 0;
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
