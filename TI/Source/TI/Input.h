#pragma once

#include <string>

#include <SDL.h>

enum Key
{
	UnknownKey = 0,
	A = SDL_SCANCODE_A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Escape = SDL_SCANCODE_ESCAPE,
	LeftCtrl = SDL_SCANCODE_LCTRL,
	ControllerButtonB = SDL_CONTROLLER_BUTTON_B,
	Right = SDL_SCANCODE_RIGHT,
	Left,
	Down,
	Up,
};

enum Axis
{
	UnknownAxis = 0,
	MouseX = 513,
	MouseY,
	ControllerLeftStickX,
	ControllerLeftStickY,
	ControllerRightStickX,
	ControllerRightStickY
};

enum ActionInputType
{
	KeyPress,
	KeyRelease
};

class Application;
class InputHandler;

class Input
{
public:
	Input(Application* app);
	void handleInput();

	void releaseMouse();

private:
	void handleMouseMotion(InputHandler* inputHandler);
	void handleControllerAxisMotion(InputHandler* inputHandler);
	void handleWindowFocus(bool gained);
	void handleWindowResized();

	Application* app;
	SDL_Event event;

	int lastX;
	int lastY;

	bool mouseReleased;
};