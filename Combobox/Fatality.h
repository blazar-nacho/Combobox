#pragma once
#include "Mundo.h"

#define DIST 4.0f
#define RANGO_DIST 5.0f
#define RETRASO_SPRT 6

// declaro para poder usar 
class Cuerpo;

class Fatality {
public:
	Fatality(Personaje* jugadorGanador, Cuerpo* cuerpoGanador, SDL_Texture* texturaGanadorNueva, Personaje* jugadorPerdedor, Cuerpo* cuerpoPerdedor, SDL_Texture* texturaPerdedorNueva, SDL_Renderer* rendererSDL, SDL_Rect cuadroGanadorNuevo, SDL_Rect cuadroPerdedorNuevo, Mundo* refMundoNueva, std::vector<double> colorGanador);

	// Básicamente reproduce secuencias de sprites específicos de fatality
	void realizar();

	// devuelve true si termino de hacer la fatality
	bool finalizo();

	// devuelve la textura del ganador para reestablecer la vista
	SDL_Texture* getTexturaGanador();

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
	bool fatalityFinalizada;

	std::vector<SDL_Rect*> *fatalityGanador;
	int cuadroActualGanador;
	SDL_Rect cuadroGanador;
	std::vector<SDL_Rect*> *fatalityPerdedor;
	int cuadroActualPerdedor;
	SDL_Rect cuadroPerdedor;
	int retraso;

	std::vector<SDL_Rect*> *Caminar;
	int cuadroActualCaminar;
	bool distanciaCorrecta;

	SDL_Texture* texturaSDL;
	SDL_Renderer* renderer;

	SDL_Texture* texturaGanador;
	SDL_Texture* texturaPerdedor;

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