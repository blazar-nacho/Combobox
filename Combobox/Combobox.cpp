// ComboBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// al incluir vld.h se hace la deteccion de perdidas de memoria
#include <vld.h>
#include "Parser.h"
#include "Vista.h"
#include "Mundo.h"
#include "ControladorAutomatico.h"
#include "ControladorDeMouse.h"
//#include <windows.h>
#include "Combobox.h"
#include "Timer.h"
#include "Sprites.h"
#include "Sonido.h"
#include "Menu.h"
#include "Modo.h"

//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
int _tmain(int argc, _TCHAR* argv[])
{

#ifdef DEBUGENVENTANA
	cout << "Esta activado el modo DEBUG EN VENTANA, se registra solo por consola" << "\n";
#endif
	bool error;

	int accion = REINICIAR;

	while (accion == REINICIAR){
		Controlador* controladorUno;
		Controlador* controladorDos;
		if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0){
			const char* msg = ((std::string)"Error iniciando SDL: ").append(SDL_GetError()).c_str();
			Log::getInstancia().logearMensajeEnModo("No se puede inicializar SDL, se cierra el programa", Log::MODO_ERROR);
			return  EXIT_FAILURE;
		}

		//Se inicializa SDLttf
		if (TTF_Init() == -1)
			Log::getInstancia().logearMensajeEnModo(("SDL_ttf no se pudo iniciar! SDL_ttf Error: %s\n", TTF_GetError()), Log::MODO_ERROR);

		//Inicializo el sonido
		Sonido::Iniciar();

		std::string nombreArchivo(PRUEBA_JSON);
		bool ParseoExitoso = Parser::getInstancia().parsear(nombreArchivo);

		if (!ParseoExitoso) return EXIT_FAILURE; //si el json por defecto fallo, el programa termina

		vector2D vecGravedad(0.0f, GRAVEDAD_Y);

		/*********************************************************************/
		/*      Inicialización de vista y variables del juego               */
		/*********************************************************************/
		//Parte de creación inicial.		
		Mundo* unMundo = new Mundo(vecGravedad);
		Vista* unaVista = new Vista(unMundo, &error, true);

		if (error){
			Log::getInstancia().logearMensajeEnModo("Error iniciando SDL con aceleracion de hardware, se iniciara en modo software...", Log::MODO_WARNING);
			delete unaVista;
			Vista* unaVista = new Vista(unMundo, &error, false);
		}
		if (error){
			std::string mensaje = ((std::string)"Error iniciando SDL: ").append(SDL_GetError()).c_str();
			mensaje += ", se cierra el programa";
			Log::getInstancia().logearMensajeEnModo(mensaje, Log::MODO_ERROR);
			delete unaVista;
			return  EXIT_FAILURE;
		}

		defCuerpo personaje1 = defCuerpo();
		personaje1.posicion = vector2D((P1_POS_INI + Parser::getInstancia().getEscenario().getAncho() / 2), (Parser::getInstancia().getEscenario().getYPiso()));
		defCuerpo personaje2 = defCuerpo();
		personaje2.posicion = vector2D((float)(P2_POS_INI + Parser::getInstancia().getEscenario().getAncho() / 2), (Parser::getInstancia().getEscenario().getYPiso()));

		accion = INICIAR;
		while (accion == INICIAR){

			//Instanciación de modos de juego

			Menu* menu = new Menu(new Modo(unaVista));
			int resultadoModo = menu->seleccionarModo(Parser::getInstancia().getControlador1(), Parser::getInstancia().getControlador2());

			controladorUno = menu->getModo()->getControlador1();
			controladorDos = menu->getModo()->getControlador2();

			//Personaje* perso1 = Parser::getInstancia().getPelea()->getPersonaje1();
			personaje1.nombre = Parser::getInstancia().getPelea()->getPersonaje1()->getNombre();

			Cuerpo *unCuerpo = new Cuerpo(personaje1, controladorUno);
			unCuerpo->recibeObservador(Parser::getInstancia().getPelea()->getPersonaje1());
			unCuerpo->setSensores(unCuerpo->getSprite()->getSensores());
			unCuerpo->setSensoresProyectil(unCuerpo->getSprite()->getSensoresDisparo());

			unMundo->agregarCuerpo(unCuerpo);

			personaje2.nombre = Parser::getInstancia().getPelea()->getPersonaje2()->getNombre();

			Cuerpo *otroCuerpo = new Cuerpo(personaje2, controladorDos);
			otroCuerpo->recibeObservador(Parser::getInstancia().getPelea()->getPersonaje2());
			otroCuerpo->setSensores(otroCuerpo->getSprite()->getSensores());
			otroCuerpo->setSensoresProyectil(otroCuerpo->getSprite()->getSensoresDisparo());

			unMundo->agregarCuerpo(otroCuerpo);

			//HABILITAR VIBRACION
			unaVista->habilitarVibracion();
			//Timer de cuadros por segundo
			Timer fpsTimer;
			//Timer del corte de cuadros por segundo
			Timer capTimer;

			//Contador de cuadros
			int conteoDeCuadros = 0;
			fpsTimer.start();

			unaVista->crearPersonajes();

			/***************************************************************************/
			/*     GAMELOOP															   */
			/***************************************************************************/
			Parser::getInstancia().getPelea()->iniciarPelea();
			unaVista->resetContadorLogoPelea();
			menu->getModo()->iniciarModo();
			while (true) {

				int estadoUno = controladorUno->cambiar();
				int estadoDos = controladorDos->cambiar();

				if (estadoUno == REINICIAR || estadoDos == REINICIAR){
					accion = REINICIAR;
					break;
				}

				if ((estadoUno == REINICIAR_PELEA && menu->getModo()->esModoPractica()) || (estadoDos == REINICIAR_PELEA && menu->getModo()->esModoPractica())){
					unaVista->resetContadorLogoPelea();
					unMundo->reiniciar();
					unaVista->reiniciarCamara();
				}

				else if (estadoUno == FIN || estadoDos == FIN){
					accion = FIN;
					break;
				};

				//Se inicializa el Timer de corte
				capTimer.start();

				//Calcula y corrije cuadros por segundo
				float avgFPS = conteoDeCuadros / (fpsTimer.getTicks() / 1000.f);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				//Se actualiza la pantalla
				int estadoVida = unMundo->Paso(0.13f);
				unaVista->actualizar();

				if (estadoVida == REINICIAR || Parser::getInstancia().getPelea()->terminoLaPelea()){
					if (Parser::getInstancia().getPelea()->getPersonajeGanador() != nullptr){
						Log::getInstancia().logearMensajeEnModo("Gano personaje " + Parser::getInstancia().getPelea()->getPersonajeGanador()->getNombreActual(), Log::MODO_DEBUG);
					}
					else Log::getInstancia().logearMensajeEnModo("Pelea empatada", Log::MODO_DEBUG);
					unaVista->resetContadorLogoPelea();
					unMundo->reiniciar();
					unaVista->reiniciarCamara();
					unaVista->reiniciarMenu();
					accion = INICIAR;
					break;
				}
				if (estadoVida == CAMBIAR_ROUND){
					if (!Parser::getInstancia().getPelea()->getRoundActual()->getPersonajeGanador()){
						std::stringstream stream;
						stream << Parser::getInstancia().getPelea()->getRoundActual()->getNumeroDeRound();
						Log::getInstancia().logearMensajeEnModo("Round " + stream.str() + " empatado", Log::MODO_DEBUG);
					}
					unaVista->resetContadorLogoPelea();
					unMundo->reiniciar();
					unaVista->reiniciarCamara();
					Parser::getInstancia().getPelea()->empezarRound();

				}
				else if (estadoVida == FIN){
					accion = FIN;
					break;
				};

				++conteoDeCuadros;

				//Si el cuadro finalizó antes
				int frameTicks = capTimer.getTicks();
				if (frameTicks < MSxCUADRO)
				{
					//Se espera el tiempo que queda
					SDL_Delay((Uint32)round(MSxCUADRO - frameTicks));
				}



			}
			// Se eliminan variables del juego y se libera la memoria
			capTimer.stop();
			fpsTimer.stop();
			Parser::getInstancia().getPelea()->terminarPelea();
			delete unCuerpo;
			delete otroCuerpo;
			delete menu;
		}
		delete controladorUno;
		delete controladorDos;
		delete unaVista;
		delete unMundo;
		Parser::FreeInstancia();
		//Cierro el sonido
		Sonido::Cerrar();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}
	return EXIT_SUCCESS;
}



