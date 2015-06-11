#include "stdafx.h"
#include "Modo1vs1.h"


Modo1vs1::Modo1vs1()
{
	controlador1 = Parser::getInstancia().getControlador1();
	controlador2 = Parser::getInstancia().getControlador2();
}

Modo1vs1::~Modo1vs1()
{
	controlador1 = nullptr;
	controlador2 = nullptr;
}
