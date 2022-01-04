#include "Utils.h"

namespace utils
{
	glm::uvec3 indexToPosition(size_t index, const glm::uvec3& volumeDimensions)
	{
		size_t z = index / (static_cast<size_t>(volumeDimensions.x) * volumeDimensions.y);
		index -= (z * volumeDimensions.x * volumeDimensions.y);
		size_t y = index / volumeDimensions.x;
		size_t x = index % volumeDimensions.x;

		return { x, y, z };
	}

	size_t positionToIndex(const glm::uvec3& position, const glm::uvec3& volumeDimensions)
	{
		return (static_cast<size_t>(position.z) * volumeDimensions.x * volumeDimensions.y) + (static_cast<size_t>(position.y) * volumeDimensions.x) + position.x;
	}
}
