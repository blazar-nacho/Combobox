#pragma once
#include "Constantes.h"
#include "ConversorDeEventos.h"
#include "ControladorDeTomas.h"
#include "Movimiento.h"
#include "Timer.h"

class Controlador
{
public:
	Controlador();
	
	~Controlador();
	virtual int cambiar();
	virtual std::vector<Movimiento*> getMovimientos();
	ConversorDeEventos* getConversorDeEventos();
	void setConversorDeEventos(ConversorDeEventos* unConversorDeEventos);
	Movimiento* getUltimoMovimientoActivo();
	MOV_TIPO getUltimoMovTipo();
	ControladorDeTomas* getControladorDeTomas();

private:
	MOV_TIPO movimientoAnterior;
	int j;
	Timer* timer;
	std::vector<Movimiento*> movimientos;
	std::vector<Movimiento*> movimientosActivos;
	ConversorDeEventos* conversorDeEventos;
	SDL_Event event;
	const Uint8 *state;
	SDL_Keycode keyCode;
	SDL_JoystickID numeroDeJoystick;
	Uint8 jBoton;
	SDL_Joystick* unJoystick;
	SDL_Joystick* otroJoystick;
	Sint16 valorDelEje;
	int cantidadDeEventosAnterior;
	int otraCantidadDeEventosAnterior;
	ControladorDeTomas* controladorDeTomas;
	bool golpeBajoTecladoHabilitado;
	bool golpeAltoTecladoHabilitado;
	bool patadaBajaTecladoHabilitada;
	bool patadaAltaTecladoHabilitada;
	bool armaTecladoHabilitada;

	bool golpeBajoJoystickBotonHabilitado;
	bool golpeBajoJoystickFlechaHabilitado;
	bool golpeBajoJoystickEjeHabilitado;

	bool golpeAltoJoystickBotonHabilitado;
	bool golpeAltoJoystickFlechaHabilitado;
	bool golpeAltoJoystickEjeHabilitado;

	bool patadaBajaJoystickBotonHabilitada;
	bool patadaBajaJoystickFlechaHabilitada;
	bool patadaBajaJoystickEjeHabilitada;

	bool patadaAltaJoystickBotonHabilitada;
	bool patadaAltaJoystickFlechaHabilitada;
	bool patadaAltaJoystickEjeHabilitada;

	void mantenerMovimientos();
	void mantenerMovimientosActivos();
	
};

