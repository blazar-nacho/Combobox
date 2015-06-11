#pragma once
#include "IA.h"

class ControladorAutomatico: public Controlador
{
public:
	ControladorAutomatico();
	int cambiar();
	ControladorDeTomas* getControladorDeTomas();
	std::vector<Movimiento*> getMovimientos();
	void setPersonajeRival(Personaje* unPersonaje);
	void setNivel(int unNivel);
	~ControladorAutomatico();

private:
	std::vector<Movimiento*> movimientos;
	ControladorDeTomas* controladorDeTomas;
	IA* iA;
};

