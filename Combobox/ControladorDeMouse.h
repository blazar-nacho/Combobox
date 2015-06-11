#pragma once
#include "EventoDeMouse.h"

class ControladorDeMouse
{
public:
	ControladorDeMouse();
	void actualizar();
	EventoDeMouse* getEventoDeMouse();
	~ControladorDeMouse();

private:
	EventoDeMouse* eventoDeMouse;
};

