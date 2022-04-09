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
	void setRotation(const glm::vec3& rotation);
	void rotateAbsolute(float angle, const glm::vec3& axis);
	void setScale(const glm::vec3& scale);

	const glm::vec3& getLocalPosition() const;
	const glm::vec3& getAbsolutePosition() const;
	const glm::vec3& getLocalRotation() const;
	const glm::vec3& getLocalScale() const;

	void setParent(SceneNode* parent);
	void addChild(SceneNode* child);
	bool isChildOf(SceneNode* node);

	glm::vec3 getForwardVector() const;
	glm::vec3 getUpVector() const;
	glm::vec3 getRightVector() const;

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;

	glm::mat4 transform;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::vector<glm::mat4> rotationTransform;
};
