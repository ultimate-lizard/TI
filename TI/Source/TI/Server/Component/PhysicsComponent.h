#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>
#include <TI/Server/PlaneSide.h>

class TransformComponent;

struct CollisionBox
{
private:
	glm::vec3 unorientedSize;
	glm::vec3 unorientedOffset;

	glm::vec3 size;
	glm::vec3 offset;

	// OrientationInfo cachedOrientation;

public:
	inline CollisionBox() :
		unorientedSize(0.0f),
		unorientedOffset(0.0f),
		size(0.0f),
		offset(0.0f)
		//cachedOrientation()
	{

	}

	inline CollisionBox(const glm::vec3& size, const glm::vec3& offset) :
		unorientedSize(size),
		unorientedOffset(offset),
		size(size),
		offset(offset)
		//cachedOrientation()
	{

	}

	inline void setSize(const glm::vec3& newSize)
	{
		unorientedSize = newSize;
		//orient(cachedOrientation);
	}

	inline void setOffset(const glm::vec3& newOffset)
	{
		unorientedOffset = newOffset;
		//orient(cachedOrientation);
	}

	// Returns oriented in space size
	inline glm::vec3 getSize() const
	{
		return size;
	}

	// Returns oriented in space offset
	inline glm::vec3 getOffset() const
	{
		return offset;
	}

	// Returns oriented in space size
	inline glm::vec3 getUnorientedSize() const
	{
		return unorientedSize;
	}

	// Returns oriented in space offset
	inline glm::vec3 getUnorientedOffset() const
	{
		return unorientedOffset;
	}

	// TODO: Unhardcore this!
	//inline void orient(const OrientationInfo& orientationInfo)
	//{
	//	if (cachedOrientation != orientationInfo)
	//	{
	//		cachedOrientation = orientationInfo;
	//	}

	//	size[orientationInfo.sideAxis] = unorientedSize.x;
	//	size[orientationInfo.heightAxis] = unorientedSize.y;
	//	size[orientationInfo.frontAxis] = unorientedSize.z;

	//	offset[orientationInfo.sideAxis] = unorientedOffset.x;
	//	offset[orientationInfo.heightAxis] = unorientedOffset.y;
	//	offset[orientationInfo.frontAxis] = unorientedOffset.z;

	//	if (!orientationInfo.positive)
	//	{
	//		for (size_t i = 0; i < 3; ++i)
	//		{
	//			// newSize[i] *= -1.0f;
	//			offset[i] *= -1.0f;
	//		}
	//	}
	//}
};

struct CollisionResult
{
	glm::bvec3 collidedAxis;
	glm::vec3 adjustedVelocity;
	glm::vec3 adjustedPosition;
};

struct RayCollisionResult
{
	float near;
	float far;
	glm::vec3 normal;
};

class PhysicsComponent : public Component
{
	enum class Axis
	{
		X,
		Y,
		Z
	};

public:
	PhysicsComponent();
	PhysicsComponent(const PhysicsComponent& other);
	PhysicsComponent(PhysicsComponent&&) = delete;

	void init() override;
	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void setCollisionBox(CollisionBox collisionBox);
	CollisionBox getCollisionBox() const;

	CollisionResult resolveCollision(const glm::vec3& position, const glm::vec3& velocity, float dt);
	
	void setRenderCollisionBoxEnabled(bool renderBoxEnabled);
	bool isRenderCollisiobBoxEnabled() const;

	void setRenderCollisions(bool renderCollisions);
	bool isRenderCollisionsEnabled() const;

	std::optional<RayCollisionResult> checkRayVsAabb(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& box2pos, const CollisionBox& box2);
	bool checkCollision(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);

private:
	glm::vec3 calculateAabbDistanceTo(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);

private:
	TransformComponent* transformComponent;
	CollisionBox collisionBox;

	bool renderCollisionBox;
	bool renderCollisions;
};
