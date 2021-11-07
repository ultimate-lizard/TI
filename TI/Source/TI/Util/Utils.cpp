#include "Utils.h"

namespace utils
{
	glm::uvec3 indexToVector(size_t index, size_t volumeSize)
	{
		size_t z = index / (volumeSize * volumeSize);
		index -= (z * volumeSize * volumeSize);
		size_t y = index / volumeSize;
		size_t x = index % volumeSize;

		return { x, y, z };
	}

	size_t positionToIndex(const glm::uvec3& position, const glm::uvec3& volumeDimensions)
	{
		return (static_cast<size_t>(position.z) * volumeDimensions.x * volumeDimensions.y) + (static_cast<size_t>(position.y) * volumeDimensions.x) + position.x;
	}
}
