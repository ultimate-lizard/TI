#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>

SceneNode::SceneNode() :
	parent(nullptr),
	coordinateSystems({CoordinateSystem(), CoordinateSystem() })
{
}

SceneNode::SceneNode(const SceneNode& other) :
	parent(nullptr),
	coordinateSystems(other.coordinateSystems)
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

glm::vec3 SceneNode::getForwardVector(CoordinateSystemScale coordinateScale)
{
	const int index = static_cast<int>(coordinateScale);

	updateTransform();
	return normalize(glm::vec3(coordinateSystems[index].transform[2]));
}

glm::vec3 SceneNode::getUpVector(CoordinateSystemScale coordinateScale)
{
	const int index = static_cast<int>(coordinateScale);

	updateTransform();
	return normalize(glm::vec3(coordinateSystems[index].transform[1]));
}

glm::vec3 SceneNode::getRightVector(CoordinateSystemScale coordinateScale)
{
	const int index = static_cast<int>(coordinateScale);

	updateTransform();
	return -normalize(glm::vec3(coordinateSystems[index].transform[0]));
}

void SceneNode::updateTransform()
{
	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), coordinateSystems[i].position);
		transform *= glm::toMat4(coordinateSystems[i].orientation);
		transform = glm::scale(transform, coordinateSystems[i].scale);
		coordinateSystems[i].transform = parent ? parent->coordinateSystems[i].transform * transform : transform;
	}

	for (SceneNode* child : children)
	{
		child->updateTransform();
	}
}

glm::mat4 SceneNode::getTransform(CoordinateSystemScale coordinateScale) const
{
	const int index = static_cast<int>(coordinateScale);

	return coordinateSystems[index].transform;
}

void SceneNode::setPosition(const glm::vec3& position, CoordinateSystemScale coordinateScale)
{
	const float INTERPLANETARY_SCALE = 1000.0f;
	const float INTERSTELLAR_SCALE = 1'000'000.0f;

	switch (coordinateScale)
	{
	case CoordinateSystemScale::Planetary:
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Planetary)].position = position;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position = (position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position) / INTERPLANETARY_SCALE;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interstellar)].position = (coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Interstellar)].position) / INTERSTELLAR_SCALE;
		break;
	case CoordinateSystemScale::Interplanetary:
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Planetary)].position = (coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Planetary)].position) * INTERPLANETARY_SCALE;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position = position;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interstellar)].position = (position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Interstellar)].position) / INTERPLANETARY_SCALE;
		break;
	case CoordinateSystemScale::Interstellar:
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Planetary)].position = (coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Planetary)].position) * INTERSTELLAR_SCALE;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position = (position - coordinateSystems[static_cast<int>(CoordinateSystemScale::Interplanetary)].position) * INTERPLANETARY_SCALE;
		coordinateSystems[static_cast<int>(CoordinateSystemScale::Interstellar)].position = position;
		break;
	default:
		break;
	}

	updateTransform();
}

//void SceneNode::setPositionInterplanetary(const glm::vec3& position)
//{
//	coordinateSystems[1].position = position;
//	updateTransform();
//}

void SceneNode::setOrientation(const glm::quat& orientation)
{
	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
	{
		coordinateSystems[i].orientation = orientation;
	}
	
	updateTransform();
}

void SceneNode::setRotation(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };

	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
	{
		coordinateSystems[i].orientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));
	}
	
	// coordinateSystems[1].orientation = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	updateTransform();
}

void SceneNode::setRotationInWorldSpace(const glm::vec3& rotation)
{
	const glm::vec3 rotationRadians{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };
	glm::quat rotationQuat = glm::toQuat(glm::eulerAngleYXZ(rotationRadians.y, rotationRadians.x, rotationRadians.z));

	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
	{
		coordinateSystems[i].orientation = coordinateSystems[i].orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();
	}

	// coordinateSystems[1].orientation = coordinateSystems[0].orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneNode::rotateInWorldSpace(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(angle, axis);

	for (int i = 0; i < static_cast<int>(CoordinateSystemScale::COUNT); ++i)
	{
		coordinateSystems[i].orientation = coordinateSystems[i].orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();
	}

	// coordinateSystems[1].orientation = coordinateSystems[1].orientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneNode::setScale(const glm::vec3& scale, CoordinateSystemScale coordinateScale)
{
	coordinateSystems[static_cast<int>(coordinateScale)].scale = scale;
	updateTransform();
}

//void SceneNode::setScaleInterplanetary(const glm::vec3& scale)
//{
//	coordinateSystems[1].scale = scale;
//	updateTransform();
//}

glm::vec3 SceneNode::getPosition(CoordinateSystemScale coordinateScale) const
{
	return coordinateSystems[static_cast<int>(coordinateScale)].position;
}

glm::quat SceneNode::getOrientation() const
{
	// Warning! Suppose the orientation is the same for all coordinate systems
	return coordinateSystems[0].orientation;
}

glm::vec3 SceneNode::getRotation() const
{
	// Warning! Suppose the rotation is the same for all coordinate systems
	glm::vec3 rot = glm::eulerAngles(coordinateSystems[0].orientation);
	return rot;
}

glm::vec3 SceneNode::getScale(CoordinateSystemScale coordinateScale) const
{
	return coordinateSystems[static_cast<int>(coordinateScale)].scale;
}

glm::quat SceneNode::getOrientationInWorldSpace() const
{
	// Warning! Suppose the orientation is the same for all coordinate systems

	if (parent)
	{
		return parent->coordinateSystems[0].orientation * coordinateSystems[0].orientation;
	}

	return coordinateSystems[0].orientation;
}
