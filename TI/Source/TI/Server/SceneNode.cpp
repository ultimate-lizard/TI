#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>

SceneMultiNode::SceneNode::SceneNode() :
	parent(nullptr),
	transform(glm::mat4(1.0f)),
	scale(glm::vec3(1.0f))
{
}

SceneMultiNode::SceneNode::SceneNode(const SceneNode& other) :
	parent(nullptr),
	transform(other.transform),
	position(other.position),
	orientation(other.orientation),
	scale(other.scale)
{
}

void SceneMultiNode::SceneNode::setParent(SceneNode* parent)
{
	this->parent = parent;
	if (parent)
	{
		parent->addChild(this);
	}
}

void SceneMultiNode::SceneNode::addChild(SceneNode* child)
{
	children.push_back(child);
	child->parent = this;
}

bool SceneMultiNode::SceneNode::isChildOf(SceneNode* node)
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

glm::vec3 SceneMultiNode::SceneNode::getForwardVector()
{
	updateTransform();
	return normalize(glm::vec3(transform[2]));
}

glm::vec3 SceneMultiNode::SceneNode::getUpVector()
{
	updateTransform();
	return normalize(glm::vec3(transform[1]));
}

glm::vec3 SceneMultiNode::SceneNode::getRightVector()
{
	updateTransform();
	return -normalize(glm::vec3(transform[0]));
}

void SceneMultiNode::SceneNode::updateTransform()
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

glm::mat4 SceneMultiNode::SceneNode::getTransform() const
{
	return transform;
}

void SceneMultiNode::SceneNode::setPosition(const glm::vec3& position)
{
	this->position = position;
	updateTransform();
}

void SceneMultiNode::SceneNode::offset(const glm::vec3& offset)
{
	setPosition(getPosition() + offset);
}

void SceneMultiNode::SceneNode::setOrientation(const glm::quat& orientation)
{
	this->orientation = orientation;
	updateTransform();
}

void SceneMultiNode::SceneNode::setRotation(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	this->orientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	updateTransform();
}

//void SceneMultiNode::SceneNode::setRotationInWorldSpace(const glm::vec3& rotation)
//{
//	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
//	glm::quat rotationQuat = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));
//
//	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
//	{
//		coordinateSystems[i].orientation = coordinateSystems[i].orientation * glm::inverse(getOrientationInWorldSpace(CoordinateSystemScale::Planetary)) * rotationQuat * getOrientationInWorldSpace(CoordinateSystemScale::Planetary);
//	}
//
//	updateTransform();
//}

void SceneMultiNode::SceneNode::rotateInWorldSpace(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(angle, axis);
	orientation = orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneMultiNode::SceneNode::rotate(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(angle, axis);
	orientation = rotationQuat * orientation;

	updateTransform();
}

void SceneMultiNode::SceneNode::setScale(const glm::vec3& scale)
{
	this->scale = scale;
	updateTransform();
}

glm::vec3 SceneMultiNode::SceneNode::getPosition() const
{
	return position;
}

glm::quat SceneMultiNode::SceneNode::getOrientation() const
{
	return orientation;
}

//glm::vec3 SceneMultiNode::SceneNode::getRotation() const
//{
//	// Warning! Suppose the rotation is the same for all coordinate systems
//	glm::vec3 rot = glm::eulerAngles(coordinateSystems[0].orientation);
//	return rot;
//}

glm::vec3 SceneMultiNode::SceneNode::getScale() const
{
	return scale;
}

glm::quat SceneMultiNode::SceneNode::getOrientationInWorldSpace() const
{
	if (parent)
	{
		return parent->orientation * orientation;
	}

	return orientation;
}

SceneMultiNode::SceneMultiNode() :
	coordinateSystems({SceneNode(), SceneNode()})
{

}

SceneMultiNode::SceneMultiNode(const SceneMultiNode& other) :
	coordinateSystems(other.coordinateSystems)
{
}

glm::mat4 SceneMultiNode::getTransform(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getTransform();
}

void SceneMultiNode::setPosition(const glm::vec3& position, CoordinateSystem cs)
{
	coordinateSystems[cs].setPosition(position);
}

void SceneMultiNode::offset(const glm::vec3& position, CoordinateSystem cs)
{
	float factor = 1.0f;
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].offset(position / factor);
		factor *= 1000.0f;
	}

	//coordinateSystems[cs].offset(position);
}

void SceneMultiNode::setOrientation(const glm::quat& orientation, CoordinateSystem cs)
{
	coordinateSystems[cs].setOrientation(orientation);
}

void SceneMultiNode::setScale(const glm::vec3& scale, CoordinateSystem cs)
{
	coordinateSystems[cs].setScale(scale);
}

void SceneMultiNode::setRotation(const glm::vec3& rotation, CoordinateSystem cs)
{
	for (size_t i = 0; i < 2; ++i)
	{
		coordinateSystems[i].setRotation(rotation);
	}
}

void SceneMultiNode::rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < 2; ++i)
	{
		coordinateSystems[i].rotateInWorldSpace(angle, axis);
	}
}

void SceneMultiNode::rotateInWorldSpaceExclusive(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	coordinateSystems[cs].rotateInWorldSpace(angle, axis);
}

void SceneMultiNode::rotate(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < 2; ++i)
	{
		coordinateSystems[i].rotate(angle, axis);
	}
}

glm::vec3 SceneMultiNode::getPosition(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getPosition();
}

glm::quat SceneMultiNode::getOrientation(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getOrientation();
}

glm::vec3 SceneMultiNode::getScale(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getScale();
}

glm::vec3 SceneMultiNode::getForwardVector(CoordinateSystem cs)
{
	return coordinateSystems[cs].getForwardVector();
}

glm::vec3 SceneMultiNode::getUpVector(CoordinateSystem cs)
{
	return coordinateSystems[cs].getUpVector();
}

glm::vec3 SceneMultiNode::getRightVector(CoordinateSystem cs)
{
	return coordinateSystems[cs].getRightVector();
}

void SceneMultiNode::setParent(SceneMultiNode* parent, CoordinateSystem cs)
{
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].setParent(&parent->coordinateSystems[i]);
	}
}

void SceneMultiNode::addChild(SceneMultiNode* child)
{
	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].addChild(&child->coordinateSystems[i]);
	}
}

bool SceneMultiNode::isChildOf(SceneMultiNode* node)
{
	bool isChild = true;
	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		isChild &= coordinateSystems[i].isChildOf(&node->coordinateSystems[i]);
	}

	return isChild;
}
