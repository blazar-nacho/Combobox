#include "stdafx.h"
#include "ValidadorDePelea.h"
#include <ctype.h>


ValidadorDePelea::ValidadorDePelea()
{
	laPelea = nullptr;
}


void ValidadorDePelea::validarPeleaDesdeParaLosPeronajes(Json::Value unaPelea, std::vector<Personaje*>* personajes){
	int rounds;
	int tiempo;
	laPelea = new Pelea();

	if (!unaPelea)
	{
		Log::getInstancia().logearMensajeEnModo("[BAD] Fallo el parseo de la pelea", Log::MODO_WARNING);
		Log::getInstancia().logearMensajeEnModo("Se cargan los valores para la pelea por defecto", Log::MODO_WARNING);
		return;
	}

	else{
		//ROUNDS
		if (unaPelea.isMember("rounds") && (unaPelea.get("rounds", ROUNDS_MAXIMOS).isNumeric())){
			rounds = unaPelea.get("fighters", ROUNDS_MAXIMOS).asInt();
			if ((rounds > 0) && (rounds < 9999)){
				laPelea->setCantidadDeRounds(rounds);
				Log::getInstancia().logearMensajeEnModo("Se cargaron los rounds correctamente", Log::MODO_DEBUG);
			}
			else {
				Log::getInstancia().logearMensajeEnModo("[BAD] Fallo el parseo de los rounds", Log::MODO_WARNING);
				Log::getInstancia().logearMensajeEnModo("Se cargan rounds por defecto", Log::MODO_WARNING);
			}
		}
		else{
			Log::getInstancia().logearMensajeEnModo("[BAD] Fallo el parseo de los rounds", Log::MODO_WARNING);
			Log::getInstancia().logearMensajeEnModo("Se cargan rounds por defecto", Log::MODO_WARNING);
		}

		//TIEMPO
		if (unaPelea.isMember("tiempo") && (unaPelea.get("tiempo", TIEMPO_DE_PELEA).isNumeric())){
			tiempo = unaPelea.get("tiempo", TIEMPO_DE_PELEA).asInt();
			if (tiempo > 0) {
				if (tiempo >= 999) tiempo = TIEMPO_INFINITO;
				laPelea->setTiempoDelRound((Uint32)tiempo);
				Log::getInstancia().logearMensajeEnModo("Se cargo el tiempo del round correctamente", Log::MODO_DEBUG);
			}
			else {
				Log::getInstancia().logearMensajeEnModo("[BAD] Fallo el parseo del tiempo del round", Log::MODO_WARNING);
				Log::getInstancia().logearMensajeEnModo("Se cargan tiempo del round por defecto", Log::MODO_WARNING);
			}
		}
		else {
			Log::getInstancia().logearMensajeEnModo("[BAD] Fallo el parseo del tiempo del round", Log::MODO_WARNING);
			Log::getInstancia().logearMensajeEnModo("Se cargan tiempo del round por defecto", Log::MODO_WARNING);
		}
	}
	Log::getInstancia().logearMensajeEnModo("Se cargo el combate correctamente", Log::MODO_DEBUG);
}


std::string ValidadorDePelea::getPeleaComoString(){
	return pelea;
}


Pelea* ValidadorDePelea::getPelea(){
	return laPelea;
}


ValidadorDePelea::~ValidadorDePelea()
{
	pelea.clear();
	if (laPelea != nullptr) delete laPelea;
}
