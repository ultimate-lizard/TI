#define SDL_MAIN_HANDLED

#include <iostream>

#include <TI/Application.h>

int main(int argc, char* argv[])
{
	std::cout << argv[0] << std::endl;

	Application App;
	App.start();

	return 0;
}
