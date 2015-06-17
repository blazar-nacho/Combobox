#pragma once
#include "Constantes.h"
#include "Modo.h"
#include "Vista.h"
#include "ControladorDeMouse.h"

class Menu
{
public:
	Menu(Modo* unModo);
	int seleccionarModo(Controlador* unControlador, Controlador* otroControlador);
	Modo* getModo();
	MODOS_DE_JUEGO getModoSeleccionado();
	void reiniciarMenu();
	~Menu();
private:
	Modo* modo;
	Vista* vista;
	Mundo* mundo;
	MODOS_DE_JUEGO modoSeleccionado;
	int torre;
	int personaje1,personaje2;
	std::string nombreP1;
	std::string nombreP2;
	PJSELECCION personajes;
	ControladorDeMouse* controladorDeMouse;
};

