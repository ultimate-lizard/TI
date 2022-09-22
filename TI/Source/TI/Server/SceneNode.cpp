#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>

SceneMultiNode::SceneNode::SceneNode() :
	parent(nullptr),
	transform(glm::mat4(1.0f)),
	localScale(glm::vec3(1.0f))
{
}

SceneMultiNode::SceneNode::SceneNode(const SceneNode& other) :
	parent(nullptr),
	transform(other.transform),
	localPosition(other.localPosition),
	localOrientation(other.localOrientation),
	localScale(other.localScale)
{
}

void SceneMultiNode::SceneNode::setParent(SceneNode* parent)
{
	this->parent = parent;
	if (parent)
	{
		parent->addChild(this);
	}

	updateTransform();
}

void SceneMultiNode::SceneNode::removeParent()
{
	if (parent)
	{
		parent->removeChild(this);
		parent = nullptr;
	}

	updateTransform();
}

void SceneMultiNode::SceneNode::addChild(SceneNode* child)
{
	children.push_back(child);
	child->parent = this;
	updateTransform();
}

void SceneMultiNode::SceneNode::removeChild(SceneNode* child)
{
	if (auto foundIter = std::find(children.begin(), children.end(), child); foundIter != children.end())
	{
		children.erase(foundIter);
	}

	updateTransform();
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

		// derivedPosition = parentOrientation * (parentScale * localPosition); // AKNOWLEDGE PARENT'S SCALE
		derivedPosition = parentOrientation * localPosition;
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

glm::mat4 SceneMultiNode::SceneNode::getTransform() const
{
	return transform;
}

void SceneMultiNode::SceneNode::setPosition(const glm::vec3& position)
{
	this->localPosition = position;
	updateTransform();
}

void SceneMultiNode::SceneNode::offset(const glm::vec3& offset)
{
	setPosition(getLocalPosition() + offset);
}

void SceneMultiNode::SceneNode::setLocalOrientation(const glm::quat& orientation)
{
	this->localOrientation = orientation;
	updateTransform();
}

void SceneMultiNode::SceneNode::setRotation(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	this->localOrientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

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
	localOrientation = localOrientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneMultiNode::SceneNode::rotate(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(angle, axis);
	localOrientation = rotationQuat * localOrientation;

	updateTransform();
}

void SceneMultiNode::SceneNode::setScale(const glm::vec3& scale)
{
	this->localScale = scale;
	updateTransform();
}

glm::vec3 SceneMultiNode::SceneNode::getLocalPosition() const
{
	return localPosition;
}

glm::vec3 SceneMultiNode::SceneNode::getDerivedPosition() const
{
	return parent ? parent->getDerivedOrientation() * getLocalPosition() + parent->getDerivedPosition() : getLocalPosition();
}

glm::quat SceneMultiNode::SceneNode::getLocalOrientation() const
{
	return localOrientation;
}

glm::quat SceneMultiNode::SceneNode::getDerivedOrientation() const
{
	return parent ? parent->getDerivedOrientation() * getLocalOrientation() : getLocalOrientation();
}

//glm::vec3 SceneMultiNode::SceneNode::getRotation() const
//{
//	// Warning! Suppose the rotation is the same for all coordinate systems
//	glm::vec3 rot = glm::eulerAngles(coordinateSystems[0].orientation);
//	return rot;
//}

glm::vec3 SceneMultiNode::SceneNode::getScale() const
{
	return localScale;
}

glm::quat SceneMultiNode::SceneNode::getOrientationInWorldSpace() const
{
	if (parent)
	{
		return parent->localOrientation * localOrientation;
	}

	return localOrientation;
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

void SceneMultiNode::setLocalPosition(const glm::vec3& position, CoordinateSystem cs)
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

void SceneMultiNode::setLocalOrientation(const glm::quat& orientation, CoordinateSystem cs, bool propagateToChildren)
{
	if (propagateToChildren)
	{
		for (size_t i = cs; i < coordinateSystems.size(); ++i)
		{
			coordinateSystems[i].setLocalOrientation(orientation);
		}
	}
	else
	{
		coordinateSystems[cs].setLocalOrientation(orientation);
	}
}

void SceneMultiNode::setLocalScale(const glm::vec3& scale, CoordinateSystem cs)
{
	coordinateSystems[cs].setScale(scale);
}

void SceneMultiNode::setRotation(const glm::vec3& rotation, CoordinateSystem cs)
{
	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].setRotation(rotation);
	}
}

void SceneMultiNode::rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
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
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].rotate(angle, axis);
	}
}

glm::vec3 SceneMultiNode::getLocalPosition(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getLocalPosition();
}

glm::vec3 SceneMultiNode::getDerivedPosition(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getDerivedPosition();
}

glm::quat SceneMultiNode::getLocalOrientation(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getLocalOrientation();
}

glm::quat SceneMultiNode::getDerivedOrientation(CoordinateSystem cs) const
{
	return coordinateSystems[cs].getDerivedOrientation();
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

void SceneMultiNode::removeParent(CoordinateSystem cs)
{
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].removeParent();
	}
}

void SceneMultiNode::addChild(SceneMultiNode* child, CoordinateSystem cs)
{
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].addChild(&child->coordinateSystems[i]);
	}
}

void SceneMultiNode::removeChild(SceneMultiNode* child, CoordinateSystem cs)
{
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		coordinateSystems[i].removeChild(&child->coordinateSystems[i]);
	}
}

bool SceneMultiNode::isChildOf(SceneMultiNode* node, CoordinateSystem cs)
{
	bool isChild = true;
	for (size_t i = cs; i < coordinateSystems.size(); ++i)
	{
		isChild &= coordinateSystems[i].isChildOf(&node->coordinateSystems[i]);
	}

	return isChild;
}
