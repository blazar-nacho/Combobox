#include "stdafx.h"
#include "Movimiento.h"


Movimiento::Movimiento()
{
	movimiento = QUIETO;
	tiempo = 0;
}


void Movimiento::setMovimiento(MOV_TIPO unMovimiento){
	movimiento = unMovimiento;
}


void Movimiento::setTiempo(Uint32 unTiempo){
	tiempo = unTiempo;
}


MOV_TIPO Movimiento::getMovimiento(){
	return movimiento;
}


Uint32 Movimiento::getTiempo(){
	return tiempo;
}

Movimiento::~Movimiento()
{
}
