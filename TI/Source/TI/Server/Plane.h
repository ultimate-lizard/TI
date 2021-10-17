#pragma once

#include <TI/Server/Chunk.h>

#include <glm/glm.hpp>

#include <vector>

class Plane
{
public:
	Plane(const glm::vec3& pos = glm::vec3(0.0f), const glm::ivec3& size = glm::ivec3(4, 4, 4), unsigned int chunkSize = DEFAULT_CHUNK_SIZE);
	Plane(const Plane&) = delete;
	Plane(Plane&&) = delete;

	const glm::ivec3& getSize() const;
	unsigned int getChunkSize() const;

	const std::vector<Chunk>& getChunks() const;

	void spawnRandomBlock();
	void spawnBlock(const glm::vec3& position, unsigned int blockType);

	unsigned int getBlock(const glm::vec3& pos) const;
	const Chunk* getChunk(const glm::vec3& pos) const;

	unsigned long long planePositionToChunkIndex(const glm::vec3& position) const;
	glm::uvec3 planePositionToChunkPosition(const glm::vec3& position) const;

	bool isPositionInPlaneBounds(const glm::vec3& position) const;

	void setGravityEnabled(bool gravityEnabled);
	bool isGravityEnabled() const;

private:
	glm::vec3 position;
	glm::ivec3 size;

	const unsigned int chunkSize;

	std::vector<Chunk> chunks;

	bool gravityEnabled;
};
