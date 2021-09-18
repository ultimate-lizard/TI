#include "SceneNode.h"

#include <glm/gtc/matrix_transform.hpp>

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
	transform = glm::rotate(transform, -glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(0.0f, 0.0f, 1.0f));

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

const glm::vec3& SceneNode::getPosition() const
{
	return position;
}

const glm::vec3& SceneNode::getRotation() const
{
	return rotation;
}

const glm::vec3& SceneNode::getScale() const
{
	return scale;
}
