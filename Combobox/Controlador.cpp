#include "stdafx.h"
#include "Controlador.h"
#include "ConversorAString.h"

Controlador::Controlador()
{
	cantidadDeEventosAnterior = 0;
	otraCantidadDeEventosAnterior = 0;
	j = 0;
	golpeBajoTecladoHabilitado = true;
	golpeAltoTecladoHabilitado = true;
	patadaBajaTecladoHabilitada = true;
	patadaAltaTecladoHabilitada = true;
	armaTecladoHabilitada = true;

	golpeBajoJoystickBotonHabilitado = true;
	golpeBajoJoystickFlechaHabilitado = true;
	golpeBajoJoystickEjeHabilitado = true;

	golpeAltoJoystickBotonHabilitado = true;
	golpeAltoJoystickFlechaHabilitado = true;
	golpeAltoJoystickEjeHabilitado = true;

	patadaBajaJoystickBotonHabilitada = true;
	patadaBajaJoystickFlechaHabilitada = true;
	patadaBajaJoystickEjeHabilitada = true;

	patadaAltaJoystickBotonHabilitada = true;
	patadaAltaJoystickFlechaHabilitada = true;
	patadaAltaJoystickEjeHabilitada = true;

	movimientos = std::vector<Movimiento*>();
	movimientos.push_back(new Movimiento());

	movimientosActivos = std::vector<Movimiento*>();

	timer = new Timer();
	timer->start();

	controladorDeTomas = new ControladorDeTomas();
	controladorDeTomas->setMovimientos(&movimientosActivos);
	controladorDeTomas->setTimer(timer);

	conversorDeEventos = nullptr;
	if (SDL_NumJoysticks() > 0){
		unJoystick = SDL_JoystickOpen(0);
		if (SDL_NumJoysticks() > 1){
			otroJoystick = SDL_JoystickOpen(1);
		}
	}
}


void Controlador::setConversorDeEventos(ConversorDeEventos* unConversorDeEventos){
	conversorDeEventos = unConversorDeEventos;
}


std::vector<Movimiento*> Controlador::getMovimientos(){
	return movimientos;
}


void Controlador::mantenerMovimientosActivos(){
	std::vector<Movimiento*> movimientosAuxilares;

	if (movimientosActivos.size() == 20){
		movimientosAuxilares = std::vector<Movimiento*>();
		for (size_t i = 1; i < movimientosActivos.size(); i++){
			movimientosAuxilares.push_back(movimientosActivos.at(i));
		}
		movimientosActivos = movimientosAuxilares;
	}
}


void Controlador::mantenerMovimientos(){
	std::vector<Movimiento*> movimientosAuxilares;

	if (movimientos.size() == CANTIDAD_MAXIMA_EVENTOS){
		movimientosAuxilares = std::vector<Movimiento*>();
		for (size_t i = (movimientos.size() - (CANTIDAD_MAXIMA_EVENTOS / 2) - 1); i < movimientos.size(); i++){
			movimientosAuxilares.push_back(movimientos.at(i));
		}
		movimientos = movimientosAuxilares;
	}

}


ControladorDeTomas* Controlador::getControladorDeTomas(){
	return controladorDeTomas;
}


MOV_TIPO Controlador::getUltimoMovTipo(){
	Movimiento* unMovimiento;
	MOV_TIPO unMov;

	if (!movimientos.empty()){
		unMovimiento = movimientos.back();
		unMov = unMovimiento->getMovimiento();
		if (otraCantidadDeEventosAnterior != movimientos.size()){
			otraCantidadDeEventosAnterior = movimientos.size();
			if (unMov != QUIETO){
				return unMov;
			}
		}
	}
	return QUIETO;
}


Movimiento* Controlador::getUltimoMovimientoActivo(){
	
	Movimiento* unMovimiento;
	if (!movimientos.empty()){
		unMovimiento = movimientos.back();
		if (cantidadDeEventosAnterior != movimientos.size()){
			cantidadDeEventosAnterior = movimientos.size();
			if (unMovimiento->getMovimiento() != QUIETO) return unMovimiento;
		}
	}
	return nullptr;
}


int Controlador::cambiar(){

	Movimiento* otroMovimiento = new Movimiento();
	otroMovimiento->setTiempo(timer->getTicks());

	if (conversorDeEventos != nullptr){

	SDL_JoystickUpdate();
	SDL_PumpEvents();
	state = SDL_GetKeyboardState(NULL);

	//SALIR
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) return FIN;

	//ACTUALIZO EL VECTOR
	mantenerMovimientos();
	mantenerMovimientosActivos();
	//ConversorAString* unConversorAString = new ConversorAString();

	Movimiento* unMovimiento;

	/*if (MOSTRAR_MOVIMIENTOS){
	std::string mensaje = unConversorAString->getTeclaComoStringDelMovimientoParaElConversorDeEventos(unMovimiento->getMovimiento(), conversorDeEventos);
	if (mensaje != "") std::cout << mensaje << "\n";
	delete unConversorAString;
	}*/
	//TECLADO--------------------------------------------------------------------------
	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::QUIT)]) {
		return FIN;
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::REBOOT)]) {
		return REINICIAR;
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::REBOOT_FIGHT)]) {
		return REINICIAR_PELEA;
	}

	if (!state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LOW_PUNCH)]) golpeBajoTecladoHabilitado = true;
	if (!state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::HIGH_PUNCH)]) golpeAltoTecladoHabilitado = true;
	if (!state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LOW_KICK)]) patadaBajaTecladoHabilitada = true;
	if (!state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::HIGH_KICK)]) patadaAltaTecladoHabilitada = true;
	if (!state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::WEAPON)]) armaTecladoHabilitada = true;

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LOW_PUNCH)]){
		if (golpeBajoTecladoHabilitado){
			golpeBajoTecladoHabilitado = false;
			if (movimientos.back()->getMovimiento() == ABAJO){
				otroMovimiento->setMovimiento(G_ABAJO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
				return CONTINUAR;
			}
			if (movimientos.back()->getMovimiento() == G_ABAJO) return CONTINUAR;

			if (movimientos.back()->getMovimiento() != G_BAJO){
				otroMovimiento->setMovimiento(G_BAJO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::HIGH_PUNCH)]){
		if (golpeAltoTecladoHabilitado){
			golpeAltoTecladoHabilitado = false;
			if (movimientos.back()->getMovimiento() == ABAJO){
				otroMovimiento->setMovimiento(G_GANCHO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
				return CONTINUAR;
			}
			if (movimientos.back()->getMovimiento() == G_GANCHO) return CONTINUAR;

			if (movimientos.back()->getMovimiento() != G_ALTO){
				otroMovimiento->setMovimiento(G_ALTO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LOW_KICK)]){
		if (patadaBajaTecladoHabilitada){
			patadaBajaTecladoHabilitada = false;
			if (movimientos.back()->getMovimiento() == ABAJO){
				otroMovimiento->setMovimiento(P_BAJA_ABAJO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
				return CONTINUAR;
			}
			if (movimientos.back()->getMovimiento() == P_BAJA_ABAJO) return CONTINUAR;

			if (movimientos.back()->getMovimiento() != P_BAJA){
				otroMovimiento->setMovimiento(P_BAJA);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::HIGH_KICK)]){
		if (patadaAltaTecladoHabilitada){
			patadaAltaTecladoHabilitada = false;
			if (movimientos.back()->getMovimiento() == ABAJO){
				otroMovimiento->setMovimiento(P_ALTA_ABAJO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
				return CONTINUAR;
			}
			if (movimientos.back()->getMovimiento() == P_ALTA_ABAJO) return CONTINUAR;

			if (movimientos.back()->getMovimiento() != P_ALTA){
				otroMovimiento->setMovimiento(P_ALTA);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::WEAPON)]){
		if (armaTecladoHabilitada){
			armaTecladoHabilitada = false;
			if (movimientos.back()->getMovimiento() != ARMA){
				otroMovimiento->setMovimiento(ARMA);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}	
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::HOLD)]){
		if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::DOWN)]){
			if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
				otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
				return CONTINUAR;
		}
		if (movimientos.back()->getMovimiento() != DEFENSA){
			otroMovimiento->setMovimiento(DEFENSA);
			movimientos.push_back(otroMovimiento);
			unMovimiento = getUltimoMovimientoActivo();
			if (unMovimiento != nullptr) {
				movimientosActivos.push_back(unMovimiento);
			}
		}
		return CONTINUAR;
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::DOWN)]){
		if (movimientos.back()->getMovimiento() != ABAJO){
			otroMovimiento->setMovimiento(ABAJO);
			movimientos.push_back(otroMovimiento);
			unMovimiento = getUltimoMovimientoActivo();
			if (unMovimiento != nullptr) {
				movimientosActivos.push_back(unMovimiento);
			}
		}
		return CONTINUAR;
	}

	if ((state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LEFT)]) && (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::RIGHT)]))
		if ((movimientos.back()->getMovimiento() == DER) || (movimientos.back()->getMovimiento() == IZQ))
			return CONTINUAR;

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::RIGHT)]){
		if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::UP)]){
			if (movimientos.back()->getMovimiento() != SALTODER) {
				otroMovimiento->setMovimiento(SALTODER);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
		if (movimientos.back()->getMovimiento() != DER){
			otroMovimiento->setMovimiento(DER);
			movimientos.push_back(otroMovimiento);
			unMovimiento = getUltimoMovimientoActivo();
			if (unMovimiento != nullptr) {
				movimientosActivos.push_back(unMovimiento);
			}
		}
		return CONTINUAR;
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::LEFT)]){
		if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::UP)]){
			if (movimientos.back()->getMovimiento() != SALTOIZQ){
				otroMovimiento->setMovimiento(SALTOIZQ);
				movimientos.push_back(otroMovimiento);
				unMovimiento = getUltimoMovimientoActivo();
				if (unMovimiento != nullptr) {
					movimientosActivos.push_back(unMovimiento);
				}
			}
			return CONTINUAR;
		}
		if (movimientos.back()->getMovimiento() != IZQ){
			otroMovimiento->setMovimiento(IZQ);
			movimientos.push_back(otroMovimiento);
			unMovimiento = getUltimoMovimientoActivo();
			if (unMovimiento != nullptr) {
				movimientosActivos.push_back(unMovimiento);
			}
		}
		return CONTINUAR;
	}

	if (state[conversorDeEventos->getScanCodeDeLaAccion(ConversorDeEventos::UP)]){
		if (movimientos.back()->getMovimiento() != ARRIBA){
			otroMovimiento->setMovimiento(ARRIBA);
			movimientos.push_back(otroMovimiento);
			unMovimiento = getUltimoMovimientoActivo();
			if (unMovimiento != nullptr) {
				movimientosActivos.push_back(unMovimiento);
			}
		}
		return CONTINUAR;
	}
	
	//JOYSTICK-----------------------------------------------------------------------------------
	if (SDL_NumJoysticks() > 0){
		SDL_Joystick* joystickAlternativo = nullptr;

		//PATADA ALTA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HIGH_KICK) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HIGH_KICK) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HIGH_KICK)) == 1){
				if (patadaAltaJoystickBotonHabilitada){
					patadaAltaJoystickBotonHabilitada = false;
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO) {
							otroMovimiento->setMovimiento(P_ALTA_ABAJO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
								otroMovimiento->setMovimiento(P_ALTA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
								otroMovimiento->setMovimiento(P_ALTA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != P_ALTA) {
						otroMovimiento->setMovimiento(P_ALTA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
			else patadaAltaJoystickBotonHabilitada = true;

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::HIGH_KICK)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::HIGH_KICK) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (patadaAltaJoystickFlechaHabilitada){
						patadaAltaJoystickFlechaHabilitada = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO) {
								otroMovimiento->setMovimiento(P_ALTA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
									otroMovimiento->setMovimiento(P_ALTA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
									otroMovimiento->setMovimiento(P_ALTA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != P_ALTA){
							otroMovimiento->setMovimiento(P_ALTA);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else patadaAltaJoystickFlechaHabilitada = true;

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::HIGH_KICK)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HIGH_KICK));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::HIGH_KICK)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::HIGH_KICK))){
					if (patadaAltaJoystickEjeHabilitada){
						patadaAltaJoystickEjeHabilitada = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
								otroMovimiento->setMovimiento(P_ALTA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
									otroMovimiento->setMovimiento(P_ALTA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != P_ALTA_ABAJO){
									otroMovimiento->setMovimiento(P_ALTA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != P_ALTA){
							otroMovimiento->setMovimiento(P_ALTA);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}

			}
		}
		else patadaAltaJoystickEjeHabilitada = true;

		//PATADA BAJA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LOW_KICK) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LOW_KICK) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LOW_KICK)) == 1){
				if (patadaBajaJoystickBotonHabilitada){
					patadaBajaJoystickBotonHabilitada = false;
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
							otroMovimiento->setMovimiento(P_BAJA_ABAJO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO) {
								otroMovimiento->setMovimiento(P_BAJA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
								otroMovimiento->setMovimiento(P_BAJA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != P_BAJA){
						otroMovimiento->setMovimiento(P_BAJA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
			else patadaBajaJoystickBotonHabilitada = true;

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::LOW_KICK)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LOW_KICK) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (patadaBajaJoystickFlechaHabilitada){
						patadaBajaJoystickFlechaHabilitada = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
								otroMovimiento->setMovimiento(P_BAJA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO) {
									otroMovimiento->setMovimiento(P_BAJA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO) {
									otroMovimiento->setMovimiento(P_BAJA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != P_BAJA){
							otroMovimiento->setMovimiento(P_BAJA);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else patadaBajaJoystickFlechaHabilitada = true;

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LOW_KICK)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LOW_KICK));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LOW_KICK)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::LOW_KICK))){
					if (patadaBajaJoystickEjeHabilitada){
						patadaBajaJoystickEjeHabilitada = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
								otroMovimiento->setMovimiento(P_BAJA_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
									otroMovimiento->setMovimiento(P_BAJA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != P_BAJA_ABAJO){
									otroMovimiento->setMovimiento(P_BAJA_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != P_BAJA) {
							otroMovimiento->setMovimiento(P_BAJA);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else patadaBajaJoystickEjeHabilitada = true;

		}


		//GOLPE ALTO----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HIGH_PUNCH) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HIGH_PUNCH) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HIGH_PUNCH)) == 1){
				if (golpeAltoJoystickBotonHabilitado){
					golpeAltoJoystickBotonHabilitado = false;
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != G_GANCHO){
							otroMovimiento->setMovimiento(G_GANCHO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != G_GANCHO) {
								otroMovimiento->setMovimiento(G_GANCHO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != G_GANCHO){
								otroMovimiento->setMovimiento(G_GANCHO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != G_ALTO){
						otroMovimiento->setMovimiento(G_ALTO);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
			else golpeAltoJoystickBotonHabilitado = true;

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::HIGH_PUNCH)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::HIGH_PUNCH) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (golpeAltoJoystickFlechaHabilitado){
						golpeAltoJoystickFlechaHabilitado = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != G_GANCHO){
								otroMovimiento->setMovimiento(G_GANCHO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != G_GANCHO){
									otroMovimiento->setMovimiento(G_GANCHO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != G_GANCHO){
									otroMovimiento->setMovimiento(G_GANCHO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != G_ALTO){
							otroMovimiento->setMovimiento(G_ALTO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else golpeAltoJoystickFlechaHabilitado = true;

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::HIGH_PUNCH)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HIGH_PUNCH));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::HIGH_PUNCH)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::HIGH_PUNCH))){
					if (golpeAltoJoystickEjeHabilitado){
						golpeAltoJoystickEjeHabilitado = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != G_GANCHO){
								otroMovimiento->setMovimiento(G_GANCHO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != G_GANCHO){
									otroMovimiento->setMovimiento(G_GANCHO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != G_GANCHO){
									otroMovimiento->setMovimiento(G_GANCHO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != G_ALTO) {
							otroMovimiento->setMovimiento(G_ALTO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else golpeAltoJoystickEjeHabilitado = true;

		}

		//GOLPE BAJO----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LOW_PUNCH) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LOW_PUNCH) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LOW_PUNCH)) == 1){
				if (golpeBajoJoystickBotonHabilitado){
					golpeBajoJoystickBotonHabilitado = false;
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != G_ABAJO){
							otroMovimiento->setMovimiento(G_ABAJO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != G_ABAJO) {
								otroMovimiento->setMovimiento(G_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != G_ABAJO) {
								otroMovimiento->setMovimiento(G_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != G_BAJO) {
						otroMovimiento->setMovimiento(G_BAJO);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
			else golpeBajoJoystickBotonHabilitado = true;

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::LOW_PUNCH)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LOW_PUNCH) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (golpeBajoJoystickFlechaHabilitado){
						golpeBajoJoystickFlechaHabilitado = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != G_ABAJO){
								otroMovimiento->setMovimiento(G_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != G_ABAJO){
									otroMovimiento->setMovimiento(G_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != G_ABAJO){
									otroMovimiento->setMovimiento(G_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != G_BAJO){
							otroMovimiento->setMovimiento(G_BAJO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else golpeBajoJoystickFlechaHabilitado = true;

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LOW_PUNCH)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LOW_PUNCH));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LOW_PUNCH)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::HIGH_PUNCH))){
					if (golpeBajoJoystickEjeHabilitado){
						golpeBajoJoystickEjeHabilitado = false;
						//BOTON
						if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
							if (movimientos.back()->getMovimiento() != G_ABAJO) {
								otroMovimiento->setMovimiento(G_ABAJO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
						//FLECHAS
						if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
							if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
								if (movimientos.back()->getMovimiento() != G_ABAJO) {
									otroMovimiento->setMovimiento(G_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//EJES
						if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
							valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
							if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
								if (movimientos.back()->getMovimiento() != G_ABAJO) {
									otroMovimiento->setMovimiento(G_ABAJO);
									movimientos.push_back(otroMovimiento);
									unMovimiento = getUltimoMovimientoActivo();
									if (unMovimiento != nullptr) {
										movimientosActivos.push_back(unMovimiento);
									}
								}
								return CONTINUAR;
							}
						}
						//NADA
						if (movimientos.back()->getMovimiento() != G_BAJO){
							otroMovimiento->setMovimiento(G_BAJO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
			}
			else golpeBajoJoystickEjeHabilitado = true;

		}

		//SALIR----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::QUIT) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::QUIT) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::QUIT)) == 1){
				return FIN;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::QUIT)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::QUIT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					return FIN;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::QUIT)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::QUIT));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::QUIT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::QUIT))){
					return FIN;
				}
			}

		}

		//REINICIAR PELEA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::REBOOT_FIGHT) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::REBOOT_FIGHT) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::REBOOT_FIGHT)) == 1){
				return REINICIAR_PELEA;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::REBOOT_FIGHT)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::REBOOT_FIGHT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					return REINICIAR_PELEA;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::REBOOT_FIGHT)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::REBOOT_FIGHT));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::REBOOT_FIGHT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::REBOOT_FIGHT))){
					return REINICIAR_PELEA;
				}
			}

		}

		//REINICIAR----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::REBOOT) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::REBOOT) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::REBOOT)) == 1){
				return REINICIAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::REBOOT)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::REBOOT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					return REINICIAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::REBOOT)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::REBOOT));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::REBOOT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::REBOOT))){
					return REINICIAR;
				}
			}

		}


		//DEFENSA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HOLD) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::HOLD) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HOLD)) == 1){
				//BOTON
				if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
					if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
						otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				//FLECHAS
				if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
					if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
						if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
							otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//EJES
				if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
					valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
					if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
						if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
							otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//NADA
				if (movimientos.back()->getMovimiento() != DEFENSA) {
					otroMovimiento->setMovimiento(DEFENSA);
					movimientos.push_back(otroMovimiento);
					unMovimiento = getUltimoMovimientoActivo();
					if (unMovimiento != nullptr) {
						movimientosActivos.push_back(unMovimiento);
					}
				}
				return CONTINUAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::HOLD)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::HOLD) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
							otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
								otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
								otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != DEFENSA){
						otroMovimiento->setMovimiento(DEFENSA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::HOLD)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::HOLD));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::HOLD)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::HOLD))){
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
						if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
							otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
								otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
							if (movimientos.back()->getMovimiento() != DEFENSA_AGACHADO){
								otroMovimiento->setMovimiento(DEFENSA_AGACHADO);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != DEFENSA){
						otroMovimiento->setMovimiento(DEFENSA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

		}


		//ABAJO----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::DOWN) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::DOWN) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN)) == 1){
				if (movimientos.back()->getMovimiento() != ABAJO) {
					otroMovimiento->setMovimiento(ABAJO);
					movimientos.push_back(otroMovimiento);
					unMovimiento = getUltimoMovimientoActivo();
					if (unMovimiento != nullptr) {
						movimientosActivos.push_back(unMovimiento);
					}
				}
				return CONTINUAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::DOWN)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::DOWN) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (movimientos.back()->getMovimiento() != ABAJO){
						otroMovimiento->setMovimiento(ABAJO);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::DOWN)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::DOWN));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::DOWN)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::DOWN))){
					if (movimientos.back()->getMovimiento() != ABAJO){
						otroMovimiento->setMovimiento(ABAJO);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

		}



		//ARRIBA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::UP) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::UP) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::UP)) == 1){
				//BOTON DER
				if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT)) == 1){
					if (movimientos.back()->getMovimiento() != SALTODER){
						otroMovimiento->setMovimiento(SALTODER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				//FLECHAS DER 
				if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::RIGHT)){
					if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
						if (movimientos.back()->getMovimiento() != SALTODER) {
							otroMovimiento->setMovimiento(SALTODER);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//EJES DER
				if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::RIGHT)){
					valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT));
					if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::RIGHT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::RIGHT))){
						if (movimientos.back()->getMovimiento() != SALTODER){
							otroMovimiento->setMovimiento(SALTODER);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//BOTON IZQ
				if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT)) == 1){
					if (movimientos.back()->getMovimiento() != SALTOIZQ) {
						otroMovimiento->setMovimiento(SALTOIZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				//FLECHAS IZQ
				if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::LEFT)){
					if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
						if (movimientos.back()->getMovimiento() != SALTOIZQ){
							otroMovimiento->setMovimiento(SALTOIZQ);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//EJES IZQ
				if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LEFT)){
					valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT));
					if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LEFT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::LEFT))){
						if (movimientos.back()->getMovimiento() != SALTOIZQ){
							otroMovimiento->setMovimiento(SALTOIZQ);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
				}
				//NADA
				if (movimientos.back()->getMovimiento() != ARRIBA){
					otroMovimiento->setMovimiento(ARRIBA);
					movimientos.push_back(otroMovimiento);
					unMovimiento = getUltimoMovimientoActivo();
					if (unMovimiento != nullptr) {
						movimientosActivos.push_back(unMovimiento);
					}
				}
				return CONTINUAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::UP)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT)) == 1){
						if (movimientos.back()->getMovimiento() != SALTODER){
							otroMovimiento->setMovimiento(SALTODER);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::RIGHT)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::RIGHT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::RIGHT))){
							if (movimientos.back()->getMovimiento() != SALTODER){
								otroMovimiento->setMovimiento(SALTODER);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//BOTON IZQ
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT)) == 1){
						if (movimientos.back()->getMovimiento() != SALTOIZQ){
							otroMovimiento->setMovimiento(SALTOIZQ);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//EJES IZQ
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LEFT)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LEFT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::LEFT))){
							if (movimientos.back()->getMovimiento() != SALTOIZQ){
								otroMovimiento->setMovimiento(SALTOIZQ);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != ARRIBA){
						otroMovimiento->setMovimiento(ARRIBA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::UP)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::UP));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::UP)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::UP))){
					//BOTON
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT)) == 1){
						if (movimientos.back()->getMovimiento() != SALTODER){
							otroMovimiento->setMovimiento(SALTODER);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::RIGHT)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != SALTODER) {
								otroMovimiento->setMovimiento(SALTODER);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::RIGHT)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::RIGHT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::RIGHT))){
							if (movimientos.back()->getMovimiento() != SALTODER){
								otroMovimiento->setMovimiento(SALTODER);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//BOTON IZQ
					if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT)) == 1){
						if (movimientos.back()->getMovimiento() != SALTOIZQ){
							otroMovimiento->setMovimiento(SALTOIZQ);
							movimientos.push_back(otroMovimiento);
							unMovimiento = getUltimoMovimientoActivo();
							if (unMovimiento != nullptr) {
								movimientosActivos.push_back(unMovimiento);
							}
						}
						return CONTINUAR;
					}
					//FLECHAS IZQ
					if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::LEFT)){
						if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
							if (movimientos.back()->getMovimiento() != SALTOIZQ){
								otroMovimiento->setMovimiento(SALTOIZQ);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//EJES IZQ
					if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LEFT)){
						valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT));
						if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LEFT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::LEFT))){
							if (movimientos.back()->getMovimiento() != SALTOIZQ){
								otroMovimiento->setMovimiento(SALTOIZQ);
								movimientos.push_back(otroMovimiento);
								unMovimiento = getUltimoMovimientoActivo();
								if (unMovimiento != nullptr) {
									movimientosActivos.push_back(unMovimiento);
								}
							}
							return CONTINUAR;
						}
					}
					//NADA
					if (movimientos.back()->getMovimiento() != ARRIBA) {
						otroMovimiento->setMovimiento(ARRIBA);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

		}

		//SALTOS COSTADOS CON FLECHAS-----------------------------------------------------------------------------
		if ((conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::UP) == 0) && (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::RIGHT) == 0)){
			joystickAlternativo = unJoystick;
		}
		else{
			if ((conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::UP) == 1) && (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::RIGHT) == 1)) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){
			if (SDL_JoystickGetHat(joystickAlternativo, 0) == SDL_HAT_RIGHTUP){
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_UP) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_RIGHT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_RIGHT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_UP))){
					if (movimientos.back()->getMovimiento() != SALTODER){
						otroMovimiento->setMovimiento(SALTODER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_UP) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_LEFT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_LEFT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_UP))){
					if (movimientos.back()->getMovimiento() != SALTOIZQ) {
						otroMovimiento->setMovimiento(SALTOIZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			if (SDL_JoystickGetHat(joystickAlternativo, 0) == SDL_HAT_LEFTUP){
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_UP) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_RIGHT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_RIGHT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_UP))){
					if (movimientos.back()->getMovimiento() != SALTODER){
						otroMovimiento->setMovimiento(SALTODER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_UP) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_LEFT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_LEFT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_UP))){
					if (movimientos.back()->getMovimiento() != SALTOIZQ){
						otroMovimiento->setMovimiento(SALTOIZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
			if (SDL_JoystickGetHat(joystickAlternativo, 0) == SDL_HAT_RIGHTDOWN){
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_DOWN) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_RIGHT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_RIGHT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_DOWN))){
					if (movimientos.back()->getMovimiento() != SALTODER) {
						otroMovimiento->setMovimiento(SALTODER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_DOWN) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_LEFT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_LEFT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_HAT_DOWN))){
					if (movimientos.back()->getMovimiento() != SALTOIZQ){
						otroMovimiento->setMovimiento(SALTOIZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			if (SDL_JoystickGetHat(joystickAlternativo, 0) == SDL_HAT_LEFTDOWN){
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_DOWN) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_RIGHT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_RIGHT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_DOWN))){
					if (movimientos.back()->getMovimiento() != SALTODER) {
						otroMovimiento->setMovimiento(SALTODER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
				if (((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_DOWN) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_LEFT)) || ((conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::UP) == SDL_HAT_LEFT) && (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_HAT_DOWN))){
					if (movimientos.back()->getMovimiento() != SALTOIZQ){
						otroMovimiento->setMovimiento(SALTOIZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}
		}


		//DERECHA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::RIGHT) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::RIGHT) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT)) == 1){
				if (movimientos.back()->getMovimiento() == IZQ){
					return CONTINUAR;
				}
				if (movimientos.back()->getMovimiento() != DER){
					otroMovimiento->setMovimiento(DER);
					movimientos.push_back(otroMovimiento);
					unMovimiento = getUltimoMovimientoActivo();
					if (unMovimiento != nullptr) {
						movimientosActivos.push_back(unMovimiento);
					}
				}
				return CONTINUAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::RIGHT)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::RIGHT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (movimientos.back()->getMovimiento() == IZQ){
						return CONTINUAR;
					}
					if (movimientos.back()->getMovimiento() != DER){
						otroMovimiento->setMovimiento(DER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::RIGHT)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::RIGHT));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::RIGHT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::RIGHT))){
					if (movimientos.back()->getMovimiento() == IZQ){
						return CONTINUAR;
					}
					if (movimientos.back()->getMovimiento() != DER){
						otroMovimiento->setMovimiento(DER);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

		}


		//IZQUIERDA----------------------------------------------------------------------------
		if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LEFT) == 0){
			joystickAlternativo = unJoystick;
		}
		else{
			if (conversorDeEventos->geNnumeroDeJoystickDeLaAccion(ConversorDeEventos::LEFT) == 1) joystickAlternativo = otroJoystick;
		}
		if (joystickAlternativo != nullptr){

			//BOTON
			if (SDL_JoystickGetButton(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT)) == 1){
				if (movimientos.back()->getMovimiento() == DER){
					return CONTINUAR;
				}
				if (movimientos.back()->getMovimiento() != IZQ){
					otroMovimiento->setMovimiento(IZQ);
					movimientos.push_back(otroMovimiento);
					unMovimiento = getUltimoMovimientoActivo();
					if (unMovimiento != nullptr) {
						movimientosActivos.push_back(unMovimiento);
					}
				}
				return CONTINUAR;
			}

			//FLECHAS
			if (conversorDeEventos->hayFlechasParaLaAccion(ConversorDeEventos::LEFT)){
				if (conversorDeEventos->getHatDeLaAccion(ConversorDeEventos::LEFT) == SDL_JoystickGetHat(joystickAlternativo, 0)){
					if (movimientos.back()->getMovimiento() == DER){
						return CONTINUAR;
					}
					if (movimientos.back()->getMovimiento() != IZQ) {
						otroMovimiento->setMovimiento(IZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

			//EJES
			if (conversorDeEventos->hayEjeParaLaAccion(ConversorDeEventos::LEFT)){
				valorDelEje = SDL_JoystickGetAxis(joystickAlternativo, conversorDeEventos->getJbotonDeLaAccion(ConversorDeEventos::LEFT));
				if ((valorDelEje >= conversorDeEventos->getValorDesdeDelEjeDeLaAccion(ConversorDeEventos::LEFT)) && (valorDelEje <= conversorDeEventos->getValorHastaDelEjeDeLaAccion(ConversorDeEventos::LEFT))){
					if (movimientos.back()->getMovimiento() == DER){
						return CONTINUAR;
					}
					if (movimientos.back()->getMovimiento() != IZQ){
						otroMovimiento->setMovimiento(IZQ);
						movimientos.push_back(otroMovimiento);
						unMovimiento = getUltimoMovimientoActivo();
						if (unMovimiento != nullptr) {
							movimientosActivos.push_back(unMovimiento);
						}
					}
					return CONTINUAR;
				}
			}

		}
	}

	}
	
	if (movimientos.back()->getMovimiento() != QUIETO){
		otroMovimiento->setMovimiento(QUIETO);
		movimientos.push_back(otroMovimiento);
	}

	return CONTINUAR;
	
}


ConversorDeEventos* Controlador::getConversorDeEventos(){
	return conversorDeEventos;
}


Controlador::~Controlador()
{
	for (size_t i = 0; i < movimientos.size(); i++){
		delete movimientos.at(i);
	}
	movimientos.clear();

	movimientosActivos.clear();

	delete controladorDeTomas;
}