#include <iostream>
#include <time.h>

#include <SDL.h>

#include <TI/Application.h>

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(0)));

	std::vector<std::string> args;

	for (int i = 1; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}

	Application App(args);
	App.start();

	return 0;
}
