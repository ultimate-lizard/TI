#pragma once

#include <memory>

#include <TI/Server/AstroBody.h>

class Plane;

class Planet : public AstroBody
{
public:
	Planet(const glm::uvec3& size, size_t chunkSize);
	Planet(std::unique_ptr<Plane> plane);
	~Planet();

	Plane* getPlane();

private:
	std::unique_ptr<Plane> plane;
};
