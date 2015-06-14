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

	while (true){
		int estado = unControlador->cambiar();
		int estado2 = otroControlador->cambiar();
		controladorDeMouse->actualizar();

		if (modoSeleccionado == COM){
			if (personajes.seleccionados == NO)
				personajes = personajes = vista->elegirPersonajes(unControlador, personajes, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
			else{
				if (torre == NINGUNA)
					torre = vista->elegirDificultad(unControlador, controladorDeMouse->getEventoDeMouse());
				else{
				modo->setPersonajes(personajes);
				Parser::getInstancia().getPelea()->getPersonaje1()->setNombreActual(nombreP1);
				Parser::getInstancia().getPelea()->getPersonaje2()->setNombreActual(nombreP2);
				break;
				}
			}
		}
		if (modoSeleccionado == VS){
			if (personajes.seleccionados == NO)
				personajes = personajes = vista->elegirPersonajes(unControlador, otroControlador, personajes, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
			else{
				modo->setPersonajes(personajes);
				Parser::getInstancia().getPelea()->getPersonaje1()->setNombreActual(nombreP1);
				Parser::getInstancia().getPelea()->getPersonaje2()->setNombreActual(nombreP2);
				break;
			}
			
		}
		if (modoSeleccionado == PRACTICA){
			if (personajes.seleccionados == NO)
				personajes = vista->elegirPersonajes(unControlador, personajes, controladorDeMouse->getEventoDeMouse(), nombreP1, nombreP2);
			else{
			modo->setPersonajes(personajes);
			Parser::getInstancia().getPelea()->getPersonaje1()->setNombreActual(nombreP1);
			Parser::getInstancia().getPelea()->getPersonaje2()->setNombreActual(nombreP2);
			break;
			}
		}

		if ((modoSeleccionado == NINGUNO)){
			if (MODO_RAPIDO){
				personajes.seleccionados = SI;
				modoSeleccionado = VS;
			}
			else
				modoSeleccionado = vista->actualizarMenu(unControlador, controladorDeMouse->getEventoDeMouse());
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
