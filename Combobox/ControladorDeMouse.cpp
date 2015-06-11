#include "stdafx.h"
#include "ControladorDeMouse.h"


ControladorDeMouse::ControladorDeMouse()
{
	eventoDeMouse = new EventoDeMouse();
}


void ControladorDeMouse::actualizar(){
	SDL_PumpEvents();
	Uint8 botones = 0;
	int x, y;

	botones = SDL_GetMouseState(&x, &y);
	eventoDeMouse->setX(x);
	eventoDeMouse->setY(y);
	if (botones == 1) eventoDeMouse->setClick(true);
	else eventoDeMouse->setClick(false);
}


EventoDeMouse* ControladorDeMouse::getEventoDeMouse(){
	return eventoDeMouse;
}


ControladorDeMouse::~ControladorDeMouse()
{
	delete eventoDeMouse;
}
