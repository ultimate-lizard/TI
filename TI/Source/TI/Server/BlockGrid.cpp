#include "BlockGrid.h"

#include <iostream>

#include <TI/Util/Utils.h>

BlockGrid::BlockGrid(const glm::uvec3& size, size_t chunkSize) :
	blockGridSize(size),
	chunkSize(chunkSize),
	gravityEnabled(true)
{
	for (unsigned int z = 0; z < size.z; ++z)
	{
		for (unsigned int y = 0; y < size.y; ++y)
		{
			for (unsigned int x = 0; x < size.x; ++x)
			{
				glm::vec3 chunkPosition;
				chunkPosition.x = static_cast<float>(x * chunkSize);
				chunkPosition.y = static_cast<float>(y * chunkSize);
				chunkPosition.z = static_cast<float>(z * chunkSize);
				chunks.emplace_back(chunkSize, chunkPosition);
			}
		}
	}
}

const glm::uvec3& BlockGrid::getBlockGridSize() const
{
	return blockGridSize;
}

size_t BlockGrid::getChunkSize() const
{
	return chunkSize;
}

const std::vector<Chunk>& BlockGrid::getChunks() const
{
	return chunks;
}

void BlockGrid::spawnRandomBlock()
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

void BlockGrid::spawnBlock(const glm::uvec3& position, unsigned int blockType)
{
	if (!isPositionInGridBounds(position) || chunks.empty())
	{
		return;
	}

	glm::uvec3 chunkPosition = positionToChunkPosition(position);

	Chunk& chunk = chunks[utils::positionToIndex(chunkPosition, blockGridSize)];
	chunk.setBlock({ position.x % chunkSize, position.y % chunkSize, position.z % chunkSize }, blockType);
}

unsigned int BlockGrid::getBlock(const glm::uvec3& position) const
{
	if (!isPositionInGridBounds(position))
	{
		return 0;
	}

	glm::uvec3 chunkPosition = positionToChunkPosition(position);

	const Chunk& chunk = chunks[utils::positionToIndex(chunkPosition, blockGridSize)];
	return chunk.getBlock({ position.x % chunkSize, position.y % chunkSize, position.z % chunkSize });
}

unsigned int BlockGrid::getNeighborBlock(const glm::uvec3& position, BlockFace direction) const
{
	glm::uvec3 blockPosition = position;

	switch (direction)
	{
	case BlockFace::Front:
		blockPosition.z += 1;
		break;
	case BlockFace::Back:
		blockPosition.z -= 1;
		break;
	case BlockFace::Left:
		blockPosition.x -= 1;
		break;
	case BlockFace::Right:
		blockPosition.x += 1;
		break;
	case BlockFace::Top:
		blockPosition.y += 1;
		break;
	case BlockFace::Bottom:
		blockPosition.y -= 1;
		break;
	}

	if (!isPositionInGridBounds(blockPosition))
	{
		return 0;
	}

	return getBlock(blockPosition);
}

const Chunk* BlockGrid::getChunk(const glm::uvec3& position) const
{
	if (!isPositionInGridBounds({position.x * chunkSize, position.y * chunkSize, position.z * chunkSize }))
	{
		return nullptr;
	}

	return &chunks[utils::positionToIndex(position, blockGridSize)];
}

bool BlockGrid::isPositionInGridBounds(const glm::uvec3& position) const
{
	if (position.x < 0.0f || position.y < 0.0f || position.z < 0.0f ||
		position.x >= blockGridSize.x * chunkSize || position.y >= blockGridSize.y * chunkSize || position.z >= blockGridSize.z * chunkSize)
	{
		return false;
	}

	return true;
}

void BlockGrid::setGravityEnabled(bool gravityEnabled)
{
	this->gravityEnabled = gravityEnabled;
}

bool BlockGrid::isGravityEnabled() const
{
	return gravityEnabled;
}

glm::ivec3 BlockGrid::positionToChunkPosition(const glm::vec3& position) const
{
	return glm::uvec3(
		position.x / chunkSize,
		position.y / chunkSize,
		position.z / chunkSize
	);
}

glm::uvec3 BlockGrid::positionToChunkLocalPosition(const glm::uvec3& position) const
{
	return glm::uvec3(
		position.x % chunkSize,
		position.y % chunkSize,
		position.z % chunkSize
	);
}
