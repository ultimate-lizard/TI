#include "Chunk.h"

#include <iostream>

#include <noise/noise.h>

#include <TI/Util/Utils.h>

Chunk::Chunk(size_t size, const glm::vec3& position) :
	size(size),
	position(position)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, position.y <= 2 * 16 ? 1 : 0);
	// blocks.resize(longSize * longSize * longSize, 1);

	/*if (position.y == 0)
	{
		for (int i = 0; i < 4096; ++i)
		{
			blocks[rand() % 4096] = 0;
		}
	}*/

	static noise::module::Perlin perlinModule;

	for (unsigned short i = 0; i < 4096; ++i)
	{
		glm::vec3 pos = utils::indexToPosition(i, { 16, 16, 16 });
		glm::vec3 perlinPos(pos.x + position.x, pos.y + position.y, pos.z + position.z);

		static const float scale = 100.0f;

		perlinPos.x = perlinPos.x / scale;
		perlinPos.y = perlinPos.y / scale;
		perlinPos.z = perlinPos.z / scale;

		float height = 16 * perlinModule.GetValue(perlinPos.x, 0.0f, perlinPos.z);
		blocks[i] = pos.y + position.y - 16.0f > height ? 0 : 1;
		if (pos.y + position.y == 0.0f)
		{
			blocks[i] = 1;
		}
	}
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
