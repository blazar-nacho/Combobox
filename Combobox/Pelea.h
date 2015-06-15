#pragma once
#include "Round.h"

class Pelea
{
public:
	Pelea();
	void setPersonaje1(Personaje* unPersonaje);
	void setPersonaje2(Personaje* unPersonaje);
	void setCantidadDeRounds(int unaCantidad);
	void setTiempoDelRound(Uint32 unTiempo);
	Uint32 getTiempoDelRound();
	void personajeGanoElRound(Personaje* unPersonaje);
	float getSegundosTranscurridosDelRoundFloat();
	void iniciarPelea();
	void terminarPelea();
	void terminarRound();
	void empezarRound();
	Uint32 getSegundosTranscurridosDelRound();
	bool terminoLaPelea();
	Personaje* getPersonajeGanador();
	Round* getRoundActual();
	std::vector<Round*> getRounds();
	
	Personaje* getPersonaje1();
	Personaje* getPersonaje2();
	~Pelea();

private:
	Personaje* personaje1;
	Personaje* personaje2;
	std::vector<Round*> rounds;
	unsigned int cantidadDeRounds;
	Uint32 tiempoDelRound;
	Timer* reloj;
	bool peleaTerminada;
};

