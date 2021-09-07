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
		bool done = false;
		size_t tries = 0;
		while (!done || tries >= 10)
		{
			Chunk& chunk = chunks[0];
			unsigned int random = rand() % (chunkSize * chunkSize * chunkSize);
			if (chunk.getBlock(random) == 0)
			{
				done = true;
				chunk.setBlock(random, 1);
			}

			++tries;
		}
	}
}
