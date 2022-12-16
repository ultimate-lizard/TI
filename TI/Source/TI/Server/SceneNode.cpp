#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <SDL.h>

static unsigned long long ids = 0;

SceneNode::SceneNode() :
	parent(nullptr),
	transform(glm::mat4(1.0f)),
	localScale(glm::vec3(1.0f))
{
}

SceneNode::SceneNode(const SceneNode& other) :
	parent(other.parent),
	transform(other.transform),
	localPosition(other.localPosition),
	localOrientation(other.localOrientation),
	localScale(other.localScale)
{
}

void SceneNode::setParent(SceneNode* parent)
{
	this->parent = parent;

	if (parent)
	{
		if (auto foundIter = std::find(parent->children.begin(), parent->children.end(), this); foundIter == parent->children.end())
		{
			parent->children.push_back(this);
		}
	}

	updateTransform();
}

void SceneNode::removeParent()
{
	if (parent)
	{
		if (auto foundIter = std::find(parent->children.begin(), parent->children.end(), this); foundIter != parent->children.end())
		{
			parent->children.erase(foundIter);
		}
	}

	parent = nullptr;

	updateTransform();
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

glm::vec3 SceneNode::getLocalForwardVector()
{
	return normalize(glm::vec3(getLocalTransform()[2]));
}

glm::vec3 SceneNode::getLocalUpVector()
{
	return normalize(glm::vec3(getLocalTransform()[1]));
}

glm::vec3 SceneNode::getLocalRightVector()
{
	return -normalize(glm::vec3(getLocalTransform()[0]));
}

void SceneNode::updateTransform()
{
	glm::vec3 derivedPosition = localPosition;
	glm::quat derivedOrientation = localOrientation;

	if (parent)
	{
		glm::vec3 parentPosition;
		glm::quat parentOrientation;
		glm::vec3 parentScale;
		glm::vec3 scew;
		glm::vec4 perspective;

		glm::decompose(parent->transform, parentScale, parentOrientation, parentPosition, scew, perspective);

		parentOrientation = glm::conjugate(parentOrientation);

		//derivedPosition = parentScale * localPosition; // AKNOWLEDGE PARENT'S SCALE
		// derivedPosition = parentOrientation * localPosition;
		derivedPosition += parentPosition;

		derivedOrientation = parentOrientation * derivedOrientation;
	}

	glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), derivedPosition);
	newTransform *= glm::toMat4(derivedOrientation);
	newTransform = glm::scale(newTransform, localScale);
	this->transform = newTransform;

	for (SceneNode* child : children)
	{
		child->updateTransform();
	}
}

glm::mat4 SceneNode::getTransform() const
{
	return transform;
}

glm::mat4 SceneNode::getLocalTransform() const
{
	glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), localPosition);
	newTransform *= glm::toMat4(localOrientation);
	newTransform = glm::scale(newTransform, localScale);

	return newTransform;
}

void SceneNode::setPosition(const glm::vec3& position)
{
	this->localPosition = position;
	updateTransform();
}

void SceneNode::offset(const glm::vec3& offset)
{
	setPosition(getLocalPosition() + offset);
}

void SceneNode::setLocalOrientation(const glm::quat& orientation)
{
	this->localOrientation = orientation;
	updateTransform();
}

void SceneNode::setRotation(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	this->localOrientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	updateTransform();
}

void SceneNode::setRotation(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	localOrientation = rotationQuat;

	updateTransform();
}

void SceneNode::rotateInWorldSpace(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	localOrientation = localOrientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneNode::rotate(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	localOrientation = rotationQuat * localOrientation;

	updateTransform();
}

void SceneNode::setScale(const glm::vec3& scale)
{
	this->localScale = scale;
	updateTransform();
}

glm::vec3 SceneNode::getLocalPosition() const
{
	return localPosition;
}

glm::vec3 SceneNode::getDerivedPosition(bool inheritOrientation) const
{
	if (parent)
	{
		glm::vec3 derivedPosition = getLocalPosition();

		if (inheritOrientation)
		{
			derivedPosition = parent->getDerivedOrientation() * derivedPosition;
		}

		return derivedPosition + parent->getDerivedPosition();
	}

	return getLocalPosition();
}

glm::quat SceneNode::getLocalOrientation() const
{
	return localOrientation;
}

glm::quat SceneNode::getDerivedOrientation() const
{
	if (parent)
	{
		glm::quat parentDerivedOrientation = parent->getDerivedOrientation();
		glm::quat localOrientation = getLocalOrientation();
		glm::quat result = parentDerivedOrientation * localOrientation;
		return result;
	}
	else
	{
		return getLocalOrientation();
	}
}

glm::vec3 SceneNode::getScale() const
{
	return localScale;
}

glm::quat SceneNode::getOrientationInWorldSpace() const
{
	if (parent)
	{
		return parent->localOrientation * localOrientation;
	}

	return localOrientation;
}
