#include "Math.h"

glm::vec3 clampVectorMagnitude(const glm::vec3& vector, float max)
{
	const float vSqr = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
	if (vSqr > max * max)
	{
		const float scale = max * glm::inversesqrt(vSqr);
		return glm::vec3(vector.x * scale, vector.y * scale, vector.z * scale);
	}

	return vector;
}
