#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class PhysicsComponent : public Component
{
public:
	PhysicsComponent();

	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void addForce(const glm::vec3& force);

private:
	glm::vec3 getGravityVector() const;
	void updatePlaneSideRotation(float dt);

	glm::vec3 velocity;

	glm::vec3 previousUpVector;
	float pendingRotationAngle;
	bool shouldRotate;
	bool planeSideTransitionInProgress;
	float currentRotationAngle;
	glm::vec3 sideRotationAxis;

	float friction;
};
