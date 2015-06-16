#pragma once
#include "Mundo.h"
#include "Cuerpo.h"

#define DIST 4.0f
#define RANGO_DIST 2.5f
#define RETRASO_SPRT 6
#define CONTADOR_INI 200


class Fatality {
public:
	Fatality(Personaje* jugadorGanador, Cuerpo* cuerpoGanador, SDL_Texture* texturaGanadorNueva, Personaje* jugadorPerdedor, Cuerpo* cuerpoPerdedor, SDL_Texture* texturaPerdedorNueva, SDL_Renderer* rendererSDL, Mundo* refMundoNueva, std::vector<double> colorGanador);

	// Básicamente reproduce secuencias de sprites específicos de fatality
	void realizar(SDL_Rect* cuadroGanador, SDL_Rect* cuadroPerdedor);

	// devuelve true si termino de hacer la fatality
	bool finalizo();

	// devuelve la textura del ganador para reestablecer la vista
	SDL_Texture* getTexturaGanador();

	SDL_Texture* getTexturaPerdedor();
	std::string getImagenDir() const;

	~Fatality();

private:
	Personaje* jugadorGanador;
	Cuerpo* cuerpoGanador;
	Personaje* jugadorPerdedor;
	Cuerpo* cuerpoPerdedor;
	std::string imagenDir;
	float distancia;
	Mundo* refMundo;
	// contador de tiempo de fatality
	int contador;
	bool fatalityFinalizada;

	std::vector<SDL_Rect*> *fatalityGanador;
	int cuadroActualGanador;
	SDL_Rect* cuadroGanador;
	std::vector<SDL_Rect*> *fatalityPerdedor;
	int cuadroActualPerdedor;
	SDL_Rect* cuadroPerdedor;
	int retraso;

	std::vector<SDL_Rect*> *Caminar;
	std::vector<SDL_Rect*> *CaminarAtras;
	int cuadroActualCaminar;
	bool distanciaCorrecta;

	SDL_Texture* texturaSDL;
	SDL_Renderer* renderer;

	SDL_Texture* texturaGanador;
	SDL_Texture* texturaPerdedor;
	bool texturaPerdedorBloqueada;

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