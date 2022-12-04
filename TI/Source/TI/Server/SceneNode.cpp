#include "SceneNode.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <SDL.h>

static unsigned long long ids = 0;

SceneMultiNode::SceneNode::SceneNode() :
	parent(nullptr),
	transform(glm::mat4(1.0f)),
	localScale(glm::vec3(1.0f)),
	type(CoordinateSystem::Planetary)
{
}

SceneMultiNode::SceneNode::SceneNode(CoordinateSystem cs) :
	parent(nullptr),
	transform(glm::mat4(1.0f)),
	localScale(glm::vec3(1.0f)),
	type(cs)
{
}

SceneMultiNode::SceneNode::SceneNode(const SceneNode& other) :
	parent(other.parent),
	transform(other.transform),
	localPosition(other.localPosition),
	localOrientation(other.localOrientation),
	localScale(other.localScale),
	type(other.type)
{
}

void SceneMultiNode::SceneNode::setParent(SceneNode* parent)
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

void SceneMultiNode::SceneNode::removeParent()
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

glm::vec3 SceneMultiNode::SceneNode::getLocalForwardVector()
{
	return normalize(glm::vec3(getLocalTransform()[2]));
}

glm::vec3 SceneMultiNode::SceneNode::getLocalUpVector()
{
	return normalize(glm::vec3(getLocalTransform()[1]));
}

glm::vec3 SceneMultiNode::SceneNode::getLocalRightVector()
{
	return -normalize(glm::vec3(getLocalTransform()[0]));
}

CoordinateSystem SceneMultiNode::SceneNode::getType() const
{
	return type;
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

glm::mat4 SceneMultiNode::SceneNode::getTransform() const
{
	return transform;
}

glm::mat4 SceneMultiNode::SceneNode::getLocalTransform() const
{
	glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), localPosition);
	newTransform *= glm::toMat4(localOrientation);
	newTransform = glm::scale(newTransform, localScale);

	return newTransform;
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

void SceneMultiNode::SceneNode::setRotation(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	localOrientation = rotationQuat;

	updateTransform();
}

void SceneMultiNode::SceneNode::rotateInWorldSpace(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
	localOrientation = localOrientation * glm::inverse(getOrientationInWorldSpace()) * rotationQuat * getOrientationInWorldSpace();

	updateTransform();
}

void SceneMultiNode::SceneNode::rotate(float angle, const glm::vec3& axis)
{
	glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
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

glm::vec3 SceneMultiNode::SceneNode::getDerivedPosition(bool inheritOrientation) const
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

glm::quat SceneMultiNode::SceneNode::getLocalOrientation() const
{
	return localOrientation;
}

glm::quat SceneMultiNode::SceneNode::getDerivedOrientation() const
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
	parent(nullptr)
{
	for (size_t i = CoordinateSystem::Planetary; i < CoordinateSystem::COUNT; ++i)
	{
		coordinateSystems.emplace_back(std::make_unique<SceneMultiNode::SceneNode>(static_cast<CoordinateSystem>(i)));
	}
}

SceneMultiNode::SceneMultiNode(CoordinateSystem minimalCoordinateSystem) :
	parent(nullptr)
{
	for (size_t i = minimalCoordinateSystem; i < CoordinateSystem::COUNT; ++i)
	{
		coordinateSystems.emplace_back(std::make_unique<SceneMultiNode::SceneNode>(static_cast<CoordinateSystem>(i)));
	}
}

SceneMultiNode::SceneMultiNode(const SceneMultiNode& other)
{
	for (size_t i = 0; i < other.coordinateSystems.size(); ++i)
	{
		coordinateSystems.emplace_back(std::unique_ptr<SceneMultiNode::SceneNode>(new SceneMultiNode::SceneNode(*other.coordinateSystems[i].get())));
	}

	parent = other.parent;
}

glm::mat4 SceneMultiNode::getTransform(CoordinateSystem cs) const
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getTransform();
		}
	}
	
	throw std::exception();
}

void SceneMultiNode::setLocalPosition(const glm::vec3& position, CoordinateSystem cs, bool propagateToUpperCoordinateSystems)
{
	if (propagateToUpperCoordinateSystems)
	{
		float factor = 1.0f;

		for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
		{
			if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
			{
				coordinateSystem.value()->setPosition(position / factor);
			}

			factor *= 1000.0f;
		}
	}
	else
	{
		if (auto coordinateSystem = getCoordinateSystem(cs); coordinateSystem.has_value())
		{
			coordinateSystem.value()->setPosition(position);
		}
		else
		{
			throw std::runtime_error("Tried to set exclusive position for unexisting coordinate system");
		}
	}
}

//void SceneMultiNode::setLocalPositionExclusive(const glm::vec3& position, CoordinateSystem cs)
//{
//	if (auto coordinateSystem = getCoordinateSystem(cs); coordinateSystem.has_value())
//	{
//		coordinateSystem.value()->setPosition(position);
//	}
//}

void SceneMultiNode::offset(const glm::vec3& position, CoordinateSystem cs)
{
	float factor = 1.0f;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->offset(position / factor);
		}
		
		factor *= 1000.0f;
	}
}

void SceneMultiNode::setLocalOrientation(const glm::quat& orientation, CoordinateSystem cs, bool propagateToUpperCoordinateSystems)
{
	if (propagateToUpperCoordinateSystems)
	{
		for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
		{
			if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
			{
				coordinateSystem.value()->setLocalOrientation(orientation);
			}
		}
	}
	else
	{
		if (auto coordinateSystem = getCoordinateSystem(cs); coordinateSystem.has_value())
		{
			coordinateSystem.value()->setLocalOrientation(orientation);
		}
	}
}

void SceneMultiNode::setLocalScale(const glm::vec3& scale, CoordinateSystem cs)
{
	float factor = 1.0f;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->setScale(scale / factor);
		}

		factor *= 1000.0f;
	}
}

void SceneMultiNode::setRotation(const glm::vec3& rotation, CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->setRotation(rotation);
		}
	}
}

void SceneMultiNode::setRotation(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->setRotation(angle, axis);
		}
	}
}

void SceneMultiNode::rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->rotateInWorldSpace(angle, axis);
		}
	}
}

void SceneMultiNode::rotateInWorldSpaceExclusive(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	if (auto coordinateSystem = getCoordinateSystem(cs); coordinateSystem.has_value())
	{
		coordinateSystem.value()->rotateInWorldSpace(angle, axis);
	}
}

void SceneMultiNode::rotate(float angle, const glm::vec3& axis, CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->rotate(angle, axis);
		}
	}
}

glm::vec3 SceneMultiNode::getLocalPosition(CoordinateSystem cs) const
{
	float factor = 1.0f;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getLocalPosition() * factor;
		}
		else
		{
			return {};
		}

		factor *= 1000.0f;
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getDerivedPosition(CoordinateSystem cs, bool inheritOrientation, bool returnNextCsWhenPossible) const
{
	float factor = 1.0f;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getDerivedPosition(inheritOrientation) * factor;
		}
		else
		{
			if (!returnNextCsWhenPossible)
			{
				return {};
			}
		}

		factor *= 1000.0f;
	}

	throw std::exception();
}

glm::quat SceneMultiNode::getLocalOrientation(CoordinateSystem cs) const
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getLocalOrientation();
		}
	}

	throw std::exception();
}

glm::quat SceneMultiNode::getDerivedOrientation(CoordinateSystem cs) const
{
	if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(cs)); coordinateSystem.has_value())
	{
		return coordinateSystem.value()->getDerivedOrientation();
	}
	else
	{
		return {};
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getScale(CoordinateSystem cs) const
{
	float factor = 1.0f;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getScale() / factor;
		}

		factor *= 1000.0f;
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getForwardVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getForwardVector();
		}
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getUpVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getUpVector();
		}
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getRightVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getRightVector();
		}
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getLocalForwardVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getLocalForwardVector();
		}
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getLocalUpVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getLocalUpVector();
		}
	}

	throw std::exception();
}

glm::vec3 SceneMultiNode::getLocalRightVector(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			return coordinateSystem.value()->getLocalRightVector();
		}
	}

	throw std::exception();
}

void SceneMultiNode::setParent(SceneMultiNode* parent, CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			if (const auto parentCoordinateSystem = parent->getCoordinateSystem(static_cast<CoordinateSystem>(i)); parentCoordinateSystem.has_value())
			{
				coordinateSystem.value()->setParent(parentCoordinateSystem.value());
			}
		}
	}

	if (parent)
	{
		parent->children.push_back(this);
	}

	this->parent = parent;
}

void SceneMultiNode::removeParent(CoordinateSystem cs)
{
	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			coordinateSystem.value()->removeParent();
		}
	}

	if (parent)
	{
		if (auto foundIter = std::find(parent->children.begin(), parent->children.end(), this); foundIter != parent->children.end())
		{
			parent->children.erase(foundIter);
		}
	}

	parent = nullptr;
}

bool SceneMultiNode::isChildOf(SceneMultiNode* node, CoordinateSystem cs)
{
	bool isChild = true;

	for (size_t i = cs; i < CoordinateSystem::COUNT; ++i)
	{
		if (const auto coordinateSystem = getCoordinateSystem(static_cast<CoordinateSystem>(i)); coordinateSystem.has_value())
		{
			if (const auto parentCoordinateSystem = node->getCoordinateSystem(static_cast<CoordinateSystem>(i)); parentCoordinateSystem.has_value())
			{
				isChild &= coordinateSystem.value()->isChildOf(parentCoordinateSystem.value());
			}
		}
	}

	return isChild;
}

void SceneMultiNode::leaveNode(CoordinateSystem cs)
{
	for (auto& child : children)
	{
		child->leaveNode(cs);
	}

	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		if (coordinateSystems[i]->getType() == cs)
		{
			coordinateSystems[i]->removeParent();

			inactiveCoordinateSystems.push_back(std::move(coordinateSystems[i]));

			coordinateSystems.erase(coordinateSystems.begin() + i);
		}
	}
}

void SceneMultiNode::enterNode(CoordinateSystem cs)
{
	auto newCoordinateSystem = std::make_unique<SceneMultiNode::SceneNode>(cs);

	/*If we don't restore parent and children, the entered node won't act properly. It will just exist by itself, not connected to its parent and children*/

	// Restore parent
	if (parent)
	{
		if (auto parentCoordinateSystem = parent->getCoordinateSystem(cs); parentCoordinateSystem.has_value())
		{
			newCoordinateSystem->setParent(parentCoordinateSystem.value());
		}
	}

	auto previouslyDeletedCsIter = std::find_if(inactiveCoordinateSystems.begin(), inactiveCoordinateSystems.end(), [&](const std::unique_ptr<SceneMultiNode::SceneNode>& ics) {
		return ics->getType() == cs;
	});

	if (previouslyDeletedCsIter != inactiveCoordinateSystems.end())
	{
		newCoordinateSystem->setPosition(previouslyDeletedCsIter->get()->getLocalPosition());
		newCoordinateSystem->setLocalOrientation(previouslyDeletedCsIter->get()->getLocalOrientation());
		newCoordinateSystem->setScale(previouslyDeletedCsIter->get()->getScale());

		inactiveCoordinateSystems.erase(previouslyDeletedCsIter);
	}

	coordinateSystems.push_back(std::move(newCoordinateSystem));

	for (auto& child : children)
	{
		child->enterNode(cs);
	}
}

std::optional<const SceneMultiNode::SceneNode*> SceneMultiNode::getCoordinateSystem(CoordinateSystem cs) const
{
	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		if (coordinateSystems[i]->getType() == cs)
		{
			return coordinateSystems[i].get();
		}
	}

	return {};
}

std::optional<SceneMultiNode::SceneNode*> SceneMultiNode::getCoordinateSystem(CoordinateSystem cs)
{
	for (size_t i = 0; i < coordinateSystems.size(); ++i)
	{
		if (coordinateSystems[i]->getType() == cs)
		{
			return coordinateSystems[i].get();
		}
	}

	return {};
}

CoordinateSystem SceneMultiNode::getCurrentCoordinateSystem() const
{
	if (!coordinateSystems.empty())
	{
		int minimalCs = CoordinateSystem::Intergalactical;

		for (const auto& cs : coordinateSystems)
		{
			if (cs->getType() < minimalCs)
			{
				minimalCs = cs->getType();
			}
		}

		return static_cast<CoordinateSystem>(minimalCs);
	}

	throw std::runtime_error("There are no coordinate systems in the scene node");
}
