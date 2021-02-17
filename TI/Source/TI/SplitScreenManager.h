#pragma once

#include <vector>

class LocalClient;
class Application;

enum class SplitScreenOrientation
{
	Horizontal,
	Vertical
};

class SplitScreenManager
{
public:
	SplitScreenManager(Application* app);

	void setHost(LocalClient* host);
	void addGuest(LocalClient* guest);

	void setOrientation(SplitScreenOrientation orientation);

	void displayHost();
	void displayGuest(LocalClient* guest);
	void displayAll();

private:
	LocalClient* host;
	std::vector<LocalClient*> guests;

	Application* app;

	SplitScreenOrientation orientation;
};
