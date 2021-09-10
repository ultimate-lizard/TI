#include "Plane.h"

#include <iostream>

Plane::Plane(const glm::vec3& pos, const glm::ivec3& size, unsigned int chunkSize) :
	position(pos),
	size(size),
	chunkSize(chunkSize)
{
	for (int z = 0; z < size.z; ++z)
	{
		for (int y = 0; y < size.y; ++y)
		{
			for (int x = 0; x < size.x; ++x)
			{
				chunks.emplace_back(chunkSize, glm::vec3(static_cast<float>(x * chunkSize), static_cast<float>(y * chunkSize), static_cast<float>(z * chunkSize)));
			}
		}
	}
}

const glm::ivec3& Plane::getSize() const
{
	return size;
}

unsigned int Plane::getChunkSize() const
{
	return chunkSize;
}

const std::vector<Chunk>& Plane::getChunks() const
{
	return chunks;
}

void Plane::spawnRandomBlock()
{
	if (!chunks.empty())
	{
		Chunk& chunk = chunks[0];
		unsigned int random = rand() % (chunkSize * chunkSize * chunkSize);
		if (chunk.getBlock(random) != 0)
		{
			chunk.setBlock(random, 0);
		}
	}
}

void Plane::spawnBlock(const glm::vec3& position, unsigned int blockType)
{
	if (!isPositionInPlaneBounds(position) || chunks.empty())
	{
		return;
	}


	Chunk& chunk = chunks[planePositionToChunkIndex(position)];
	std::cout << "Spawning at chunk: " << chunk.getPosition().x << " " << chunk.getPosition().y << " " << chunk.getPosition().z << std::endl;
	chunk.setBlock(position, blockType);
}

unsigned int Plane::getBlock(const glm::vec3& pos) const
{
	if (!isPositionInPlaneBounds(pos) || chunks.empty())
	{
		return 0;
	}

	const Chunk& chunk = chunks[planePositionToChunkIndex(pos)];
	return chunk.getBlock(chunk.planePositionToBlockIndex(pos));
}

unsigned long long Plane::planePositionToChunkIndex(const glm::vec3& position) const
{
	unsigned long long z = (static_cast<unsigned long long>(position.z) / chunkSize) * size.x * size.y;
	unsigned long long y = (static_cast<unsigned long long>(position.y) / chunkSize) * size.x;
	unsigned long long x = static_cast<unsigned long long>(position.x) / chunkSize;

	return x + y + z;
}

bool Plane::isPositionInPlaneBounds(const glm::vec3& position) const
{
	if (position.x < 0.0f || position.y < 0.0f || position.z < 0.0f ||
		position.x >= size.x * chunkSize || position.y >= size.y * chunkSize || position.z >= size.z * chunkSize)
	{
		return false;
	}

	return true;
}
