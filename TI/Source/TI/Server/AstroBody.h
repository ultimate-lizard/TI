#pragma once

#include <memory>

#include <TI/Server/SceneNode.h>

class AstroBody : public SceneNode
{
public:
	AstroBody(CoordinateSystemScale coordinateSystem);
	~AstroBody();

	// void addBodyToOrbit(std::unique_ptr<AstroBody> body);

	CoordinateSystemScale getCoordinateSystemScale() const;
	// const std::vector<std::unique_ptr<AstroBody>>& getOrbitingBodies() const;

protected:
	CoordinateSystemScale coordinateSystem;
	// std::vector<std::unique_ptr<AstroBody>> orbitingBodies;
};
