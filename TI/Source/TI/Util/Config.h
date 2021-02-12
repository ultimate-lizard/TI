#pragma once

#include <string>
#include <vector>

#include <TI/Input.h>

struct KeyEntry
{
	std::string bindingName;
	Key key;
};

struct AxisEntry
{
	std::string bindingName;
	Axis axis;
	Key key;
	float scale;
};

class Config
{
public:
	void load(const std::string& path);

	const std::vector<KeyEntry>& getKeyEntries() const;
	const std::vector<AxisEntry>& getAxisEntries() const;

private:
	void parse(std::fstream& file);
	void parseControlsLine(const std::string& line);

	Key stringToKey(const std::string& string);
	Axis stringToAxis(const std::string& string);

	std::vector<KeyEntry> keyEntries;
	std::vector<AxisEntry> axisEntries;
};
