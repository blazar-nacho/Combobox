#include "stdafx.h"
#include "IA.h"


IA::IA()
{
	personajeRival = nullptr;
	personajeControlado = nullptr;
	umbralDeProbabilidad = 5;
	cantidadDeMovimientosAdelante = 0;
	cantidadDeMovimientosAtras = 0;
}


void IA::setNivel(NIVEL unNivel){
	if (unNivel == BAJO) umbralDeProbabilidad = 2;
	if (unNivel == DIFICIL) umbralDeProbabilidad = 8;
	if (unNivel == EXPERTO) umbralDeProbabilidad = 10;
}


bool IA::tienePersonajeRival(){
	if (!personajeRival) return false;
	return true;
}



bool IA::tienePersonajeControlado(){
	if (!personajeControlado) return false;
	return true;
}


MOV_TIPO IA::getMovimiento(){
	int probabilidadDeHacerLoCorrecto = rand() % 10;
	int probabilidadDeMovimiento;
	MOV_TIPO adelante;
	MOV_TIPO atras;
	MOV_TIPO saltoAdelante;
	MOV_TIPO saltoAtras;
	MOV_TIPO movimiento;
	ESTADO unEstado = personajeRival->getEstado();
	bool posicionDeGolpeo = false;
	float posicion1 = personajeRival->getPosicionUn().first + personajeRival->getAncho() / 2;
	float posicion2 = personajeControlado->getPosicionUn().first + personajeControlado->getAncho() / 2;
	float posicionEntreAmbos = posicion1 - posicion2;
	if (posicionEntreAmbos < 0) posicionEntreAmbos = posicionEntreAmbos * -1;

	if (posicionEntreAmbos >= 0 && posicionEntreAmbos <= (personajeControlado->getAncho() / 2)) {
		posicionDeGolpeo = true;
	}

	if (personajeRival->getOrientacion() == "DER"){
		adelante = IZQ;
		atras = DER;
		saltoAdelante = SALTOIZQ;
		saltoAtras = SALTODER;
	}
	if (personajeRival->getOrientacion() == "IZQ"){
		adelante = DER;
		atras = IZQ;
		saltoAdelante = SALTODER;
		saltoAtras = SALTOIZQ;
	}

	//MOVIMIENTOS ADELANTE
	if (cantidadDeMovimientosAdelante != 0){
		cantidadDeMovimientosAdelante--;
		return adelante;
	}

	//MOVIMIENTOS ADELANTE
	if (cantidadDeMovimientosAtras != 0){
		cantidadDeMovimientosAtras--;
		return atras;
	}

	cantidadDeMovimientosAdelante = 0;
	cantidadDeMovimientosAtras = 0;

	if (probabilidadDeHacerLoCorrecto < umbralDeProbabilidad){
		//BASICOS
		if (personajeControlado->getEstado().golpeado == GOLPEADO){
			probabilidadDeMovimiento = (rand() % 48);
			if (probabilidadDeMovimiento <= 2) return saltoAdelante;
			if (probabilidadDeMovimiento > 2 && probabilidadDeMovimiento <= 4) return saltoAtras;
			if (probabilidadDeMovimiento > 4 && probabilidadDeMovimiento <= 6) return ARRIBA;
			if (probabilidadDeMovimiento > 6 && probabilidadDeMovimiento <= 8) return ABAJO;
			if (probabilidadDeMovimiento > 8) return DEFENSA;
		}
		if (unEstado.movimiento == GUARDIA){
			probabilidadDeMovimiento = (rand() % 50);
			if (probabilidadDeMovimiento == 0) return saltoAdelante;
			if (probabilidadDeMovimiento == 1) return saltoAtras;
			if (probabilidadDeMovimiento > 1 && probabilidadDeMovimiento < 40){
				cantidadDeMovimientosAdelante = 10;
				return adelante;
			}
			if (probabilidadDeMovimiento >= 40){
				cantidadDeMovimientosAtras = 10;
				return atras;
			}
		}
		if (unEstado.movimiento == ARMA_ARROJABLE || unEstado.movimiento == BICICLETA){
			probabilidadDeMovimiento = (rand() % 50);
			if (probabilidadDeMovimiento < 5) return DEFENSA;
			if (probabilidadDeMovimiento >= 5 && probabilidadDeMovimiento < 10) return DEFENSA_AGACHADO;
			if (probabilidadDeMovimiento >= 10 && probabilidadDeMovimiento < 47)  return ABAJO;
			if (probabilidadDeMovimiento == 47) return ARRIBA;
			if (probabilidadDeMovimiento == 48) return saltoAdelante;
			if (probabilidadDeMovimiento == 49) return saltoAtras;
		}

		//SI ESTAN LEJOS
		if (!posicionDeGolpeo){
			probabilidadDeMovimiento = (rand() % 46);
			if (probabilidadDeMovimiento <= 40){
				cantidadDeMovimientosAdelante = 10;
				return adelante;
			}
			if (probabilidadDeMovimiento == 41) return saltoAdelante;
			if (probabilidadDeMovimiento > 41) return ARMA;
		}

		//SI ESTAN AL LADO
		else{
			if (unEstado.accion == PARADO){
				probabilidadDeMovimiento = (rand() % 8);
				if (probabilidadDeMovimiento == 0) return G_ABAJO;
				if (probabilidadDeMovimiento == 1) return G_ALTO;
				if (probabilidadDeMovimiento == 2) return P_ALTA;
				if (probabilidadDeMovimiento == 3) return P_BAJA;
				if (probabilidadDeMovimiento == 4) return G_ABAJO;
				if (probabilidadDeMovimiento == 5) return P_ALTA_ABAJO;
				if (probabilidadDeMovimiento == 6) return P_BAJA_ABAJO;
				if (probabilidadDeMovimiento == 7) return G_GANCHO;
			}
			if (unEstado.accion == AGACHADO){
				probabilidadDeMovimiento = (rand() % 4);
				if (probabilidadDeMovimiento == 0) return G_ABAJO;
				if (probabilidadDeMovimiento == 1) return P_ALTA_ABAJO;
				if (probabilidadDeMovimiento == 2) return P_BAJA_ABAJO;
				if (probabilidadDeMovimiento == 3) return G_GANCHO;
			}
		}
	}
	probabilidadDeMovimiento = (rand() % 10);
	if (probabilidadDeMovimiento == 1){
		cantidadDeMovimientosAdelante = 10;
		return adelante;
	}
	if (probabilidadDeMovimiento == 2){
		cantidadDeMovimientosAtras = 10;
		return atras;
	}
	if (probabilidadDeMovimiento > 2) return QUIETO;
	movimiento = (MOV_TIPO)(rand() % 22);
	if (movimiento != CERRAR || movimiento != RECARGAR) return movimiento;
	return QUIETO;
}


void IA::setPersonajeRival(Personaje* unPersonaje){
	personajeRival = unPersonaje;
}


void IA::setPersonajeControlado(Personaje* unPersonaje){
	personajeControlado = unPersonaje;
}


IA::~IA()
{
	personajeRival = nullptr;
	personajeControlado = nullptr;
}
