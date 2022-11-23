#pragma once

class Model;
class CelestialBody;

class CelestialBodyMesh
{
public:
	CelestialBodyMesh(CelestialBody* astroBody, Model* model);

	Model* getModel();
	inline CelestialBody* getCelestialBody()
	{
		return astroBody;
	}

private:
	CelestialBody* astroBody;
	Model* model;
};
