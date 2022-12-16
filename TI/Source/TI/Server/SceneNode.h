#pragma once

#include <vector>
#include <optional>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

enum CoordinateSystem
{
	Planetary = 0,
	Interplanetary,
	Interstellar,
	Intergalactical,
	COUNT = 4
};

class SceneNode
{
public:
	SceneNode();

	SceneNode(const SceneNode& other);
	SceneNode& operator=(const SceneNode& other) = delete;
	SceneNode(SceneNode&&) noexcept = default;

	glm::mat4 getTransform() const;
	glm::mat4 getLocalTransform() const;

	void setPosition(const glm::vec3& position);
	void offset(const glm::vec3& position);
	void setLocalOrientation(const glm::quat& orientation);
	void setScale(const glm::vec3& scale);
	void setRotation(const glm::vec3& rotation);
	void setRotation(float angle, const glm::vec3& axis);

	void rotateInWorldSpace(float angle, const glm::vec3& axis);
	void rotate(float angle, const glm::vec3& axis);

	glm::vec3 getLocalPosition() const;
	glm::vec3 getDerivedPosition(bool inheritOrientation = true) const;

	glm::quat getLocalOrientation() const;
	glm::quat getDerivedOrientation() const;

	glm::vec3 getScale() const;

	void setParent(SceneNode* parent);
	void removeParent();

	bool isChildOf(SceneNode* node);

	glm::vec3 getForwardVector();
	glm::vec3 getUpVector();
	glm::vec3 getRightVector();

	glm::vec3 getLocalForwardVector();
	glm::vec3 getLocalUpVector();
	glm::vec3 getLocalRightVector();

protected:
	void updateTransform();
	glm::quat getOrientationInWorldSpace() const;

	SceneNode* parent;
	std::vector<SceneNode*> children;

	glm::mat4 transform;

	glm::vec3 localPosition;
	glm::quat localOrientation;
	glm::vec3 localScale;
};
