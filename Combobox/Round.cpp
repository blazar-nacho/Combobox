#include "stdafx.h"
#include "Round.h"


Round::Round(int unNumeroDeRound)
{
	numeroDeRound = unNumeroDeRound;
	personajeGanador = nullptr;
}


int Round::getNumeroDeRound(){
	return numeroDeRound;
}


Personaje* Round::getPersonajeGanador(){
	return personajeGanador;
}


void Round::setPersonajeGanador(Personaje* unPersonajeGanador){
	personajeGanador = unPersonajeGanador;
}


Round::~Round()
{
	personajeGanador = nullptr;
}
