#include "stdafx.h"
#include "ControladorDeTomas.h"


ControladorDeTomas::ControladorDeTomas()
{
	tomas = std::vector<Toma*>();
	ultimaCantidadDeMovimientos = 0;
}


void ControladorDeTomas::setToma(Toma* unaToma){
	tomas.push_back(unaToma);
}


void ControladorDeTomas::setMovimientos(std::vector<Movimiento*>* unosMovimientosActivos){
	movimientosActivos = unosMovimientosActivos;
}


bool ControladorDeTomas::hayToma(Toma* unaToma, bool estaInvertido){
	int error = 0;
	int aciertos = 0;
	int tiempo;
	int tiempoMaximo;
	int j = unaToma->getMovimientos()->size() - 1;
	Movimiento* otroMovimiento;
	Movimiento* unMovimiento;

	if (!movimientosActivos->empty()) tiempo = movimientosActivos->at(movimientosActivos->size() - 1)->getTiempo();
	else return false;
	if (ultimaCantidadDeMovimientos == movimientosActivos->size()) return false;
	if ((timer->getTicks() - tiempo) > 1000) return false;

	ultimaCantidadDeMovimientos++;
	tiempoMaximo = unaToma->getTiempo();

	if (movimientosActivos->size() >= (unaToma->getMovimientos()->size())){
		for (int i = movimientosActivos->size() - 1; i >= 0; i--){
			otroMovimiento = movimientosActivos->at(i);
			unMovimiento = new Movimiento();
			unMovimiento->setMovimiento(otroMovimiento->getMovimiento());
			unMovimiento->setTiempo(otroMovimiento->getTiempo());
			
			if (error > unaToma->getError()) return false;
			if (aciertos == unaToma->getMovimientos()->size()){
				tiempo -= unMovimiento->getTiempo();
				movimientosActivos->clear();
				if (tiempo >= 0 && tiempo <= tiempoMaximo){
					return true;
				}
				ultimaCantidadDeMovimientos = 0;
				return false;
			}

			if (estaInvertido){
				if (otroMovimiento->getMovimiento() == DER) unMovimiento->setMovimiento(IZQ);
				if (otroMovimiento->getMovimiento() == IZQ) unMovimiento->setMovimiento(DER);
				if (otroMovimiento->getMovimiento() == SALTODER) unMovimiento->setMovimiento(SALTOIZQ);
				if (otroMovimiento->getMovimiento() == SALTOIZQ) unMovimiento->setMovimiento(SALTODER);
			}

			if (unMovimiento->getMovimiento() != unaToma->getMovimientos()->at(j)){
				if (i == (movimientosActivos->size() - 1)) return false;
				error++;
				aciertos++;
			}
			else{
				j--;
				aciertos++;
			}
			delete unMovimiento;
		}

		if (error > unaToma->getError()) return false;
		if (aciertos == unaToma->getMovimientos()->size()){
			tiempo -= movimientosActivos->at(0)->getTiempo();
			movimientosActivos->clear();
			if (tiempo >= 0 && tiempo <= tiempoMaximo){
				return true;
			}
			ultimaCantidadDeMovimientos = 0;
			return false;
		}
	}
	return false;
}


void ControladorDeTomas::setTimer(Timer* unTimer){
	timer = unTimer;
}


Toma* ControladorDeTomas::getTomaSegunOrientacion(bool estaInvertido){
	for (size_t i = 0; i < tomas.size(); i++){
		if (hayToma(tomas.at(i), estaInvertido)) return tomas.at(i);
		ultimaCantidadDeMovimientos = 0;
	}
	return nullptr;
}


std::vector<Toma*>* ControladorDeTomas::getTomas(){
	return &tomas;
}


ControladorDeTomas::~ControladorDeTomas()
{
	for (size_t i = 0; i < tomas.size(); i++){
		delete tomas.at(i);
	}
	tomas.clear();
}
