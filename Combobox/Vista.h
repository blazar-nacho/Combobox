#pragma once
#include "stdafx.h"
#include "Mundo.h"
#include "Controlador.h"
#include <list>
#include "Sprites.h"
#include "Timer.h"
#include "EventoDeMouse.h"
#include "Fatality.h"

class Vista
{
public:	
	Vista(Mundo* unMundo, bool* error, bool habilitarAceleracionDeHardware);
	void crearPersonajes();
	void actualizar();
	MODOS_DE_JUEGO actualizarMenu(Controlador* unControlador, EventoDeMouse* unEventoDeMouse);
	int Vista::elegirDificultad(Controlador* unControlador, EventoDeMouse *unEventoDeMouse);
	PJSELECCION Vista::elegirPersonajes(Controlador* unControlador, PJSELECCION personajes, EventoDeMouse *unEventoDeMouse, std::string &nombreDelP1, std::string &nombreDelP2);
	PJSELECCION Vista::elegirPersonajes(Controlador* unControlador, Controlador* otroControlador, PJSELECCION personajes, EventoDeMouse *unEventoDeMouse, std::string &nombreDelP1, std::string &nombreDelP2);
	PJSELECCION Vista::traducirSeleccion(int opcion1, int opcion2, PJSELECCION personajes);
	void Vista::setColorPj1(std::vector<double> unColor);
	void Vista::setColorPj2(std::vector<double> unColor);
	SDL_Rect Vista::ajusteResolucionBase800x600(int x, int y, int anchoPx, int altoPx);
	bool Vista::cambiaColorPersonaje();
	std::string Vista::entradaTexto(SDL_Rect rectanguloDestino, std::string textoPersonaje);
	void Vista::resetContadorLogoPelea();
	void setModoActual(MODOS_DE_JUEGO modoActual);
	void Vista::FrenarSprites(Personaje* unPersonaje);
	void Vista::LiberarSprites(Personaje* unPersonaje);
	void Vista::Invisible(Personaje* unPersonaje);
	void Vista::Visible(Personaje* unPersonaje);

	void habilitarVibracion();
	void deshabilitarVibracion();
	void alfa(Uint8 alfa);
	SDL_Surface* cargarSuperficieOptimizada(std::string dirImagen);
	void Vista::cargarTexto(std::string textureText, SDL_Color textColor);
	void dibujarTexto(SDL_Rect cuadradoDestino, Uint8 Alfa);
	void reiniciarCamara();
	void reiniciarMenu();
	~Vista();

private:
	unsigned int mantenerElColor;
	int vibraciones;
	int retraso;
	bool vibracion;
	bool estaVibrando;
	int opcion;
	int opcionTorre;
	int opcionPj1;
	int opcionPj2;
	int opcionPersonaje;
	bool opcion1Seleccionada;
	bool opcion2Seleccionada;

	bool frenarsprite1;
	bool frenarsprite2;
	bool Invisible1;
	bool Invisible2;
	// hInicial, hFinal, desplazamiento
	std::vector<double> colorPj1;
	std::vector<double> colorPj2;
	bool cambiarColor;
	int anchoTexto, altoTexto;
	int contadorLogoPelea;

	SDL_Window* ventana;
	SDL_Renderer* renderer;
	std::string rutaImagen;	
	SDL_Texture* texturaSpriteUno;
	SDL_Texture* texturaSpriteDos;
	Mundo* refMundo;
	MODOS_DE_JUEGO modoActual;

	int tiempoSecuenciaSpritesDos;
	int tiempoSecuenciaSpritesUno;
	SDL_Texture *texturaVerde;
	SDL_Texture *texturaBarraDeVida;
	SDL_Texture *texturaBarraDeVidaRoja;
	SDL_Texture *texturaLogoPelea;
	SDL_Texture *texturaRonda;
	SDL_Texture *texturaFatality;
	SDL_Texture *texturaGanador;
	SDL_Texture *texturaFinishHim;

	SDL_Texture *texturaTexto;
	SDL_Texture* texturaFondoTorres;
	SDL_Texture *texturabebebubu;
	SDL_Texture *texturaMedio;
	SDL_Texture *texturaExperimentado;
	SDL_Texture *texturaExperto;
	SDL_Texture *texturaSeleccionPersonajes;
	SDL_Texture *texturaSeleccionPj1;
	SDL_Texture *texturaSeleccionPj2;
	SDL_Texture *personajesMuestra;

	SDL_Surface* SuperficieUno;
	SDL_Surface* SuperficieDos;

	Uint8 AlfaVida;
	Uint8 AlfaAnterior;
	Uint8 AlfaInicial;

	SDL_Surface* superficieBarraDeVida;
	SDL_Surface* superficieBarraDeVidaRoja;
	SDL_Surface* supTexto;

	SDL_Rect barraDeVida1;
	SDL_Rect barraDeVida2;
	SDL_Rect barraDeVidaImagen1;
	SDL_Rect barraDeVidaImagen2;
	SDL_Rect muestra1;
	SDL_Rect muestra2;

	SDL_Rect barraRojaDeVida1;
	SDL_Rect barraRojaDeVida2;
	SDL_Rect barraRojaDeVidaImagen1;
	SDL_Rect barraRojaDeVidaImagen2;

	SDL_Surface* modoJuego;
	SDL_Texture *juegoModo;
	std::string texto;

	SDL_Rect rectanguloVentana;
	SDL_Rect rectDificultad;

	SDL_Rect texto1elegido;
	SDL_Rect texto2elegido;
	SDL_Rect ima1elegida;
	SDL_Rect ima2elegida;
	SDL_Rect imaColor1elegida;
	SDL_Rect imaColor2elegida;

	std::string textoIngresado;
	std::string textoIngresado2;
	std::string nombreTextoPj1;
	std::string nombreTextoPj2;
	SDL_Rect cuadradoRedimension;
	SDL_Rect cuadradoRedimension2;
	SDL_Rect cuadradoRedimension3;
	SDL_Rect cuadradoRedimension4;
	SDL_Rect cuadradoRedimension5;
	SDL_Rect cuadradoRedimension6;
	SDL_Rect cuadradoRedimension7;
	SDL_Rect cuadradoRedimension8;

	std::string textoCombos;

	Timer finishhimTimer;
	Timer menuTimer;
	TTF_Font* fuente = NULL;
	int posBarraDeVida1;
	int anchoImagenBarraDeVida;
	int anchoBarraDeVida1;
	int anchoBarraDeVida2;	

	//Anchos Auxiliares
	int anchoAnteriorBarra1;
	int anchoAnteriorBarra2;
	int anchoAnteriorBarraImagen1;
	int anchoAnteriorBarraImagen2;

	//Anchos finales
	int anchoRojoBarra1;
	int anchoRojoBarra2;
	int anchoRojoImagenBarra1;
	int anchoRojoImagenBarra2;
	
	//contadores efectos
	int finishHim;

	std::vector<Capa*> capasVista;
	int numeroDeCuadroUno;
	std::vector<SDL_Rect*>* listaDeCuadrosUno;
	int numeroDeCuadroDos;
	std::vector<SDL_Rect*>* listaDeCuadrosDos;
	//std::vector<Capa*> capasDeTextura;
	float camaraXLog;
	float xPjUno;
	float xPjDos;
	ManejadorULogicas manejadorULog;
	bool fatalityCreada = false;
	Fatality* fatality;
	// La fatality va a dibujar los sprites que hagan falta y setear estado invisible al personaje cuando haga falta
	void RealizarFatality(std::vector<Personaje*>* personajesVista, SDL_Rect* personajeUno, SDL_Rect* personajeDos);

	ESTADO estadoAnteriorPj1;
	ESTADO estadoAnteriorPj2;
	void OrdenarCapas();
	std::string GetEstadoDelPersonaje(ESTADO , Personaje* );
	void Dibujar(std::vector<Personaje*>);

	void DibujarBarrasDeVida(std::vector<Personaje*>);
	void DibujarCapasAnteriores(std::vector<Personaje*> personajes, float anchoVentana, int anchoVentanaPx, int altoVentanaPx, float anchoEscenario);
	void DibujarCapasPosteriores(std::vector<Personaje*> personajes, float anchoVentana, int anchoVentanaPx, int altoVentanaPx, float anchoEscenario);
	void DibujarPersonajes(std::vector<Personaje*> personajes);
	void DibujarInput(Controlador* unControlador);
	void DibujarReloj();
	void DibujarRondaPelea();
	void DibujarFinishHim();
	void DibujarFatality();
	void DibujarGanador(int resultado);
	void DibujarNombres();
};

