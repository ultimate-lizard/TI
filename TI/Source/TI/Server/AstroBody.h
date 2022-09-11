#pragma once

#include <memory>

#include <TI/Server/SceneNode.h>

class AstroBody : public SceneMultiNode
{
public:
	AstroBody();
	~AstroBody();

	// void addBodyToOrbit(std::unique_ptr<AstroBody> body);

	// const std::vector<std::unique_ptr<AstroBody>>& getOrbitingBodies() const;

protected:
	// std::vector<std::unique_ptr<AstroBody>> orbitingBodies;
};
