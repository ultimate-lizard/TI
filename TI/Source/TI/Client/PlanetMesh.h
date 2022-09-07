#pragma once

// #include <TI/Server/SceneNode.h>
// #include <TI/Server/AstroBody.h>

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
