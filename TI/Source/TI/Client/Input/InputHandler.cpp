#include "InputHandler.h"

#include <SDL.h>

void InputHandler::onKeyInput(int key, ActionInputType inputType)
{
	auto keyIter = keyMappings.find(key);
	if (keyIter != keyMappings.end())
	{
		auto iter = keyBindings.find(keyIter->second);
		if (iter != keyBindings.end())
		{
			auto& bindings = iter->second;
			for (auto& action : bindings)
			{
				if (action.inputType == inputType)
				{
					action.function();
				}
			}
		}
	}
}

void InputHandler::bindKey(const std::string& binding, ActionInputType inputType, std::function<void()> function)
{
	auto iter = keyBindings.find(binding);
	ActionBinding actionBinding({ function, inputType });
	if (iter != keyBindings.end())
	{
		auto& vec = iter->second;
		vec.push_back(actionBinding);
	}
	else
	{
		std::vector<ActionBinding> vec;
		vec.push_back(actionBinding);

		keyBindings.emplace(binding, vec);
	}
}

void InputHandler::onAxisInput(int axis, float value)
{
	auto keyIter = axisMappings.find(axis);
	if (keyIter != axisMappings.end())
	{
		auto iter = axisBindings.find(keyIter->second.name);
		if (iter != axisBindings.end())
		{
			auto& bindings = iter->second;
			for (auto& func : bindings)
			{
				func(value * keyIter->second.scale);
			}
		}
	}
}

void InputHandler::bindAxis(std::string bindingName, std::function<void(float)> function)
{
	auto iter = axisBindings.find(bindingName);
	if (iter != axisBindings.end())
	{
		auto& vec = iter->second;
		vec.push_back(function);
	}
	else
	{
		std::vector<std::function<void(float)>> vec;
		vec.push_back(function);

		axisBindings.emplace(bindingName, std::move(vec));
	}
}
