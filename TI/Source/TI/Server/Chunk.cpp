#include "Chunk.h"

#include <iostream>
#include <tuple>

#include <noise/noise.h>

#include <TI/Util/Utils.h>
#include <TI/Client/DebugInformation.h>

const float chunkSize = 16.0f;
const float planetSize = 10.0f * chunkSize;
const float core = 2.0f * chunkSize;
const float crust = 2.0f * chunkSize;
const float atmosphere = 2.0f * chunkSize;
const float total = core + crust + atmosphere;

Chunk::Chunk(size_t size, const glm::vec3& position) :
	size(size),
	chunkPosition(position)
{
	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, 0);

	static noise::module::RidgedMulti noiseModule;

	for (unsigned short i = 0; i < 4096; ++i)
	{
		glm::vec3 localPosition = utils::indexToPosition(i, { 16, 16, 16 });
		glm::vec3 absPosition = localPosition;
		absPosition.x += position.x;
		absPosition.y += position.y;
		absPosition.z += position.z;

		glm::vec3 perlinPos(localPosition.x + position.x, localPosition.y + position.y, localPosition.z + position.z);

		static const float scale = 150.0f;

		perlinPos.x = perlinPos.x / scale;
		perlinPos.y = perlinPos.y / scale;
		perlinPos.z = perlinPos.z / scale;
		
		if (isInTroposphere())
		{
			blocks[i] = 0;
		}

		if (isInCrust())
		{
			blocks[i] = 1;
		}
		continue;
		if (isInCrust() || isInTroposphere())
		{
			const float MAX_AMPLITUDE = 8; // How far under or above 0 the terrain will differ
			const float TERRAIN_LOCATION_HEIGHT = 2 * chunkSize; // From center
			//const float MAX_AMPLITUDE = 4; // How far under or above 0 the terrain will differ
			//const float TERRAIN_LOCATION_HEIGHT = chunkSize * 1; // From center
			
			const float generatedTerrainHeight = MAX_AMPLITUDE * noiseModule.GetValue(perlinPos.x, perlinPos.y, perlinPos.z);

			unsigned int block = 0;

			glm::vec3 normal = getSideNormal(absPosition);
			if (normal != glm::vec3(0.0f))
			{
				for (size_t axis = 0; axis < 3; ++axis)
				{
					if (normal[axis] > 0.0f)
					{
						blocks[i] = localPosition[axis] + chunkPosition[axis] - planetSize + TERRAIN_LOCATION_HEIGHT > generatedTerrainHeight ? 0 : 1;
						break;
					}
					else if (normal[axis] < 0.0f)
					{
						blocks[i] = localPosition[axis] + chunkPosition[axis] < TERRAIN_LOCATION_HEIGHT - generatedTerrainHeight - 1 ? 0 : 1;
						break;
					}
				}
			}
		}

		// if core
		if (isInCore())
		{
			blocks[i] = 1;
		}
	}
}

unsigned int Chunk::getBlock(const glm::uvec3& pos) const
{
	if (!blocks.empty())
	{
		return blocks[utils::positionToIndex(pos, { size, size, size })];
	}
	
	return 0;
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
	return chunkPosition;
}

size_t Chunk::getSize() const
{
	return size;
}

bool Chunk::isInCore() const
{
	return chunkPosition.x >= atmosphere + crust && chunkPosition.x < atmosphere + crust + core &&
		chunkPosition.y >= atmosphere + crust && chunkPosition.y < atmosphere + crust + core &&
		chunkPosition.z >= atmosphere + crust && chunkPosition.z < atmosphere + crust + core;
}

bool Chunk::isInCrust() const
{
	return !isInCore() && (chunkPosition.x >= atmosphere && chunkPosition.x < planetSize - atmosphere &&
		chunkPosition.y >= atmosphere && chunkPosition.y < planetSize - atmosphere &&
		chunkPosition.z >= atmosphere && chunkPosition.z < planetSize - atmosphere);
}

bool Chunk::isInTroposphere() const
{
	return chunkPosition.x >= 0.0f && chunkPosition.x < atmosphere ||
		chunkPosition.y >= 0.0f && chunkPosition.y < atmosphere ||
		chunkPosition.z >= 0.0f && chunkPosition.z < atmosphere ||
		chunkPosition.x < planetSize&& chunkPosition.x >= planetSize - atmosphere ||
		chunkPosition.y < planetSize&& chunkPosition.y >= planetSize - atmosphere ||
		chunkPosition.z < planetSize&& chunkPosition.z >= planetSize - atmosphere;
}

glm::vec3 Chunk::getUpDirection(const glm::vec3& localPosition) const
{
	glm::vec3 checkedPosition = { this->chunkPosition.x + localPosition.x, this->chunkPosition.y + localPosition.y, this->chunkPosition.z + localPosition.z };
	glm::vec3 planetCenter(glm::vec3(planetSize / 2.0f));

	

	static bool done = false;
	if (!done)
	{
		
		done = true;
	}

	if (checkedPosition.x <= checkedPosition.y && checkedPosition.x >= planetSize - 1.0f - checkedPosition.y &&
		checkedPosition.z <= checkedPosition.y && checkedPosition.z >= planetSize - 1.0f - checkedPosition.y)
	{

	}

	return {};
}

void Chunk::setBlock(size_t index, unsigned int blockType)
{
	blocks[index] = blockType; 
}

void Chunk::setBlock(const glm::uvec3& position, unsigned int blockType)
{
	blocks[utils::positionToIndex(position, { size, size, size })] = blockType;
}

glm::vec3 Chunk::getSideNormal(const glm::vec3& inPosition)
{
	const float bgSize = planetSize;

	std::vector<glm::vec3> positions{
		{ inPosition.y, inPosition.x, inPosition.z },
		{ inPosition.x, inPosition.y, inPosition.z },
		{ inPosition.x, inPosition.z, inPosition.y }
	};

	// Check positive pyramids
	for (size_t i = 0; i < 3; ++i)
	{
		if (positions[i].x <= positions[i].y && positions[i].x >= bgSize - 1 - positions[i].y &&
			positions[i].z <= positions[i].y && positions[i].z >= bgSize - 1 - positions[i].y)
		{
			glm::vec3 normal;
			normal[i] = 1.0f;
			return normal;
		}
	}

	// Check negative pyramids
	for (size_t i = 0; i < 3; ++i)
	{
		if (positions[i].x >= positions[i].y && positions[i].x <= bgSize - 1 - positions[i].y &&
			positions[i].z >= positions[i].y && positions[i].z <= bgSize - 1 - positions[i].y)
		{
			glm::vec3 normal;
			normal[i] = -1.0f;
			return normal;
		}
	}

	return {};
}