#pragma once

#include <glm/glm.hpp>

namespace utils
{
	glm::uvec3 indexToPosition(size_t index, const glm::uvec3& volumeDimensions);
	size_t positionToIndex(const glm::uvec3& position, const glm::uvec3& volumeDimensions);
}
