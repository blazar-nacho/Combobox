#include "stdafx.h"
#include "ModoPractica.h"


ModoPractica::ModoPractica()
{
	controlador1 = Parser::getInstancia().getControlador1();
	controlador2 = new Controlador();
}


bool ModoPractica::esModoPractica(){
	return true;
}


void ModoPractica::iniciar(){
	Parser::getInstancia().getPelea()->getPersonaje2()->setVida(VIDA_INFINITA);
	Parser::getInstancia().getPelea()->setTiempoDelRound(TIEMPO_INFINITO);
}


ModoPractica::~ModoPractica()
{
	controlador1 = nullptr;
	controlador2 = nullptr;
}
