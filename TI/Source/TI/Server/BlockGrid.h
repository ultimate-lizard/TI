#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <TI/Server/Chunk.h>
#include <TI/Server/Block.h>

static const size_t DEFAULT_CHUNK_SIZE = 16;

class BlockGrid
{
public:
	BlockGrid(const glm::uvec3& size, size_t chunkSize = DEFAULT_CHUNK_SIZE);
	BlockGrid(const BlockGrid&) = delete;
	BlockGrid(BlockGrid&&) = delete;

	const glm::uvec3& getSize() const;
	size_t getChunkSize() const;

	const std::vector<Chunk>& getChunks() const;

	void spawnRandomBlock();
	void spawnBlock(const glm::uvec3& position, unsigned int blockType);

	unsigned int getBlock(const glm::uvec3& position) const;
	unsigned int getNeighborBlock(const glm::uvec3& position, BlockFace direction) const;
	const Chunk* getChunk(const glm::uvec3& position) const;

	bool isPositionInGridBounds(const glm::uvec3& position) const;

	void setGravityEnabled(bool gravityEnabled);
	bool isGravityEnabled() const;

	glm::ivec3 positionToChunkPosition(const glm::vec3& position) const;

	// Converts position to chunk's local position
	glm::uvec3 positionToChunkLocalPosition(const glm::uvec3& position) const;

private:
	// glm::vec3 position;
	glm::uvec3 size;

	const size_t chunkSize;

	std::vector<Chunk> chunks;

	bool gravityEnabled;
};
