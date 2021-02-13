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
	Config() : pendingSave(false) {}

	void load(const std::string& path);
	void save();

	const std::vector<KeyEntry>& getKeyEntries() const;
	const std::vector<AxisEntry>& getAxisEntries() const;

private:
	void parse(std::fstream& file);
	void parseControlsLine(const std::string& line);

	Key stringToKey(const std::string& string);
	std::string keyToString(Key key);

	Axis stringToAxis(const std::string& string);
	std::string axisToString(Axis axis);

	std::vector<KeyEntry> keyEntries;
	std::vector<AxisEntry> axisEntries;

	std::string path;

	bool pendingSave;
};
