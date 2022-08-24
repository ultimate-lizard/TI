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
	// if not 2 dimensional
	orientations = { Orientations::TOP, Orientations::BOTTOM, Orientations::RIGHT, Orientations::LEFT, Orientations::FRONT, Orientations::BACK };

	unsigned long long longSize = size;
	blocks.resize(longSize * longSize * longSize, 0);

	static noise::module::RidgedMulti noiseModule;

	for (unsigned short i = 0; i < 4096; ++i)
	{
		glm::vec3 localPosition = utils::indexToPosition(i, { 16, 16, 16 });
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

		//continue;
		if (isInCrust() || isInTroposphere())
		{
			// const float MAX_AMPLITUDE = 8; // How far under or above 0 the terrain will differ
			// const float TERRAIN_LOCATION_HEIGHT = 2 * chunkSize; // From center
			const float MAX_AMPLITUDE = 8; // How far under or above 0 the terrain will differ
			const float TERRAIN_LOCATION_HEIGHT = chunkSize * 2; // From center
			
			for (const OrientationInfo& orientationInfo : orientations)
			{
				const float generatedTerrainHeight = MAX_AMPLITUDE * noiseModule.GetValue(perlinPos.x, perlinPos.y, perlinPos.z);

				unsigned int block = 0;
				if (orientationInfo.positive)
				{
					block = localPosition[orientationInfo.heightAxis] + chunkPosition[orientationInfo.heightAxis] - planetSize + TERRAIN_LOCATION_HEIGHT > generatedTerrainHeight ? 0 : 1;
				}
				else
				{
					block = localPosition[orientationInfo.heightAxis] + chunkPosition[orientationInfo.heightAxis] < TERRAIN_LOCATION_HEIGHT - generatedTerrainHeight ? 0 : 1;
				}
				
				if (isInCone(localPosition, orientationInfo))
				{
					blocks[i] = block;
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
	return chunkPosition;
}

size_t Chunk::getSize() const
{
	return size;
}

bool Chunk::isInCone(const glm::vec3& localPosition, const OrientationInfo& orientationInfo) const
{
	glm::vec3 checkedPosition = { this->chunkPosition.x + localPosition.x, this->chunkPosition.y + localPosition.y, this->chunkPosition.z + localPosition.z};

	return orientationInfo.positive ?
		checkedPosition[orientationInfo.sideAxis] <= checkedPosition[orientationInfo.heightAxis] && checkedPosition[orientationInfo.sideAxis] >= planetSize - 1.0f - checkedPosition[orientationInfo.heightAxis] &&
		checkedPosition[orientationInfo.frontAxis] <= checkedPosition[orientationInfo.heightAxis] && checkedPosition[orientationInfo.frontAxis] >= planetSize - 1.0f - checkedPosition[orientationInfo.heightAxis]
		:
		checkedPosition[orientationInfo.sideAxis] >= checkedPosition[orientationInfo.heightAxis] && checkedPosition[orientationInfo.sideAxis] <= planetSize - 1.0f - checkedPosition[orientationInfo.heightAxis] &&
		checkedPosition[orientationInfo.frontAxis] >= checkedPosition[orientationInfo.heightAxis] && checkedPosition[orientationInfo.frontAxis] <= planetSize - 1.0f - checkedPosition[orientationInfo.heightAxis];
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

std::optional<OrientationInfo> Chunk::getOrientationInfo(const glm::vec3& localPosition) const
{
	for (const OrientationInfo& orientationInfo : orientations)
	{
		if (isInCone(localPosition, orientationInfo))
		{
			return orientationInfo;
		}
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
