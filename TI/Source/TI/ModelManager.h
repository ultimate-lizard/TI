#pragma once

#include <memory>
#include <map>
#include <string>

#include <TI/Renderer/Model.h>

class ModelManager
{
public:
	ModelManager();

	Model* const findModel(const std::string& name) const;

private:
	std::map<std::string, std::unique_ptr<Model>> models;
};
