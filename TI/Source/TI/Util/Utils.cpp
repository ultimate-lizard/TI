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

	glm::vec3 alightVectorToAxis(const glm::vec3& inVector)
	{
		glm::vec3 axisAlignedVector = inVector;

		float max = 0.0f;
		size_t maxAxis = 0;
		bool positive = true;

		for (size_t axis = 0; axis < 3; ++axis)
		{
			if (max < glm::abs(axisAlignedVector[axis]))
			{
				max = glm::abs(axisAlignedVector[axis]);
				maxAxis = axis;
				positive = axisAlignedVector[axis] > 0.0f;
			}
		}

		axisAlignedVector = {};
		axisAlignedVector[maxAxis] = positive ? 1.0f : -1.0f;
		return axisAlignedVector;
	}
}
