#pragma once

#include <TI/Server/Chunk.h>

#include <glm/glm.hpp>

#include <vector>

class Plane
{
public:
	Plane(const glm::vec3& pos = glm::vec3(0.0f), unsigned int chunkSize = DEFAULT_CHUNK_SIZE);
	Plane(const Plane&) = delete;
	Plane(Plane&&) = delete;

	const std::vector<Chunk>& getChunks() const;

	void spawnRandomBlock();
	void spawnBlock(glm::ivec3 position, unsigned int blockType);

	unsigned int getBlock(const glm::vec3& pos) const;

private:
	glm::vec3 pos;
	const unsigned int chunkSize;

	std::vector<Chunk> chunks;
};
