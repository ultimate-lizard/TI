#include <iostream>
#include <time.h>

#include <SDL.h>

#include <TI/Application.h>
#include <TI/Network/Protocol.h>

int main(int argc, char* argv[])
{
	srand(time(NULL));

	std::vector<std::string> args;

	for (int i = 1; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}

	Application App(args);
	App.start();

	return 0;
}
