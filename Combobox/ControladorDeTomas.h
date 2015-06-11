#pragma once
#include "Toma.h"
#include "Movimiento.h"
#include "Timer.h"

class ControladorDeTomas
{
public:
	ControladorDeTomas();
	Toma* getTomaSegunOrientacion(bool estaInvertido);
	void setTimer(Timer* unTimer);
	void setToma(Toma* unaToma);
	void setMovimientos(std::vector<Movimiento*>* unosMovimientosActivos);
	std::vector<Toma*>* getTomas();
	~ControladorDeTomas();

private:
	std::vector<Toma*> tomas;
	std::vector<Movimiento*>* movimientosActivos;
	int ultimaCantidadDeMovimientos;
	Timer* timer;

	bool hayToma(Toma* unaToma, bool estaInvertido);
};

