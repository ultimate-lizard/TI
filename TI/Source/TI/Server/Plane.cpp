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
