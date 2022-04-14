#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>

SceneNode::SceneNode() :
	parent(nullptr),
	transform(1.0f),
	position(0.0f),
	scale(1.0f)
{
}

SceneNode::SceneNode(const SceneNode& other) :
	parent(nullptr),
	transform(other.transform),
	position(other.position),
	orientation(other.orientation),
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

glm::vec3 SceneNode::getForwardVector()
{
	updateTransform();
	return normalize(glm::vec3(transform[2]));
}

glm::vec3 SceneNode::getUpVector()
{
	updateTransform();
	return normalize(glm::vec3(transform[1]));
}

glm::vec3 SceneNode::getRightVector()
{
	updateTransform();
	return -normalize(glm::vec3(transform[0]));
}

void SceneNode::updateTransform()
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
	transform *= glm::toMat4(orientation);
	transform = glm::scale(transform, scale);

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

void SceneNode::setOrientation(const glm::quat& orientation)
{
	this->orientation = orientation;
	updateTransform();
}

void SceneNode::setRotation(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	orientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	updateTransform();
}

void SceneNode::setRotationInWorldSpace(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	glm::quat rotationQuat = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	orientation = orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneNode::rotateInWorldSpace(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(angle, axis);
	orientation = orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneNode::setScale(const glm::vec3& scale)
{
	this->scale = scale;
	updateTransform();
}

glm::vec3 SceneNode::getPosition() const
{
	return position;
}

glm::quat SceneNode::getOrientation() const
{
	return orientation;
}

glm::vec3 SceneNode::getRotation() const
{
	glm::vec3 rot = glm::eulerAngles(orientation);
	return rot;
}

glm::vec3 SceneNode::getScale() const
{
	return scale;
}

glm::quat SceneNode::getOrientationInWorldSpace() const
{
	if (parent)
	{
		return parent->orientation * orientation;
	}

	return orientation;
}
