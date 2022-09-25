#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

enum CoordinateSystem
{
	Planetary = 0,
	Interplanetary,
	Interstellar
};

class SceneMultiNode
{
	class SceneNode
	{
	public:
		SceneNode();

		SceneNode(const SceneNode&);
		SceneNode(SceneNode&&) = delete;

		glm::mat4 getTransform() const;
		glm::mat4 getLocalTransform() const;

		void setPosition(const glm::vec3& position);
		void offset(const glm::vec3& position);
		void setLocalOrientation(const glm::quat& orientation);
		void setScale(const glm::vec3& scale);
		void setRotation(const glm::vec3& rotation);
		// void setRotationInWorldSpace(const glm::vec3& rotation);

		void rotateInWorldSpace(float angle, const glm::vec3& axis);
		void rotate(float angle, const glm::vec3& axis);

		glm::vec3 getLocalPosition() const;
		glm::vec3 getDerivedPosition() const;

		glm::quat getLocalOrientation() const;
		glm::quat getDerivedOrientation() const;

		glm::vec3 getScale() const;
		// glm::vec3 getRotation() const;

		void setParent(SceneNode* parent);
		void removeParent();

		void addChild(SceneNode* child);
		void removeChild(SceneNode* child);

		bool isChildOf(SceneNode* node);

		glm::vec3 getForwardVector();
		glm::vec3 getUpVector();
		glm::vec3 getRightVector();

		glm::vec3 getLocalForwardVector();
		glm::vec3 getLocalUpVector();
		glm::vec3 getLocalRightVector();

	protected:
		void updateTransform();
		glm::quat getOrientationInWorldSpace() const;

	protected:
		SceneNode* parent;
		std::vector<SceneNode*> children;

		glm::mat4 transform;

		glm::vec3 localPosition;
		glm::quat localOrientation;
		glm::vec3 localScale;
	};

public:
	SceneMultiNode();

	SceneMultiNode(const SceneMultiNode&);
	SceneMultiNode(SceneMultiNode&&) = delete;

	glm::mat4 getTransform(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	void setLocalPosition(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void offset(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setLocalOrientation(const glm::quat& orientation, CoordinateSystem cs = CoordinateSystem::Planetary, bool propagateToChildren = false);
	void setLocalScale(const glm::vec3& scale, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setRotation(const glm::vec3& rotation, CoordinateSystem cs = CoordinateSystem::Planetary);
	// void setRotationInWorldSpace(const glm::vec3& rotation);

	void rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotateInWorldSpaceExclusive(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotate(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);

	glm::vec3 getLocalPosition(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::vec3 getDerivedPosition(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	glm::quat getLocalOrientation(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::quat getDerivedOrientation(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	glm::vec3 getScale(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	// glm::vec3 getRotation() const;

	glm::vec3 getForwardVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getUpVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getRightVector(CoordinateSystem cs = CoordinateSystem::Planetary);

	glm::vec3 getLocalForwardVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getLocalUpVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getLocalRightVector(CoordinateSystem cs = CoordinateSystem::Planetary);

	void setParent(SceneMultiNode* parent, CoordinateSystem cs = CoordinateSystem::Planetary);
	void removeParent(CoordinateSystem cs = CoordinateSystem::Planetary);

	void addChild(SceneMultiNode* child, CoordinateSystem cs = CoordinateSystem::Planetary);
	void removeChild(SceneMultiNode* child, CoordinateSystem cs = CoordinateSystem::Planetary);

	bool isChildOf(SceneMultiNode* node, CoordinateSystem cs = CoordinateSystem::Planetary);

// protected:
	// void updateTransform();
	// glm::quat getOrientationInWorldSpace() const;

private:
	std::array<SceneNode, 3> coordinateSystems;
};
