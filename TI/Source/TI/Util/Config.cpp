#include "Config.h"

#include <filesystem>
#include <fstream>
#include <sstream>

static const char* CONTROLS_SECTION_NAME = "CONTROLS";

void Config::load(const std::string& path)
{
	this->path = path;

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

	file.close();
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

void Config::save()
{
	if (pendingSave)
	{
		if (!keyEntries.empty() || !axisEntries.empty())
		{
			std::ofstream file;
			file.open(path);

			file << CONTROLS_SECTION_NAME << std::endl;

			for (const auto& entry : keyEntries)
			{
				file << "KEY" << " " << entry.bindingName << " " << keyToString(entry.key) << std::endl;
			}

			for (const auto& entry : axisEntries)
			{
				file << "AXIS" << " " << entry.bindingName << " ";
				if (entry.axis == Axis::UnknownAxis)
				{
					file << keyToString(entry.key) << " ";
				}
				else
				{
					file << axisToString(entry.axis) << " ";
				}

				file << entry.scale << std::endl;
			}

			file.close();
		}
	}
}

Key Config::stringToKey(const std::string& string)
{
	if (string == "F1") return Key::F1;
	if (string == "F2") return Key::F2;
	if (string == "F3") return Key::F3;
	if (string == "F4") return Key::F4;
	if (string == "F5") return Key::F5;
	if (string == "F6") return Key::F6;
	if (string == "F7") return Key::F7;
	if (string == "F8") return Key::F8;
	if (string == "F9") return Key::F9;
	if (string == "F10") return Key::F10;
	if (string == "F11") return Key::F11;
	if (string == "F12") return Key::F12;
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
	if (string == "Enter") return Key::Enter;
	if (string == "Space") return Key::Space;
	if (string == "LeftCtrl") return Key::LeftCtrl;
	if (string == "ControllerButtonA") return Key::ControllerButtonA;
	if (string == "ControllerButtonB") return Key::ControllerButtonB;
	if (string == "ControllerButtonX") return Key::ControllerButtonX;
	if (string == "ControllerButtonY") return Key::ControllerButtonY;
	if (string == "Right") return Key::Right;
	if (string == "Left") return Key::Left;
	if (string == "Down") return Key::Down;
	if (string == "Up") return Key::Up;
	if (string == "MouseLeft") return Key::MouseLeft;
	if (string == "MouseRight") return Key::MouseRight;
	if (string == "ScrollUp") return Key::ScrollUp;
	if (string == "ScrollDown") return Key::ScrollDown;
	if (string == "ControllerTriggerLeft") return Key::ControllerTriggerLeft;
	if (string == "ControllerTriggerRight") return Key::ControllerTriggerRight;
	if (string == "ControllerBumperLeft") return Key::ControllerBumperLeft;
	if (string == "ControllerBumperRight") return Key::ControllerBumperRight;

	return Key::UnknownKey;
}

std::string Config::keyToString(Key key)
{
	if (key == Key::F1) return "F1";
	if (key == Key::F2) return "F2";
	if (key == Key::F3) return "F3";
	if (key == Key::F4) return "F4";
	if (key == Key::F5) return "F5";
	if (key == Key::F6) return "F6";
	if (key == Key::F7) return "F7";
	if (key == Key::F8) return "F8";
	if (key == Key::F9) return "F9";
	if (key == Key::F10) return "F10";
	if (key == Key::F11) return "F11";
	if (key == Key::F12) return "F12";
	if (key == Key::A) return "A";
	if (key == Key::B) return "B";
	if (key == Key::C) return "C";
	if (key == Key::D) return "D";
	if (key == Key::E) return "E";
	if (key == Key::F) return "F";
	if (key == Key::G) return "G";
	if (key == Key::H) return "H";
	if (key == Key::I) return "I";
	if (key == Key::J) return "J";
	if (key == Key::K) return "K";
	if (key == Key::L) return "L";
	if (key == Key::M) return "M";
	if (key == Key::N) return "N";
	if (key == Key::O) return "O";
	if (key == Key::P) return "P";
	if (key == Key::Q) return "Q";
	if (key == Key::R) return "R";
	if (key == Key::S) return "S";
	if (key == Key::T) return "T";
	if (key == Key::U) return "U";
	if (key == Key::V) return "V";
	if (key == Key::W) return "W";
	if (key == Key::X) return "X";
	if (key == Key::Y) return "Y";
	if (key == Key::Z) return "Z";
	if (key == Key::Enter) return "Enter";
	if (key == Key::Space) return "Space";
	if (key == Key::Escape) return "Escape";
	if (key == Key::LeftCtrl) return "LeftCtrl";
	if (key == Key::ControllerButtonA) return "ControllerButtonA";
	if (key == Key::ControllerButtonB) return "ControllerButtonB";
	if (key == Key::ControllerButtonX) return "ControllerButtonX";
	if (key == Key::ControllerButtonY) return "ControllerButtonY";
	if (key == Key::Right) return "Right";
	if (key == Key::Left) return "Left";
	if (key == Key::Down) return "Down";
	if (key == Key::Up) return "Up";
	if (key == Key::MouseLeft) return "MouseLeft";
	if (key == Key::MouseRight) return "MouseRight";
	if (key == Key::ScrollUp) return "ScrollUp";
	if (key == Key::ScrollDown) return "ScrollDown";
	if (key == Key::ControllerTriggerLeft) return "ControllerTriggerLeft";
	if (key == Key::ControllerTriggerRight) return "ControllerTriggerRight";
	if (key == Key::ControllerBumperLeft) return "ControllerBumperLeft";
	if (key == Key::ControllerBumperRight) return "ControllerBumperRight";

	return "UnknownKey";
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

std::string Config::axisToString(Axis axis)
{
	if (axis == Axis::MouseX) return "MouseX";
	if (axis == Axis::MouseY) return "MouseY";
	if (axis == Axis::ControllerLeftStickX) return "ControllerLeftStickX";
	if (axis == Axis::ControllerLeftStickY) return "ControllerLeftStickY";
	if (axis == Axis::ControllerRightStickX) return "ControllerRightStickX";
	if (axis == Axis::ControllerRightStickY) return "ControllerRightStickY";

	return "UnknownAxis";
}
