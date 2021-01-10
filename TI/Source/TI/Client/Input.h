#pragma once

#include <string>

#include <SDL.h>

enum Key
{
	W = SDL_SCANCODE_W,
	S = SDL_SCANCODE_S,
	A = SDL_SCANCODE_A,
	D = SDL_SCANCODE_D,
	Escape = SDL_SCANCODE_ESCAPE,
	ControllerButtonB = SDL_CONTROLLER_BUTTON_B
};

enum Axis
{
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
