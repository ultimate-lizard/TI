#pragma once

#include <vector>

#include <glm/glm.hpp>

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
	void setScale(const glm::vec3& scale);

	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;
	const glm::vec3& getScale() const;

	void setParent(SceneNode* parent);
	void addChild(SceneNode* child);
	bool isChildOf(SceneNode* node);

protected:
	SceneNode* parent;
	std::vector<SceneNode*> children;

	glm::mat4 transform;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
