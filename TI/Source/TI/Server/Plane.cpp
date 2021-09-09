#include "Plane.h"

Plane::Plane(const glm::vec3& pos, unsigned int chunkSize) :
	pos(pos),
	chunkSize(chunkSize)
{
	for (int i = 0; i < 1; ++i)
	{
		for (int y = 0; y < 1; ++y)
		{
			chunks.emplace_back(chunkSize, glm::vec3(static_cast<float>(i * chunkSize), static_cast<float>(y * chunkSize), 0.0f));
		}
	}
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

void Plane::spawnBlock(glm::ivec3 position, unsigned int blockType)
{
	if (!chunks.empty())
	{
		Chunk& chunk = chunks[0];
		chunk.setBlock(position, blockType);
	}
}

unsigned int Plane::getBlock(const glm::vec3& pos) const
{
	if (pos.x < 0.f || pos.y < 0.f || pos.z < 0.f)
	{
		return 0;
	}

	int cx = pos.x / chunkSize; // + (pos.x % chunkSize);
	int cy = pos.y / chunkSize; // + (pos.y % chunkSize);
	int cz = pos.z / chunkSize; // + (pos.z % chunkSize);

	long long index = (cz * 4 * 4) + (cy * 4) + cx;

	if (chunks.size() - 1 >= index)
	{
		glm::ivec3 positive = pos;
		positive.x = cx + positive.x % chunkSize;
		positive.y = cy + positive.y % chunkSize;
		positive.z = cz + positive.z % chunkSize;

		// make sure we're getting the right block from chunk
		
		return chunks[index].getBlock(positive);
	}

	return 0;
}
