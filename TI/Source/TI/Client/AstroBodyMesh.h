#pragma once

class Model;
class AstroBody;

class AstroBodyMesh
{
public:
	AstroBodyMesh(AstroBody* astroBody, Model* model);

	Model* getModel();
	inline AstroBody* getAstroBody()
	{
		return astroBody;
	}

private:
	AstroBody* astroBody;
	Model* model;
};
