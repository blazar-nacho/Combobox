#include "stdafx.h"
#include "Modo1vsCom.h"
#include "ControladorAutomatico.h"


Modo1vsCom::Modo1vsCom()
{
	ControladorAutomatico* controladorAutomatico;
	controlador1 = Parser::getInstancia().getControlador1();
	controlador2 = new ControladorAutomatico();
	controladorAutomatico = (ControladorAutomatico*)controlador2;

}

void Modo1vsCom::setNivelDeDificultad(int unNivel){
	ControladorAutomatico* unControlador = (ControladorAutomatico*)controlador2;
	unControlador->setNivel(unNivel);
}


Modo1vsCom::~Modo1vsCom()
{
	controlador1 = nullptr;
	controlador2 = nullptr;
}
