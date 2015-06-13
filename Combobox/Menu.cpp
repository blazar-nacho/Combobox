#include "stdafx.h"
#include "Menu.h"
#include "ControladorAutomatico.h"

Menu::Menu(Modo* unModo)
{
	modo = unModo;
	vista = unModo->getRefVista();
	controladorDeMouse = new ControladorDeMouse(); 
	modoSeleccionado = NINGUNO;
	torre = NINGUNA;
	personaje1 = NINGUNPJ;
	personaje2 = NINGUNPJ;
	personajes.seleccionados = NO;
}

int Menu::seleccionarModo(Controlador* unControlador, Controlador* otroControlador){
	int estado = 0;	
	std::string nombreP1;
	std::string nombreP2;

	while (true){
		int estado = unControlador->cambiar();
		int estado2 = otroControlador->cambiar();
		controladorDeMouse->actualizar();

		if (modoSeleccionado == COM){
			if (personajes.seleccionados == NO)
				personajes = vista->elegirPersonajes(unControlador, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
			else
				torre = vista->elegirDificultad(unControlador, controladorDeMouse->getEventoDeMouse());
		}
		if (modoSeleccionado == VS){
			//Habilitar entrada de texto
			SDL_StartTextInput();
			if (personajes.seleccionados == NO){
				personajes = vista->elegirPersonajes(unControlador, otroControlador, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
			}
		}
		if (modoSeleccionado == PRACTICA){
			if (personajes.seleccionados == NO)
				personajes = vista->elegirPersonajes(unControlador, otroControlador, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
		}

		if ((modoSeleccionado == NINGUNO)){
			if (MODO_RAPIDO){
				personajes.seleccionados = SI;
				modoSeleccionado = VS;
			}
			else
				modoSeleccionado = vista->actualizarMenu(unControlador, controladorDeMouse->getEventoDeMouse());
		}
		else if ((personajes.seleccionados == SI)){
			modo->setPersonajes(personajes);
			Parser::getInstancia().getPelea()->getPersonaje1()->setNombreActual(nombreP1);
			Parser::getInstancia().getPelea()->getPersonaje2()->setNombreActual(nombreP2);
			if ((modoSeleccionado == VS) || (modoSeleccionado == PRACTICA))
				break;
			else if (torre!=NINGUNA)
				break;
		}
		SDL_Delay(70);
	}
	modo->seleccionarModo(modoSeleccionado, torre);
	vista->setModoActual(modoSeleccionado);
	return estado;
}


Modo* Menu::getModo(){
	return modo;
}

MODOS_DE_JUEGO Menu::getModoSeleccionado(){
	return modoSeleccionado;
}

Menu::~Menu()
{
	if (modo != nullptr) delete modo;
	delete controladorDeMouse;
	modoSeleccionado = NINGUNO;
	torre = NINGUNA;
	personaje1 = NINGUNPJ;
	personaje2 = NINGUNPJ;
	personajes.seleccionados = NO;
}
