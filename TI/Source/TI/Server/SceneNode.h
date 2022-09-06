#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

enum class CoordinateSystemScale
{
	Planetary = 0,
	Interplanetary,
	Interstellar,
	COUNT = 3
};

struct CoordinateSystem
{
	glm::mat4 transform = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3(0.0f);
	glm::quat orientation;
	glm::vec3 scale = glm::vec3(1.0f);
};

class SceneNode
{
public:
	SceneNode();

	SceneNode(const SceneNode&);
	SceneNode(SceneNode&&) = delete;

	glm::mat4 getTransform(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary) const;

	void setPosition(const glm::vec3& position, CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary);
	// void setPositionInterplanetary(const glm::vec3& position);
	// void setOrientation(const glm::quat& orientation, CoordinateSystemScale coordinateScale);
	void setOrientation(const glm::quat& orientation);
	void setScale(const glm::vec3& scale, CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary);
	// void setScaleInterplanetary(const glm::vec3& scale);
	// Sets rotation in euler angles
	// void setRotation(const glm::vec3& rotation, CoordinateSystemScale coordinateScale);
	void setRotation(const glm::vec3& rotation);
	// void setRotationInWorldSpace(const glm::vec3& rotation, CoordinateSystemScale coordinateScale);
	void setRotationInWorldSpace(const glm::vec3& rotation);

	void rotateInWorldSpace(float angle, const glm::vec3& axis);

	glm::vec3 getPosition(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary) const;
	// glm::quat getOrientation(CoordinateSystemScale coordinateScale) const;
	glm::quat getOrientation() const;
	glm::vec3 getScale(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary) const;
	// Gets rotation in euler angles
	// glm::vec3 getRotation(CoordinateSystemScale coordinateScale) const;
	glm::vec3 getRotation() const;

	void setParent(SceneNode* parent);
	void addChild(SceneNode* child);
	bool isChildOf(SceneNode* node);

	glm::vec3 getForwardVector(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary);
	glm::vec3 getUpVector(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary);
	glm::vec3 getRightVector(CoordinateSystemScale coordinateScale = CoordinateSystemScale::Planetary);

protected:
	void updateTransform();
	glm::quat getOrientationInWorldSpace() const;

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;

	std::array<CoordinateSystem, static_cast<int>(CoordinateSystemScale::COUNT)> coordinateSystems;
};
