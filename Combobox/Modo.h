#pragma once
#include "Modo1vs1.h"
#include "Modo1vsCom.h"
#include "ModoPractica.h"
#include "Vista.h"

class Modo
{
public:
	Modo(Vista* unaVista);
	void seleccionarModo(MODOS_DE_JUEGO modoActual, int unNivel);
	Vista* getRefVista();
	MODOS_DE_JUEGO getModoActual();
	Controlador* getControlador1();
	Controlador* getControlador2();
	void setPersonajes(PJSELECCION personajes);
	bool esModoPractica();
	void iniciarModo();
	void Modo::cambiarColorPersonajes();
	~Modo();

private:
	Vista* laVista;
	std::vector<ModoDeJuego*> modosDeJuego;
	Personaje* personaje1Seleccionado;
	Personaje* personaje2Seleccionado;
	MODOS_DE_JUEGO modoActual;
};

