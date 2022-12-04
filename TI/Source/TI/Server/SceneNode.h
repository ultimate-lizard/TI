#pragma once

#include <vector>
#include <optional>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

enum CoordinateSystem
{
	Planetary = 0,
	Interplanetary,
	Interstellar,
	Intergalactical,
	COUNT = 4
};

class SceneMultiNode
{
	class SceneNode
	{
	public:
		SceneNode();
		SceneNode(CoordinateSystem cs);

		SceneNode(const SceneNode& other);
		SceneNode& operator=(const SceneNode& other) = delete;
		SceneNode(SceneNode&&) noexcept = default;

		glm::mat4 getTransform() const;
		glm::mat4 getLocalTransform() const;

		void setPosition(const glm::vec3& position);
		void offset(const glm::vec3& position);
		void setLocalOrientation(const glm::quat& orientation);
		void setScale(const glm::vec3& scale);
		void setRotation(const glm::vec3& rotation);
		void setRotation(float angle, const glm::vec3& axis);

		void rotateInWorldSpace(float angle, const glm::vec3& axis);
		void rotate(float angle, const glm::vec3& axis);

		glm::vec3 getLocalPosition() const;
		glm::vec3 getDerivedPosition(bool inheritOrientation = true) const;

		glm::quat getLocalOrientation() const;
		glm::quat getDerivedOrientation() const;

		glm::vec3 getScale() const;

		void setParent(SceneNode* parent);
		void removeParent();

		bool isChildOf(SceneNode* node);

		glm::vec3 getForwardVector();
		glm::vec3 getUpVector();
		glm::vec3 getRightVector();

		glm::vec3 getLocalForwardVector();
		glm::vec3 getLocalUpVector();
		glm::vec3 getLocalRightVector();

		CoordinateSystem getType() const;

	protected:
		void updateTransform();
		glm::quat getOrientationInWorldSpace() const;

		SceneNode* parent;
		std::vector<SceneNode*> children;

		glm::mat4 transform;

		glm::vec3 localPosition;
		glm::quat localOrientation;
		glm::vec3 localScale;

		CoordinateSystem type;
	};

public:
	SceneMultiNode();
	SceneMultiNode(CoordinateSystem minimalCoordinateSystem);

	SceneMultiNode(const SceneMultiNode&);
	SceneMultiNode(SceneMultiNode&&) = delete;

	glm::mat4 getTransform(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	void setLocalPosition(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary, bool propagateToUpperCoordinateSystems = true);
	// void setLocalPositionExclusive(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void offset(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setLocalOrientation(const glm::quat& orientation, CoordinateSystem cs = CoordinateSystem::Planetary, bool propagateToUpperCoordinateSystems = false);
	void setLocalScale(const glm::vec3& scale, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setRotation(const glm::vec3& rotation, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setRotation(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);

	void rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotateInWorldSpaceExclusive(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotate(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);

	glm::vec3 getLocalPosition(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::vec3 getDerivedPosition(CoordinateSystem cs = CoordinateSystem::Planetary, bool inheritOrientation = true, bool returnNextCsWhenPossible = false) const;

	glm::quat getLocalOrientation(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::quat getDerivedOrientation(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	glm::vec3 getScale(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	glm::vec3 getForwardVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getUpVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getRightVector(CoordinateSystem cs = CoordinateSystem::Planetary);

	glm::vec3 getLocalForwardVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getLocalUpVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getLocalRightVector(CoordinateSystem cs = CoordinateSystem::Planetary);

	void setParent(SceneMultiNode* parent, CoordinateSystem cs = CoordinateSystem::Planetary);
	void removeParent(CoordinateSystem cs = CoordinateSystem::Planetary);

	bool isChildOf(SceneMultiNode* node, CoordinateSystem cs = CoordinateSystem::Planetary);

	void leaveNode(CoordinateSystem cs);
	void enterNode(CoordinateSystem cs);

	std::optional<const SceneMultiNode::SceneNode*> getCoordinateSystem(CoordinateSystem cs) const;
	std::optional<SceneMultiNode::SceneNode*> getCoordinateSystem(CoordinateSystem cs);

	CoordinateSystem getCurrentCoordinateSystem() const;

	std::string _name;

private:
	std::vector<std::unique_ptr<SceneMultiNode::SceneNode>> coordinateSystems;
	std::vector<std::unique_ptr<SceneMultiNode::SceneNode>> inactiveCoordinateSystems;
	SceneMultiNode* parent;
	std::vector<SceneMultiNode*> children;
};
