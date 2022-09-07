#include "AstroBody.h"

AstroBody::AstroBody(CoordinateSystemScale coordinateSystem) :
	coordinateSystem(coordinateSystem)
{

}

AstroBody::~AstroBody()
{
}

//void AstroBody::addBodyToOrbit(std::unique_ptr<AstroBody> body)
//{
//	orbitingBodies.push_back(std::move(body));
//}

CoordinateSystemScale AstroBody::getCoordinateSystemScale() const
{
	return coordinateSystem;
}

//const std::vector<std::unique_ptr<AstroBody>>& AstroBody::getOrbitingBodies() const
//{
//	return orbitingBodies;
//}
