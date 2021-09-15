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
	Enter = SDL_SCANCODE_RETURN,
	Space = SDL_SCANCODE_SPACE,
	Escape = SDL_SCANCODE_ESCAPE,
	LeftCtrl = SDL_SCANCODE_LCTRL,
	ControllerButtonA = SDL_CONTROLLER_BUTTON_A,
	ControllerButtonB = SDL_CONTROLLER_BUTTON_B,
	ControllerButtonX = SDL_CONTROLLER_BUTTON_X,
	ControllerButtonY = SDL_CONTROLLER_BUTTON_Y,
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

class Input
{
public:
	Input(class Application* app);
	void handleInput();

	void releaseMouse();

private:
	void handleMouseMotion(class InputHandler* inputHandler);
	void handleControllerAxisMotion(class InputHandler* inputHandler);
	void handleWindowFocus(bool gained);
	void handleWindowResized();

	class Application* app;
	SDL_Event event;

	int lastX;
	int lastY;

	bool mouseReleased;
};