#pragma once

class Model;
class CelestialBody;

class AstroBodyMesh
{
public:
	AstroBodyMesh(CelestialBody* astroBody, Model* model);

	Model* getModel();
	inline CelestialBody* getAstroBody()
	{
		return astroBody;
	}

private:
	CelestialBody* astroBody;
	Model* model;
};
