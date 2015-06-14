#pragma once
#include "Cuerpo.h"

#define RANGO_DIST 5.0f
#define RETRASO_SPRT 6

class Fatality {
public:	
	Fatality(Personaje* jugadorGanador, Cuerpo* cuerpoGanadorNuevo, Personaje* jugadorPerdedor, Cuerpo* cuerpoPerdedorNuevo, SDL_Renderer* rendererSDL, SDL_Rect cuadroGanadorNuevo, SDL_Rect cuadroPerdedorNuevo, std::vector<double> colorGanador);

	// Básicamente reproduce secuencias de sprites específicos de fatality
	void realizar();

	std::string getImagenDir() const;

	bool distanciaEstaCorrecta();

	~Fatality();

private:
	Personaje* jugadorGanador;
	Cuerpo* cuerpoGanador;
	Personaje* jugadorPerdedor;
	Cuerpo* cuerpoPerdedor;

	std::string imagenDir;
	float distancia;

	std::vector<SDL_Rect*> *fatalityGanador;
	int cuadroActualGanador;
	SDL_Rect cuadroGanador;
	std::vector<SDL_Rect*> *fatalityPerdedor;
	int cuadroActualPerdedor;
	SDL_Rect cuadroPerdedor;
	int retraso;
		
	bool distanciaCorrecta;

	// textura del spritesheet con las fatalities
	SDL_Texture* texturaSDL;
	SDL_Renderer* renderer;

	float distanciaMin;
	float distanciaMax;
	int delayPerdedor;

	float xJugGanador;
	float xJugPerdedor;

	void parsearFatality();

	Json::Value	ParsearRaizJson(std::string fatalityJsonDir);
	SDL_Rect* crearCuadro(Json::Value cuadro);
	void cargarTextura(std::vector<double> colorGanador);

	// mueve de a un paso al jugador ganador hasta la posicion de fatality	
	void ubicarGanador();
};