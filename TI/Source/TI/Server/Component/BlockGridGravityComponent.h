#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class BlockGridGravityComponent : public Component
{
public:
	BlockGridGravityComponent();

	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	glm::vec3 getGravityVector() const;

	void setRotationEnabled(bool enabled);
	bool isRotationEnabled() const;

private:
	void updateSideRotation(float dt);

	glm::vec3 previousUpVector;
	float pendingRotationAngle;
	bool shouldRotate;
	bool planeSideTransitionInProgress;
	float currentRotationAngle;
	glm::vec3 sideRotationAxis;

	bool rotationEnabled;
};