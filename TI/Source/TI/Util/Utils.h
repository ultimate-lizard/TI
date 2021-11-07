#pragma once

#include <glm/glm.hpp>

namespace utils
{
	glm::uvec3 indexToVector(size_t index, size_t volumeSize);
	size_t positionToIndex(const glm::uvec3& position, const glm::uvec3& volumeDimensions);
}
