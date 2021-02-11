#pragma once

#include <map>
#include <vector>
#include <functional>
#include <string>

#include <TI/Input.h>

struct AxisMapping
{
	std::string name;
	float scale;
};

struct ActionBinding
{
	std::function<void()> function;
	ActionInputType inputType;
};

class InputHandler
{
public:
	void onKeyInput(int key, ActionInputType inputType);
	void onAxisInput(int axis, float value);

	void bindKey(const std::string& binding, ActionInputType inputType, std::function<void()> function);
	void bindAxis(std::string binding, std::function<void(float)> function);

	template <typename ... Args>
	void addAxisMapping(Args&& ... args)
	{
		axisMappings.emplace(std::forward<Args>(args)...);
	}

	template <typename ... Args>
	void addKeyMapping(Args&& ... args)
	{
		keyMappings.emplace(std::forward<Args>(args)...);
	}

private:
	std::map<int, std::string> keyMappings;
	std::map<int, AxisMapping> axisMappings;

	std::map<std::string, std::vector<std::function<void(float)>>> axisBindings;
	std::map<std::string, std::vector<ActionBinding>> keyBindings;
};
