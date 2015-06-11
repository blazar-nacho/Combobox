#include "stdafx.h"
#include "ControladorAutomatico.h"


ControladorAutomatico::ControladorAutomatico()
{
	movimientos = std::vector<Movimiento*>();
	movimientos.push_back(new Movimiento());
	iA = new IA();
	controladorDeTomas = new ControladorDeTomas();
}

int ControladorAutomatico::cambiar(){
	if (!(iA->tienePersonajeRival())) iA->setPersonajeRival(Parser::getInstancia().getPelea()->getPersonaje1());
	if (!(iA->tienePersonajeControlado())) iA->setPersonajeControlado(Parser::getInstancia().getPelea()->getPersonaje2());
	Movimiento* unMovimiento = new Movimiento();
	unMovimiento->setMovimiento(iA->getMovimiento());
	movimientos.push_back(unMovimiento);
	return CONTINUAR;
}


void ControladorAutomatico::setNivel(int unNivel){
	iA->setNivel((IA::NIVEL)unNivel);
}


ControladorDeTomas* ControladorAutomatico::getControladorDeTomas(){
	return controladorDeTomas;
}

std::vector<Movimiento*>  ControladorAutomatico::getMovimientos(){
	return movimientos;
}

void ControladorAutomatico::setPersonajeRival(Personaje* unPersonaje){
	iA->setPersonajeRival(unPersonaje);
}

ControladorAutomatico::~ControladorAutomatico()
{
	for (size_t i = 0; i < movimientos.size(); i++){
		delete movimientos.at(i);
	}
	movimientos.clear();

	delete iA;
	delete controladorDeTomas;
}
