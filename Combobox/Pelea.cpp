#include "stdafx.h"
#include "Pelea.h"


Pelea::Pelea()
{
	rounds = std::vector<Round*>();
	personaje1 = nullptr;
	personaje2 = nullptr;
	reloj = new Timer();
	peleaTerminada = false;
	cantidadDeRounds = ROUNDS_MAXIMOS;
	tiempoDelRound = TIEMPO_DE_PELEA;
}


void Pelea::setTiempoDelRound(Uint32 unTiempo){
	tiempoDelRound = unTiempo;
}


Uint32 Pelea::getTiempoDelRound(){
	return tiempoDelRound;
}


void Pelea::setCantidadDeRounds(int unaCantidad){
	cantidadDeRounds = unaCantidad;
}


void Pelea::setPersonaje1(Personaje* unPersonaje){
	personaje1 = unPersonaje;
}


void Pelea::setPersonaje2(Personaje* unPersonaje){
	personaje2 = unPersonaje;
}


Personaje* Pelea::getPersonaje1(){
	return personaje1;
}


Personaje* Pelea::getPersonaje2(){
	return personaje2;
}


void Pelea::personajeGanoElRound(Personaje* unPersonaje){
	reloj->stop();
	if (rounds.size() <= cantidadDeRounds){
		Round* ultimoRound = rounds.back();
		ultimoRound->setPersonajeGanador(unPersonaje);
	}
	else peleaTerminada = true;
}


Personaje* Pelea::getPersonajeGanador(){
	int roundsGanadosP1 = 0;
	int roundsGanadosP2 = 0;
	Personaje* unPersonaje;
	int roundsMinimos = ((cantidadDeRounds / 2) + 1);
	if (rounds.size() >= roundsMinimos){
		for (size_t i = 0; i < rounds.size(); i++){
			unPersonaje = rounds.at(i)->getPersonajeGanador();
			if (unPersonaje != nullptr){
				if (unPersonaje == personaje1) roundsGanadosP1++;
				if (unPersonaje == personaje2) roundsGanadosP2++;
			}
		}
		if (roundsGanadosP1 > roundsGanadosP2) return personaje1;
		if (roundsGanadosP2 > roundsGanadosP1) return personaje2;
	}
	return nullptr;
}


bool Pelea::terminoLaPelea(){
	int roundsGanadosP1 = 0;
	int roundsGanadosP2 = 0;
	Personaje* unPersonaje;
	int roundsMinimos = ((cantidadDeRounds / 2) + 1);
	if (peleaTerminada) return true;
	if (rounds.size() >= roundsMinimos){
		for (size_t i = 0; i < rounds.size(); i++){
			unPersonaje = rounds.at(i)->getPersonajeGanador();
			if (unPersonaje != nullptr){
				if (unPersonaje == personaje1) roundsGanadosP1++;
				if (unPersonaje == personaje2) roundsGanadosP2++;
			}
		}
		if (roundsGanadosP1 != roundsGanadosP2){
			if (roundsGanadosP1 >= roundsMinimos) return true;
			if (roundsGanadosP2 >= roundsMinimos) return true;
		}
	}
	return false;
}


void Pelea::terminarRound(){
	reloj->stop();
}


void Pelea::empezarRound(){
	if (rounds.size() < cantidadDeRounds){
		rounds.push_back(new Round(rounds.back()->getNumeroDeRound() + 1));
	}
	else peleaTerminada = true;
	reloj->start();
}


void Pelea::iniciarPelea(){
	reloj->start();
	rounds.push_back(new Round(1));
	peleaTerminada = false;
}


void Pelea::terminarPelea(){
	for (size_t i = 0; i < rounds.size(); i++){
		delete rounds.at(i);
	}
	peleaTerminada = true;
	rounds.clear();
	reloj->stop();
}


Uint32 Pelea::getSegundosTranscurridosDelRound(){
	if (tiempoDelRound == TIEMPO_INFINITO || reloj->isPaused() ) return 0;
	return (reloj->getTicks() / 1000);
}


Round* Pelea::getRoundActual(){
	return rounds.back();
}


std::vector<Round*> Pelea::getRounds(){
	return rounds;
}


Pelea::~Pelea()
{
	personaje1 = nullptr;
	personaje2 = nullptr;
	for (size_t i = 0; i < rounds.size(); i++){
		delete rounds.at(i);
	}
	rounds.clear();
	delete reloj;
	peleaTerminada = false;
}
