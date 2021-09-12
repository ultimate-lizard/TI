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

	void addModel(const std::string& name, std::unique_ptr<Model> model);
	void removeModel(const std::string& name);

private:
	std::map<std::string, std::unique_ptr<Model>> models;
};
