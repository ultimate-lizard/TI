#include "SceneNode.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

SceneNode::SceneNode() :
	parent(nullptr),
	transform(1.0f),
	position(0.0f),
	rotation(0.0f),
	scale(1.0f)
{
}

SceneNode::SceneNode(const SceneNode& other) :
	parent(nullptr),
	transform(other.transform),
	position(other.position),
	rotation(other.rotation),
	scale(other.scale)
{
}

void SceneNode::setParent(SceneNode* parent)
{
	this->parent = parent;
	if (parent)
	{
		parent->addChild(this);
	}
}

void SceneNode::addChild(SceneNode* child)
{
	children.push_back(child);
	child->parent = this;
}

bool SceneNode::isChildOf(SceneNode* node)
{
	for (SceneNode* n : node->children)
	{
		if (n == this)
		{
			return true;
		}
	}

	return false;
}

void SceneNode::updateTransform()
{
	glm::mat4 transform(1.0f);

	transform = glm::translate(transform, position);
	transform = glm::scale(transform, scale);
	transform *= glm::yawPitchRoll(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));

	this->transform = parent ? parent->transform * transform : transform;

	for (SceneNode* child : children)
	{
		child->updateTransform();
	}
}

glm::mat4 SceneNode::getTransform() const
{
	return transform;
}

void SceneNode::setPosition(const glm::vec3& position)
{
	this->position = position;
	updateTransform();
}

void SceneNode::setRotation(const glm::vec3& rotation)
{
	this->rotation = rotation;
	updateTransform();
}

void SceneNode::setScale(const glm::vec3& scale)
{
	this->scale = scale;
	updateTransform();
}

const glm::vec3& SceneNode::getLocalPosition() const
{
	return position;
}

const glm::vec3& SceneNode::getAbsolutePosition() const
{
	glm::vec4 absolutePosition = getTransform() * glm::vec4(position, 1.0f);
	return glm::vec3(absolutePosition);
}

const glm::vec3& SceneNode::getLocalRotation() const
{
	return rotation;
}

const glm::vec3& SceneNode::getLocalScale() const
{
	return scale;
}
