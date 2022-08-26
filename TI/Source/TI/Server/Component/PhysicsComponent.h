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
	//glm::vec3 originalSize;
	//glm::vec3 originalOffset;

	//OrientationInfo cachedOrientation;

public:
	glm::vec3 size;
	glm::vec3 offset;

	//inline CollisionBox(const glm::vec3& size, const glm::vec3& offset) :
	//	originalSize(size),
	//	originalOffset(offset)
	//{

	//}

	// TODO: Unhardcore this!
	inline void orient(const OrientationInfo& orientationInfo)
	{
		//cachedOrientation = orientationInfo;

		static bool setOriginals = false;

		static glm::vec3 originalSize;
		static glm::vec3 originalOffset;

		if (!setOriginals)
		{
			originalSize = size;
			originalOffset = offset;
		}

		size[orientationInfo.sideAxis] = originalSize.x;
		size[orientationInfo.heightAxis] = originalSize.y;
		size[orientationInfo.frontAxis] = originalSize.z;

		offset[orientationInfo.sideAxis] = originalOffset.x;
		offset[orientationInfo.heightAxis] = originalOffset.y;
		offset[orientationInfo.frontAxis] = originalOffset.z;

		if (!orientationInfo.positive)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				// newSize[i] *= -1.0f;
				offset[i] *= -1.0f;
			}
		}

		setOriginals = true;
		
		/*if (orientationInfo == Orientations::TOP)
		{
			size = { 0.6f, 1.9f, 0.6f };
			offset = { 0.0f, 0.85f, 0.0f };

		}
		else if (orientationInfo == Orientations::FRONT)
		{
			1 2 0
			y z x
			size = { 0.6f, 0.6f, 1.9f };
			offset = { 0.0f, 0.0f, 0.85f };
		}
		else if (orientationInfo == Orientations::RIGHT)
		{
			size = { 1.9f, 0.6f, 0.6f };
			offset = { 0.85f, 0.0f, 0.0f };
		}
		else if (orientationInfo == Orientations::BOTTOM)
		{
			size = { 0.6f, 1.9f, 0.6f };
			offset = { 0.0f, -0.85f, 0.0f };
		}
		else if (orientationInfo == Orientations::BACK)
		{
			size = { 0.6f, 0.6f, 1.9f };
			offset = { 0.0f, 0.0f, -0.85f };
		}
		else if (orientationInfo == Orientations::LEFT)
		{
			size = { 1.9f, 0.6f, 0.6f };
			offset = { -0.85f, 0.0f, 0.0f };
		}*/
	}
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
