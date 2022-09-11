#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

enum CoordinateSystem
{
	Planetary = 0,
	Interplanetary = 1
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

		void setPosition(const glm::vec3& position);
		void offset(const glm::vec3& position);
		void setOrientation(const glm::quat& orientation);
		void setScale(const glm::vec3& scale);
		void setRotation(const glm::vec3& rotation);
		// void setRotationInWorldSpace(const glm::vec3& rotation);

		void rotateInWorldSpace(float angle, const glm::vec3& axis);
		void rotate(float angle, const glm::vec3& axis);

		glm::vec3 getPosition() const;
		glm::quat getOrientation() const;
		glm::vec3 getScale() const;
		// glm::vec3 getRotation() const;

		void setParent(SceneNode* parent);
		void addChild(SceneNode* child);
		bool isChildOf(SceneNode* node);

		glm::vec3 getForwardVector();
		glm::vec3 getUpVector();
		glm::vec3 getRightVector();

	protected:
		void updateTransform();
		glm::quat getOrientationInWorldSpace() const;

	protected:
		SceneNode* parent;
		std::vector<SceneNode*> children;

		glm::mat4 transform;

		glm::vec3 position;
		glm::quat orientation;
		glm::vec3 scale;
	};

public:
	SceneMultiNode();

	SceneMultiNode(const SceneMultiNode&);
	SceneMultiNode(SceneMultiNode&&) = delete;

	glm::mat4 getTransform(CoordinateSystem cs = CoordinateSystem::Planetary) const;

	void setPosition(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void offset(const glm::vec3& position, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setOrientation(const glm::quat& orientation, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setScale(const glm::vec3& scale, CoordinateSystem cs = CoordinateSystem::Planetary);
	void setRotation(const glm::vec3& rotation, CoordinateSystem cs = CoordinateSystem::Planetary);
	// void setRotationInWorldSpace(const glm::vec3& rotation);

	void rotateInWorldSpace(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotateInWorldSpaceExclusive(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);
	void rotate(float angle, const glm::vec3& axis, CoordinateSystem cs = CoordinateSystem::Planetary);

	glm::vec3 getPosition(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::quat getOrientation(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	glm::vec3 getScale(CoordinateSystem cs = CoordinateSystem::Planetary) const;
	// glm::vec3 getRotation() const;

	glm::vec3 getForwardVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getUpVector(CoordinateSystem cs = CoordinateSystem::Planetary);
	glm::vec3 getRightVector(CoordinateSystem cs = CoordinateSystem::Planetary);

	void setParent(SceneMultiNode* parent, CoordinateSystem cs = CoordinateSystem::Planetary);
	void addChild(SceneMultiNode* child);
	bool isChildOf(SceneMultiNode* node);

// protected:
	// void updateTransform();
	// glm::quat getOrientationInWorldSpace() const;

private:
	std::array<SceneNode, 2> coordinateSystems;
};
