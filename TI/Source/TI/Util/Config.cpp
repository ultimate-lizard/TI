#include "Config.h"

#include <filesystem>
#include <fstream>
#include <sstream>

static const char* CONTROLS_SECTION_NAME = "CONTROLS";

void Config::load(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        return;
    }

    std::fstream file;
    file.open(path, std::ios_base::in | std::ios_base::out);
    if (!file.is_open())
    {
        return;
    }

    parse(file);
}

const std::vector<KeyEntry>& Config::getKeyEntries() const
{
    return keyEntries;
}

const std::vector<AxisEntry>& Config::getAxisEntries() const
{
    return axisEntries;
}

void Config::parse(std::fstream& file)
{
    std::string line;
    std::string currentSection;
    while (std::getline(file, line))
    {
        // Select section
        if (line == CONTROLS_SECTION_NAME)
        {
            currentSection = line;
            continue;
        }

		if (line.size() > 0)
		{
			if (line[0] == '#')
			{
				continue;
			}
		}

        // Read section
        if (currentSection == CONTROLS_SECTION_NAME)
        {
            parseControlsLine(line);
        }
    }
}

void Config::parseControlsLine(const std::string& line)
{
	std::stringstream ss(line);

	std::string bindingType;
	std::string bindingName;
	std::string binding;
	float scale = 0.0f;

	if (ss >> bindingType >> bindingName >> binding)
	{
		if (bindingType == "AXIS")
		{
			ss >> scale;
			AxisEntry axisEntry;
			axisEntry.bindingName = bindingName;
			Axis axis = stringToAxis(binding);
			Key key = stringToKey(binding);
			axisEntry.axis = axis;
			axisEntry.key = key;
			axisEntry.scale = scale;

			axisEntries.push_back(std::move(axisEntry));
		}
		else
		{
			KeyEntry keyEntry;
			keyEntry.bindingName = bindingName;
			keyEntry.key = stringToKey(std::move(binding));
			keyEntries.push_back(std::move(keyEntry));
		}
	}
}

Key Config::stringToKey(const std::string& string)
{
	if (string == "A") return Key::A;
	if (string == "B") return Key::B;
	if (string == "C") return Key::C;
	if (string == "D") return Key::D;
	if (string == "E") return Key::E;
	if (string == "F") return Key::F;
	if (string == "G") return Key::G;
	if (string == "H") return Key::H;
	if (string == "I") return Key::I;
	if (string == "J") return Key::J;
	if (string == "K") return Key::K;
	if (string == "L") return Key::L;
	if (string == "M") return Key::M;
	if (string == "N") return Key::N;
	if (string == "O") return Key::O;
	if (string == "P") return Key::P;
	if (string == "Q") return Key::Q;
	if (string == "R") return Key::R;
	if (string == "S") return Key::S;
	if (string == "T") return Key::T;
	if (string == "U") return Key::U;
	if (string == "V") return Key::V;
	if (string == "W") return Key::W;
	if (string == "X") return Key::X;
	if (string == "Y") return Key::Y;
	if (string == "Z") return Key::Z;
	if (string == "Escape") return Key::Escape;
	if (string == "ControllerButtonB") return Key::ControllerButtonB;
	if (string == "Right") return Key::Right;
	if (string == "Left") return Key::Left;
	if (string == "Down") return Key::Down;
	if (string == "Up") return Key::Up;

	return Key::UnknownKey;
}

Axis Config::stringToAxis(const std::string& string)
{
	if (string == "MouseX") return Axis::MouseX;
	if (string == "MouseY") return Axis::MouseY;
	if (string == "ControllerLeftStickX") return Axis::ControllerLeftStickX;
	if (string == "ControllerLeftStickY") return Axis::ControllerLeftStickY;
	if (string == "ControllerRightStickX") return Axis::ControllerRightStickX;
	if (string == "ControllerRightStickY") return Axis::ControllerRightStickY;
	return Axis::UnknownAxis;
}
