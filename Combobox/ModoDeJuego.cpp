#include "stdafx.h"
#include "ModoDeJuego.h"


ModoDeJuego::ModoDeJuego()
{
}


Controlador* ModoDeJuego::getControlador1(){
	return controlador1;
}


void ModoDeJuego::iniciar(){

}


bool ModoDeJuego::esModoPractica(){
	return false;
}


Controlador* ModoDeJuego::getControlador2(){
	return controlador2;
}

Personaje* ModoDeJuego::getPersonaje1(){
	return personaje1;
}

Personaje* ModoDeJuego::getPersonaje2(){
	return personaje2;
}

ModoDeJuego::~ModoDeJuego()
{
}
