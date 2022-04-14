#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class SceneNode
{
public:
	SceneNode();

	SceneNode(const SceneNode&);
	SceneNode(SceneNode&&) = delete;

	void updateTransform();

	glm::mat4 getTransform() const;

	void setPosition(const glm::vec3& position);
	void setOrientation(const glm::quat& orientation);
	void setScale(const glm::vec3& scale);
	// Sets rotation in euler angles
	void setRotation(const glm::vec3& rotation);
	void setRotationInWorldSpace(const glm::vec3& rotation);

	void rotateInWorldSpace(float angle, const glm::vec3& axis);

	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;
	glm::vec3 getScale() const;
	// Gets rotation in euler angles
	glm::vec3 getRotation() const;

	void setParent(SceneNode* parent);
	void addChild(SceneNode* child);
	bool isChildOf(SceneNode* node);

	glm::vec3 getForwardVector();
	glm::vec3 getUpVector();
	glm::vec3 getRightVector();

protected:
	glm::quat getOrientationInWorldSpace() const;

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;

	glm::mat4 transform;

	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;
};
