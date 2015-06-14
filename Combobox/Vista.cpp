#include "stdafx.h"
#include <SDL_image.h>
#include "SDL_ttf.h"
#include "Vista.h"
#include "Mundo.h"
#include "Log.h"
#include "MatizColor.h"
#include "Timer.h"
#include <sstream>
#include <iostream>
#include "ConversorAString.h"

Vista::Vista(Mundo* unMundo, bool* error, bool habilitarAceleracionDeHardware)
{	
	*error = false;
	//VIBRACION
	vibraciones = 0;
	retraso = VIBRACIONES;
	vibracion = false;
	estaVibrando = false;

	fuente = TTF_OpenFont("ima/ttf/mkx.ttf",22);
	// Usa filtro anisotropico
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	// Se inicia SDL_image
	IMG_Init(IMG_INIT_PNG);
	ventana = SDL_CreateWindow(TITULO_VENTANA, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Parser::getInstancia().getVentana().getAnchoPx(), Parser::getInstancia().getVentana().getAltoPx(), SDL_WINDOW_SHOWN);
		if (ventana == nullptr){
			std::string mensaje = "SDL_CreateWindow Error: ";
			const char* elError = SDL_GetError();
			mensaje += elError;
			Log::getInstancia().logearMensajeEnModo(mensaje, Log::MODO_ERROR);
			SDL_Quit();
			*error = true;
			return;
		}

		std::string icono(ICONO);
		SDL_Surface* iconoSurf = IMG_Load(icono.c_str());
		SDL_SetWindowIcon(ventana, iconoSurf);

		if (habilitarAceleracionDeHardware)
			renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		else
			renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);

			if (renderer == nullptr){
				SDL_DestroyWindow(ventana);
				std::string mensaje = "SDL_CreateRenderer Error: ";
				const char* elError = SDL_GetError();
				mensaje += elError;
				Log::getInstancia().logearMensajeEnModo(mensaje, Log::MODO_ERROR);
				SDL_Quit();
				*error = true;
				return;
			}			

		for (size_t i = 0; i < Parser::getInstancia().getCapas().size(); i++) 
		{
			std::string imgFondo(Parser::getInstancia().getCapas().at(i)->getImagenFondo());
			SDL_Surface* superficieCapa = cargarSuperficieOptimizada(imgFondo);
			SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, superficieCapa);
			// se queda con la textura libero la superficie
			SDL_FreeSurface(superficieCapa);

			if (tex == nullptr){				
				std::string mensaje = "SDL_CreateTextureFromSurface Error: ";
				const char* elError = SDL_GetError();
				mensaje += elError;
				Log::getInstancia().logearMensajeEnModo(mensaje, Log::MODO_ERROR);
				*error = true;
				return;
			}

			Parser::getInstancia().getCapas().at(i)->setTexturaSDL(tex);			
		}

		// inicializo la camara en el centro del escenario
		camaraXLog = -Parser::getInstancia().getEscenario().getAncho()/2
			+ Parser::getInstancia().getVentana().getAncho()/2;

		//Creo el número de cuadros en 0,
		numeroDeCuadroUno = 0;
		//y la lista de cuadros puntero a null
		listaDeCuadrosUno = nullptr;

		//Creo el número de cuadros en 0,
		numeroDeCuadroDos = 0;
		//y la lista de cuadros puntero a null
		listaDeCuadrosDos = nullptr;

		capasVista = Parser::getInstancia().getCapas();
		//Ordeno las capas por su zindex para ser dibujadas
		OrdenarCapas();

		// Se crea textura para dibujar sensores
		SDL_Surface* sup = cargarSuperficieOptimizada("ima/bkg/texturaVerde.png");
		texturaVerde = SDL_CreateTextureFromSurface(renderer, sup);
		SDL_FreeSurface(sup);

		//Textura para la barra de vida
		superficieBarraDeVida = cargarSuperficieOptimizada("ima/bkg/barraDeVida.png");
		texturaBarraDeVida = SDL_CreateTextureFromSurface(renderer, superficieBarraDeVida);
		
		//Textura para la barra de vida roja
		superficieBarraDeVidaRoja = cargarSuperficieOptimizada("ima/bkg/barraDeVidaRoja.png");
		texturaBarraDeVidaRoja = SDL_CreateTextureFromSurface(renderer, superficieBarraDeVidaRoja);

		double anchoBarraDeVida = Parser::getInstancia().getVentana().getAnchoPx() / 2.2;
		int altoBarraDeVida = 12;

		double posXBarraDeVida1 = (Parser::getInstancia().getVentana().getAnchoPx() / 2) - anchoBarraDeVida - 10;
		int posXBarraDeVida2 = Parser::getInstancia().getVentana().getAnchoPx() / 2 + 10;
		
		int posYBarraDeVida = 10;
		int posYBarraDeVidaRoja = 40;

		barraDeVidaImagen1.x = 0;
		barraDeVidaImagen1.y = 0;
		barraDeVidaImagen1.w = superficieBarraDeVida->w;
		barraDeVidaImagen1.h = superficieBarraDeVida->h;
		
		barraRojaDeVidaImagen1.x = 0;
		barraRojaDeVidaImagen1.y = 0;
		barraRojaDeVidaImagen1.w = superficieBarraDeVidaRoja->w;
		barraRojaDeVidaImagen1.h = superficieBarraDeVidaRoja->h;

		SDL_FreeSurface(superficieBarraDeVida);
		SDL_FreeSurface(superficieBarraDeVidaRoja);

		barraDeVidaImagen2 = barraDeVidaImagen1;
		barraRojaDeVidaImagen2 = barraRojaDeVidaImagen1;

		anchoBarraDeVida1 = anchoBarraDeVida;
		anchoBarraDeVida2 = anchoBarraDeVida;

		anchoImagenBarraDeVida = barraDeVidaImagen1.w;

		anchoAnteriorBarra1 = anchoBarraDeVida;
		anchoAnteriorBarra2 = anchoBarraDeVida;

		anchoAnteriorBarraImagen1 = anchoImagenBarraDeVida;
		anchoAnteriorBarraImagen2 = anchoImagenBarraDeVida;

		posBarraDeVida1 = posXBarraDeVida1;
		//Carga de barras de vida
		barraDeVida1 = { posXBarraDeVida1, posYBarraDeVida, anchoBarraDeVida, altoBarraDeVida };
		barraDeVida2 = { posXBarraDeVida2, posYBarraDeVida, anchoBarraDeVida, altoBarraDeVida };

		barraRojaDeVida1 = { posXBarraDeVida1, posYBarraDeVida, anchoBarraDeVida, altoBarraDeVida };
		barraRojaDeVida2 = { posXBarraDeVida2, posYBarraDeVida, anchoBarraDeVida, altoBarraDeVida };

		AlfaInicial = 200;
		int nada;
		AlfaAnterior = 1;
		estadoAnteriorPj1.movimiento = PARADO;
		estadoAnteriorPj2.movimiento = PARADO;
		refMundo = unMundo;

		// Textura elección modo de juego
		modoJuego = IMG_Load("ima/bkg/selectmodeofplay.png");
		juegoModo = SDL_CreateTextureFromSurface(renderer, modoJuego);
		int anchoVentanaPx = Parser::getInstancia().getVentana().getAnchoPx();
		int altoVentanaPx = Parser::getInstancia().getVentana().getAltoPx();
		rectanguloVentana = { 0, 0, anchoVentanaPx, altoVentanaPx };
		//Opciones modo de juego
		opcion = 0;
		opcionTorre = 0;
		opcionPersonaje = 11;
		opcionPj1 = 11;
		opcionPj2 = 22;
		opcion1Seleccionada = false;
		opcion2Seleccionada = false;

		// Textura Seleccion Personajes
		SDL_Surface* supPjs = IMG_Load("ima/bkg/seleccionPersonaje.png");
		texturaSeleccionPersonajes = SDL_CreateTextureFromSurface(renderer, supPjs);
		SDL_FreeSurface(supPjs);

		// Textura Seleccion Pj1
		SDL_Surface* supPj1 = IMG_Load("ima/bkg/cuadPj1.png");
		texturaSeleccionPj1 = SDL_CreateTextureFromSurface(renderer, supPj1);
		SDL_FreeSurface(supPj1);

		// Textura Seleccion Pj2
		SDL_Surface* supPj2 = IMG_Load("ima/bkg/cuadPj2.png");
		texturaSeleccionPj2 = SDL_CreateTextureFromSurface(renderer, supPj2);
		SDL_FreeSurface(supPj2);
		cambiarColor = false;

		// Textura fondo torres
		SDL_Surface* supfondoTorre = IMG_Load("ima/bkg/nocheOscura.png");
		texturaFondoTorres = SDL_CreateTextureFromSurface(renderer, supfondoTorre);
		SDL_FreeSurface(supfondoTorre);

		// Textura Novato
		SDL_Surface* supbebe = IMG_Load("ima/bkg/d_bebebubu.png");
		texturabebebubu = SDL_CreateTextureFromSurface(renderer, supbebe);
		SDL_FreeSurface(supbebe);

		// Textura Medio
		SDL_Surface* supMedio = IMG_Load("ima/bkg/d_medio.png");
		texturaMedio = SDL_CreateTextureFromSurface(renderer, supMedio);
		SDL_FreeSurface(supMedio);

		// Textura Experimentado
		SDL_Surface* supExperimentado = IMG_Load("ima/bkg/d_Experimentado.png");
		texturaExperimentado = SDL_CreateTextureFromSurface(renderer, supExperimentado);
		SDL_FreeSurface(supExperimentado);

		// Textura Experto
		SDL_Surface* supExperto = IMG_Load("ima/bkg/d_Experto.png");
		texturaExperto = SDL_CreateTextureFromSurface(renderer, supExperto);
		SDL_FreeSurface(supExperto);

		// Textura muestras
		SDL_Surface* supMuestras = IMG_Load("ima/bkg/muestras.png");
		personajesMuestra = SDL_CreateTextureFromSurface(renderer, supMuestras);
		SDL_FreeSurface(supMuestras);

		muestra1 = { 212, 195, 63, 129 };
		muestra2 = { 528, 195, 63, 129 };

		rectDificultad = ajusteResolucionBase800x600(278, 15, 200, 200);

		//textura 
		texto = "Ingrese Nombre: ";
		nombreTextoPj1 = " ";
		nombreTextoPj2 = " ";

		textoIngresado = " ";
		textoIngresado2 = " ";

		//Carga efectos de pelea
		SDL_Surface* pelea = IMG_Load("ima/bkg/logoPelea.png");
		texturaLogoPelea = SDL_CreateTextureFromSurface(renderer, pelea);
		SDL_FreeSurface(pelea);

		// Se carga contador logo pelea
		contadorLogoPelea = CONTADORLOGOPELEA;

		//Carga  textura de ronda
		SDL_Surface* ronda = IMG_Load("ima/bkg/rondaUno.png");
		SDL_SetColorKey(ronda, SDL_TRUE, SDL_MapRGB(ronda->format, 255, 255, 255));
		texturaRonda = SDL_CreateTextureFromSurface(renderer, ronda);
		SDL_FreeSurface(ronda);

		//Carga de cuadrado redimension para texto de combos
		cuadradoRedimension = ajusteResolucionBase800x600(0, 550, 100, 50);
		textoCombos = "-";

		//Tiempo de permanecia en pantalla de efectos
		this->efectosTimer.start();
		//Menu timer
		this->menuTimer.start();

}

bool Vista::cambiaColorPersonaje(){
	return cambiarColor;
}

void Vista::crearPersonajes(){
	//Se cargan los sprites:		

	//Dirección de la imagen de Sprites
	std::string dirImgPersonajeUno = Parser::getInstancia().getPelea()->getPersonaje1()->getSpriteDir();
	std::string dirImgPersonajeDos = Parser::getInstancia().getPelea()->getPersonaje2()->getSpriteDir();

	//Carga la imagen desde la ruta especificada
	SuperficieUno = IMG_Load(dirImgPersonajeUno.c_str());
	SuperficieDos = IMG_Load(dirImgPersonajeDos.c_str());

	AlfaVida = 200;
	
	if (!MODO_RAPIDO){
		MatizColor matiz(SuperficieUno);
		matiz.desplazarMatiz(colorPj1.at(0), colorPj1.at(1), colorPj1.at(2));

		MatizColor matiz2(SuperficieDos);
		matiz2.desplazarMatiz(colorPj2.at(0), colorPj2.at(1), colorPj2.at(2));
	}

	//Creación de la textura sobre la superficie
	texturaSpriteUno = SDL_CreateTextureFromSurface(renderer, SuperficieUno);
	texturaSpriteDos = SDL_CreateTextureFromSurface(renderer, SuperficieDos);
}

void Vista::setColorPj1(std::vector<double> unColor)
{
	colorPj1 = unColor;
}

void Vista::setColorPj2(std::vector<double> unColor)
{
	colorPj2 = unColor;
}


void Vista::reiniciarCamara(){
	camaraXLog = -Parser::getInstancia().getEscenario().getAncho() / 2
		+ Parser::getInstancia().getVentana().getAncho() / 2;
}

void Vista::reiniciarMenu(){
	int anchoVentanaPx = Parser::getInstancia().getVentana().getAnchoPx();
	int altoVentanaPx = Parser::getInstancia().getVentana().getAltoPx();
	rectanguloVentana = { 0, 0, anchoVentanaPx, altoVentanaPx };
	//Opciones modo de juego
	opcion = 0;
	opcionTorre = 0;
	opcionPersonaje = 11;
	opcionPj1 = 11;
	opcionPj2 = 22;
	opcion1Seleccionada = false;
	opcion2Seleccionada = false;

	// Textura Seleccion Personajes
	SDL_DestroyTexture(texturaSeleccionPersonajes);
	SDL_Surface* supPjs = IMG_Load("ima/bkg/seleccionPersonaje.png");
	texturaSeleccionPersonajes = SDL_CreateTextureFromSurface(renderer, supPjs);
	SDL_FreeSurface(supPjs);

	// Textura Seleccion Pj1
	SDL_DestroyTexture(texturaSeleccionPj1);
	SDL_Surface* supPj1 = IMG_Load("ima/bkg/cuadPj1.png");
	texturaSeleccionPj1 = SDL_CreateTextureFromSurface(renderer, supPj1);
	SDL_FreeSurface(supPj1);

	// Textura Seleccion Pj2
	SDL_DestroyTexture(texturaSeleccionPj2);
	SDL_Surface* supPj2 = IMG_Load("ima/bkg/cuadPj2.png");
	texturaSeleccionPj2 = SDL_CreateTextureFromSurface(renderer, supPj2);
	SDL_FreeSurface(supPj2);
	cambiarColor = false;

	// Textura fondo torres
	SDL_DestroyTexture(texturaFondoTorres);
	SDL_Surface* supfondoTorre = IMG_Load("ima/bkg/nocheOscura.png");
	texturaFondoTorres = SDL_CreateTextureFromSurface(renderer, supfondoTorre);
	SDL_FreeSurface(supfondoTorre);

	// Textura Novato
	SDL_DestroyTexture(texturabebebubu);
	SDL_Surface* supbebe = IMG_Load("ima/bkg/d_bebebubu.png");
	texturabebebubu = SDL_CreateTextureFromSurface(renderer, supbebe);
	SDL_FreeSurface(supbebe);

	// Textura Medio
	SDL_DestroyTexture(texturaMedio);
	SDL_Surface* supMedio = IMG_Load("ima/bkg/d_medio.png");
	texturaMedio = SDL_CreateTextureFromSurface(renderer, supMedio);
	SDL_FreeSurface(supMedio);

	// Textura Experimentado
	SDL_DestroyTexture(texturaExperimentado);
	SDL_Surface* supExperimentado = IMG_Load("ima/bkg/d_Experimentado.png");
	texturaExperimentado = SDL_CreateTextureFromSurface(renderer, supExperimentado);
	SDL_FreeSurface(supExperimentado);

	// Textura Experto
	SDL_DestroyTexture(texturaExperto);
	SDL_Surface* supExperto = IMG_Load("ima/bkg/d_Experto.png");
	texturaExperto = SDL_CreateTextureFromSurface(renderer, supExperto);
	SDL_FreeSurface(supExperto);

	// Textura muestras
	SDL_DestroyTexture(personajesMuestra);
	SDL_Surface* supMuestras = IMG_Load("ima/bkg/muestras.png");
	personajesMuestra = SDL_CreateTextureFromSurface(renderer, supMuestras);
	SDL_FreeSurface(supMuestras);

	muestra1 = { 212, 195, 63, 129 };
	muestra2 = { 528, 195, 63, 129 };

	rectDificultad = ajusteResolucionBase800x600(278, 15, 200, 200);

	//textura 
	texto = "Ingrese Nombre: ";
	nombreTextoPj1 = " ";
	nombreTextoPj2 = " ";

	textoIngresado = " ";
	textoIngresado2 = " ";

}

void Vista::actualizar(){

	// referencias más utilizadas al actualizar la vista
	std::vector<Personaje*> personajesVista = std::vector<Personaje*>();
	if (!personajesVista.empty()) personajesVista.clear();
	personajesVista.push_back(Parser::getInstancia().getPelea()->getPersonaje1());
	personajesVista.push_back(Parser::getInstancia().getPelea()->getPersonaje2());
	Ventana ventanaVista = Parser::getInstancia().getVentana();
	


	//Parametros de la ventana

	float anchoVentana = ventanaVista.getAncho();
	int altoVentanaPx = ventanaVista.getAltoPx();

	Personaje* personajeUno = personajesVista[0];
	Personaje* personajeDos = personajesVista[1];

	//Parametros del personaje	
	xPjUno = personajeUno->getPosicionUn().first;
	float anchoPjUno = personajesVista[0]->getAncho();
	xPjDos = personajeDos->getPosicionUn().first;
	float anchoPjDos = personajesVista[1]->getAncho();

	float anchoEscenario = Parser::getInstancia().getEscenario().getAncho();


	//Se limpia la pantalla
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	float xSensPos1, xSensPos2;
	xSensPos1 = 0;
	xSensPos2 = 0;
	int anchoSens1 = refMundo->getCuerpo(0)->getSensorLargo(&xSensPos1);
	int anchoSens2 = refMundo->getCuerpo(1)->getSensorLargo(&xSensPos2);	

	// condicion de borde Personaje Uno
	bool PjUnoEstaEnBordeIzq = false;
	bool PjUnoEstaEnBordeDer = false;
	if (xPjUno + manejadorULog.darLongUnidades(xSensPos1 + (anchoSens1)) >= anchoVentana - camaraXLog)
		PjUnoEstaEnBordeDer = true;
	if (xPjUno + manejadorULog.darLongUnidades(xSensPos1) < -camaraXLog)
		PjUnoEstaEnBordeIzq = true;
	bool PjUnoEstaEnBorde = PjUnoEstaEnBordeIzq || PjUnoEstaEnBordeDer;

	// condicion de borde Personaje Dos
	bool PjDosEstaEnBordeIzq = false;
	bool PjDosEstaEnBordeDer = false;
	if (xPjDos + manejadorULog.darLongUnidades(xSensPos2 + (anchoSens2)) >= anchoVentana - camaraXLog)
		PjDosEstaEnBordeDer = true;
	if (xPjDos+ manejadorULog.darLongUnidades(xSensPos2)< -camaraXLog)
		PjDosEstaEnBordeIzq = true;
	bool PjDosEstaEnBorde = PjDosEstaEnBordeIzq || PjDosEstaEnBordeDer;

	MOV_TIPO mov1 = refMundo->getCuerpo(0)->getControlador()->getMovimientos().back()->getMovimiento();
	MOV_TIPO mov2 = refMundo->getCuerpo(1)->getControlador()->getMovimientos().back()->getMovimiento();
	ESTADO estadoPj1 = refMundo->getCuerpo(0)->getEstado();
	ESTADO estadoPj2 = refMundo->getCuerpo(1)->getEstado();



	if ((PjUnoEstaEnBordeIzq && PjDosEstaEnBordeDer) || (PjDosEstaEnBordeIzq && PjUnoEstaEnBordeDer)) {
		refMundo->FrenarCuerpos();
	}

	if (PjUnoEstaEnBordeIzq && (mov1 == DER))
		refMundo->LiberarCuerpos();

	if (PjUnoEstaEnBordeDer && (mov1 == IZQ))
		refMundo->LiberarCuerpos();

	if (PjDosEstaEnBordeIzq && (mov2 == DER))
		refMundo->LiberarCuerpos();

	if (PjDosEstaEnBordeDer && (mov2 == IZQ))
		refMundo->LiberarCuerpos();

	if (PjUnoEstaEnBordeIzq && !(refMundo->getCuerpo(0)->EstaFrenado()))
		if (personajesVista[0]->getDeltaX() > 0)
			camaraXLog += personajesVista[0]->getDeltaX();
	if (PjDosEstaEnBordeIzq && !(refMundo->getCuerpo(1)->EstaFrenado()))
		if (personajesVista[1]->getDeltaX() > 0)
			camaraXLog += personajesVista[1]->getDeltaX();
	if (PjUnoEstaEnBordeDer && !(refMundo->getCuerpo(0)->EstaFrenado()))
		if (personajesVista[0]->getDeltaX() < 0)
			camaraXLog += personajesVista[0]->getDeltaX();
	if (PjDosEstaEnBordeDer && !(refMundo->getCuerpo(1)->EstaFrenado()))
		if (personajesVista[1]->getDeltaX() < 0)
			camaraXLog += personajesVista[1]->getDeltaX();



	// la camara queda contenida dentro del escenario
	if (camaraXLog > 0) camaraXLog = 0;
	if (camaraXLog < -anchoEscenario + anchoVentana) camaraXLog = -anchoEscenario + anchoVentana;
	//camaraXLog += personajesVista[0]->getDeltaX();

	//Vibracion de la camara
	if (vibracion){
		bool golpeado = false;
		/*for (int i = 0; i < personajesVista.size(); i++){
			if ((personajesVista.at(i)->getEstado().accion == PATADA_ALTA) && ){
				golpeado = true;
				break;
			}
		}*/
		//Este es el codigo que deberia ir en realidad pero todavia no hay gancho
		if (personajesVista.at(0)->getEstado().golpeado == GOLPEADO && personajesVista.at(0)->getEstado().accion != GUARDIA){
			if (personajesVista.at(1)->getEstado().accion == GANCHO){
				golpeado = true;
			}
		}
		else{
			if (personajesVista.at(1)->getEstado().golpeado == GOLPEADO && personajesVista.at(1)->getEstado().accion != GUARDIA){
				if (personajesVista.at(0)->getEstado().accion == GANCHO){
				golpeado = true;
				}
			}
		}
		
		if ((personajesVista.at(0)->getEstado().golpeado == GOLPEADO) || (personajesVista.at(1)->getEstado().golpeado == GOLPEADO)){
			AlfaInicial = AlfaInicial - AlfaAnterior;
			if (AlfaInicial < 128){
				AlfaInicial = 200;
			}
			alfa(AlfaInicial);
		}
		else
			alfa(128);
/*		if (golpeado) vibraciones++;
		else vibraciones = 0;*/

		if (golpeado){
			retraso--;
			if (retraso == 0){
				retraso = VIBRACIONES;
				if (!estaVibrando){
					camaraXLog = camaraXLog + 5;
					estaVibrando = true;
				}
				else{
					estaVibrando = false;
					camaraXLog = camaraXLog - 5;
				}
			}
		}
	}
	

	if (!PjUnoEstaEnBorde && !PjDosEstaEnBorde)
		refMundo->LiberarCuerpos();

	//Dibujar menu 

	int anchoVentanaPx = ventanaVista.getAnchoPx();
	/*
	if ((this->menuTimer.getTicks() >= 50) && (this->menuTimer.getTicks() <= 1000))
	{
		this->dibujarMenu(anchoVentana, anchoVentanaPx, altoVentanaPx, anchoEscenario);
		SDL_RenderPresent(renderer);
	}
	else
	{
		if (this->menuTimer.getTicks() > 1000)
		{

			SDL_DestroyTexture(this->texturaMenu);
			this->menuTimer.stop();
		}
		else
		{
			*/
			// Dibuja las capas y el personaje
			Dibujar(personajesVista);

			//Dibuja las barras de vida
			DibujarBarrasDeVida(personajesVista);

			//Dibujar input del controlador si es modo 
			if (modoActual==PRACTICA)
			DibujarInput(refMundo->getCuerpo(0)->getControlador());

			//Dibujar reloj
			DibujarReloj();

			//Se actualiza la pantalla
			SDL_RenderPresent(renderer);
}

void Vista::cargarTexto(std::string texto, SDL_Color colorDelTexto)
{
	supTexto = TTF_RenderText_Blended(fuente, texto.c_str(), colorDelTexto);
	anchoTexto = supTexto->w;
	altoTexto = supTexto->h;
	texturaTexto = SDL_CreateTextureFromSurface(renderer, supTexto);
	SDL_FreeSurface(supTexto);
}

void Vista::dibujarTexto(SDL_Rect cuadradoDestino, Uint8 Alfa){
	SDL_SetTextureAlphaMod(texturaTexto, Alfa);
	SDL_SetRenderTarget(renderer, texturaTexto);
	SDL_RenderCopy(renderer, texturaTexto, NULL, &cuadradoDestino);
}

bool estaEnRectangulo(SDL_Rect rectangulo, EventoDeMouse *unEventoDeMouse){

	if (((unEventoDeMouse->getX() >= rectangulo.x) && (unEventoDeMouse->getX() <= (rectangulo.x + rectangulo.w))) && ((unEventoDeMouse->getY() >= rectangulo.y) && (unEventoDeMouse->getY() <= (rectangulo.y + rectangulo.h))))
		return true;
		return false;
}

std::string Vista::entradaTexto(SDL_Rect rectanguloDestino, std::string textoPersonaje){

	std::string textoADibujar;
	
	//Color del Texto
	SDL_Color colorDelTexto = { 255, 255, 255 };

	SDL_PumpEvents();

	//Habilitar entrada de texto
	SDL_StartTextInput();

	//manejador de Eventos
	SDL_Event e;

	//Color del texto en blanco
	SDL_Color colorTexto = { 0xFF, 0xFF, 0xFF, 0xFF };

	bool salir = false;

	//Carga del texto
	SDL_Rect rectTexto = ajusteResolucionBase800x600(0, 0, 100, 20);
	rectTexto.x = rectanguloDestino.x;
	rectTexto.y = rectanguloDestino.y - 10;

	while (!salir){
		//Se limpia la pantalla
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

		//Se dibujan el fondo y los personajes a seleccionar

		//Se dibuja el fondo
		SDL_SetRenderTarget(renderer, juegoModo);
		SDL_RenderCopy(renderer, juegoModo, NULL, &rectanguloVentana);

		//Se dibujan los personajes a elegir
		SDL_SetRenderTarget(renderer, texturaSeleccionPersonajes);
		SDL_RenderCopy(renderer, texturaSeleccionPersonajes, NULL, NULL);
		
		//Se dibujan recuadros de seleccion de personajes
		SDL_RenderCopy(renderer, texturaSeleccionPj1, NULL, &ima1elegida);
		SDL_RenderCopy(renderer, texturaSeleccionPj2, NULL, &ima2elegida);

		//Se dibujan muestras de personajes
		SDL_RenderCopy(renderer, personajesMuestra, &imaColor1elegida, &muestra1);
		SDL_RenderCopyEx(renderer, personajesMuestra, &imaColor2elegida, &muestra2, NULL, NULL, SDL_FLIP_HORIZONTAL);

		//alfa transparente
		Uint8 alfaSeleccion = 230;
		Uint8 pColor = alfaSeleccion;

		//Ver que dibujar
		if (!opcion1Seleccionada){
			cargarTexto("P1", colorDelTexto);
			dibujarTexto(texto1elegido, pColor);
		}
		else{
			cargarTexto(textoIngresado, colorDelTexto);
			texto1elegido.x = ima1elegida.x;
			texto1elegido.w = ima1elegida.w;
			dibujarTexto(texto1elegido, 240);
		}

		//Ver que dibujar
		if (!opcion2Seleccionada){
			cargarTexto("P2", colorDelTexto);
			dibujarTexto(texto2elegido, pColor);
		}
		else{
			cargarTexto(textoIngresado2, colorDelTexto);
			texto2elegido.x = ima2elegida.x;
			texto2elegido.w = ima2elegida.w;
			dibujarTexto(texto2elegido, 240);
		}

		cargarTexto(texto, colorTexto);

		dibujarTexto(rectTexto, 240);

		

	//El flag de dibujado
	bool flagDibujado = false;

	//manejar los eventos en una cola
	while (SDL_PollEvent(&e) != 0)
	{
		//el usuario solicita salir
		if ((e.type == SDL_QUIT) || (e.key.keysym.sym == SDLK_RETURN) || (e.key.keysym.sym == SDLK_KP_ENTER))
		{
			return textoPersonaje;
		}
		//Entrada especial de texto
		else if (e.type == SDL_KEYDOWN)
		{
			//manejar el Retroceso
			if (e.key.keysym.sym == SDLK_BACKSPACE && textoPersonaje.length() > 0)
			{
				textoPersonaje.pop_back();
				flagDibujado = true;
			}
			//Manejo de la copia
			else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
			{
				SDL_SetClipboardText(textoPersonaje.c_str());
			}
			//Manejar el pegado
			else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
			{
				textoPersonaje += SDL_GetClipboardText();
				flagDibujado = true;
			}
		}
		//Entrada especial de texto
		else if (e.type == SDL_TEXTINPUT)
		{
			//Ni copiar ni pegar
			if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
			{
				//Concatenar caracteres
				textoPersonaje += e.text.text;
				flagDibujado = true;
			}
		}
	}

	textoADibujar = textoPersonaje;
	if (textoPersonaje.size() > 10){
		textoADibujar.erase(0, 1);
		if (textoADibujar.size() > 10)
			textoADibujar.erase(0, textoADibujar.size()-10);
	}
	
	cargarTexto(textoADibujar, colorTexto);

	//Dibuja el texto en caso de ser necesario
	if (flagDibujado)
	{
		//Texto no esta vacío
		if (textoPersonaje != "")
		{
			//Dibujar nuevo texto
			cargarTexto(textoADibujar, colorTexto);
		}
		//Texto esta vacio
		else
		{
			//Dibujar Textura de espacio
			cargarTexto(" ", colorTexto);
		}
	}


	//Dibujar textura del texto
	rectanguloDestino.w = anchoTexto;
	rectanguloDestino.h = altoTexto;
	dibujarTexto(rectanguloDestino, 255);
	
	SDL_SetRenderTarget(renderer, NULL);
	//Se actualiza la pantalla
	SDL_RenderPresent(renderer);
	}
		//Deshabilitar Input
		SDL_StopTextInput();
		return textoPersonaje;

}

void Vista::setModoActual(MODOS_DE_JUEGO unModoActual){
	modoActual = unModoActual;
}

PJSELECCION Vista::elegirPersonajes(Controlador* unControlador, PJSELECCION personajes, EventoDeMouse *unEventoDeMouse, std::string &nombreDelP1, std::string &nombreDelP2){

	MOV_TIPO mov1 = unControlador->getMovimientos().back()->getMovimiento();

	personajes.seleccionados = NO;
	bool cliequeoEnCuadrado = false;

	//Se limpia la pantalla
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Se dibujan el fondo y los personajes a seleccionar

	//Se dibuja el fondo
	SDL_SetRenderTarget(renderer, juegoModo);
	SDL_RenderCopy(renderer, juegoModo, NULL, &rectanguloVentana);

	//Se dibujan los personajes a elegir
	SDL_SetRenderTarget(renderer, texturaSeleccionPersonajes);
	SDL_RenderCopy(renderer, texturaSeleccionPersonajes, NULL, NULL);

	//alfa transparente
	Uint8 alfaSeleccion = 230;
	Uint8 pColor = alfaSeleccion;

	//Cuadrados de los personajes
	SDL_Rect ima_p1 = ajusteResolucionBase800x600(175, 431, 70, 69);
	SDL_Rect ima_p2 = ajusteResolucionBase800x600(254, 431, 70, 69);
	SDL_Rect ima_p3 = ajusteResolucionBase800x600(334, 431, 70, 69);
	SDL_Rect ima_p4 = ajusteResolucionBase800x600(413, 431, 70, 69);
	SDL_Rect ima_p5 = ajusteResolucionBase800x600(492, 431, 70, 69);
	SDL_Rect ima_p6 = ajusteResolucionBase800x600(571, 431, 70, 69);
	SDL_Rect ima_p7 = ajusteResolucionBase800x600(176, 508, 70, 69);
	SDL_Rect ima_p8 = ajusteResolucionBase800x600(254, 508, 70, 69);
	SDL_Rect ima_p9 = ajusteResolucionBase800x600(334, 508, 70, 69);
	SDL_Rect ima_p10 = ajusteResolucionBase800x600(413, 508, 70, 69);
	SDL_Rect ima_p11 = ajusteResolucionBase800x600(492, 508, 70, 69);
	SDL_Rect ima_p12 = ajusteResolucionBase800x600(571, 508, 70, 69);

	//Cuadrados de texto de los personajes
	SDL_Rect texto_p1 = ajusteResolucionBase800x600(203, 479, 16, 20);
	SDL_Rect texto_p2 = ajusteResolucionBase800x600(281, 479, 16, 20);
	SDL_Rect texto_p3 = ajusteResolucionBase800x600(361, 479, 16, 20);
	SDL_Rect texto_p4 = ajusteResolucionBase800x600(440, 479, 16, 20);
	SDL_Rect texto_p5 = ajusteResolucionBase800x600(519, 479, 16, 20);
	SDL_Rect texto_p6 = ajusteResolucionBase800x600(598, 479, 16, 20);
	SDL_Rect texto_p7 = ajusteResolucionBase800x600(202, 557, 16, 20);
	SDL_Rect texto_p8 = ajusteResolucionBase800x600(281, 557, 16, 20);
	SDL_Rect texto_p9 = ajusteResolucionBase800x600(361, 557, 16, 20);
	SDL_Rect texto_p10 = ajusteResolucionBase800x600(440, 557, 16, 20);
	SDL_Rect texto_p11 = ajusteResolucionBase800x600(519, 557, 16, 20);
	SDL_Rect texto_p12 = ajusteResolucionBase800x600(598, 557, 16, 20);

	//Cuadrados muestra Origen
	SDL_Rect imaColor_p1 = { 0, 0, 63, 129 };
	SDL_Rect imaColor_p2 = { 63, 0, 63, 129 };
	SDL_Rect imaColor_p3 = { 126, 0, 63, 129 };
	SDL_Rect imaColor_p4 = { 189, 0, 63, 129 };
	SDL_Rect imaColor_p5 = { 252, 0, 63, 129 };
	SDL_Rect imaColor_p6 = { 315, 0, 63, 129 };
	SDL_Rect imaColor_p7 = { 378, 0, 63, 129 };
	SDL_Rect imaColor_p8 = { 441, 0, 59, 125 };
	SDL_Rect imaColor_p9 = { 500, 0, 59, 125 };
	SDL_Rect imaColor_p10 = { 559, 0, 59, 125 };
	SDL_Rect imaColor_p11 = { 618, 0, 59, 125 };
	SDL_Rect imaColor_p12 = { 677, 0, 59, 125 };
	
	//Color del Texto
	SDL_Color colorDelTexto = { 255, 255, 255 };
	
	if (!opcion2Seleccionada){
		opcionPj2 = rand() % 11 + 11;
		opcion2Seleccionada = true;
		textoIngresado2 = "       P2       ";
	}

	//MOUSE
	if (estaEnRectangulo(ima_p1, unEventoDeMouse)){
		opcionPj1 = SCORPIONROJO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p2, unEventoDeMouse)){
		opcionPj1 = SCORPIONAZUL;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p3, unEventoDeMouse)){
		opcionPj1 = SCORPION_;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p4, unEventoDeMouse)){
		opcionPj1 = SCORPIONVERDE;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p5, unEventoDeMouse)){
		opcionPj1 = SCORPIONVIOLETA;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p6, unEventoDeMouse)){
		opcionPj1 = SCORPIONGRIS;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p7, unEventoDeMouse)){
		opcionPj1 = SCORPIONNARANJA;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p8, unEventoDeMouse)){
		opcionPj1 = LIUKANGAMARILLO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p9, unEventoDeMouse)){
		opcionPj1 = LIUKANGVERDE;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p10, unEventoDeMouse)){
		opcionPj1 = LIUKANG_;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p11, unEventoDeMouse)){
		opcionPj1 = LIUKANGAZUL;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p12, unEventoDeMouse)){
		opcionPj1 = LIUKANGROJO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}

	//TECLADO
	if (!opcion1Seleccionada){
	if (mov1 == DER)
		opcionPj1++;

	if (mov1 == IZQ)
		opcionPj1--;

	if (mov1 == ARRIBA){
		if (opcionPj1>16)
			opcionPj1 -= 6;
	}

	if (mov1 == ABAJO){
		if (!(opcionPj1>16))
			opcionPj1 += 6;
	}

	if (opcionPj1 < 11)
		opcionPj1 = 11;

	if (opcionPj1 > 22)
		opcionPj1 = 22;
	}
	
	cargarTexto("P1", colorDelTexto);
	//Opciones de selección
	switch (opcionPj1){
	case 11:
		imaColor1elegida = imaColor_p1;
		texto1elegido = texto_p1;
		ima1elegida = ima_p1;
		break;
	case 12:
		imaColor1elegida = imaColor_p2;
		texto1elegido = texto_p2;
		ima1elegida = ima_p2;
		break;
	case 13:
		imaColor1elegida = imaColor_p3;
		texto1elegido = texto_p3;
		ima1elegida = ima_p3;
		break;
	case 14:
		imaColor1elegida = imaColor_p4;
		texto1elegido = texto_p4;
		ima1elegida = ima_p4;
		break;
	case 15:
		imaColor1elegida = imaColor_p5;
		texto1elegido = texto_p5;
		ima1elegida = ima_p5;
		break;
	case 16:
		imaColor1elegida = imaColor_p6;
		texto1elegido = texto_p6;
		ima1elegida = ima_p6;
		break;
	case 17:
		imaColor1elegida = imaColor_p7;
		texto1elegido = texto_p7;
		ima1elegida = ima_p7;
		break;
	case 18:
		imaColor1elegida = imaColor_p8;
		texto1elegido = texto_p8;
		ima1elegida = ima_p8;
		break;
	case 19:
		imaColor1elegida = imaColor_p9;
		texto1elegido = texto_p9;
		ima1elegida = ima_p9;
		break;
	case 20:
		imaColor1elegida = imaColor_p10;
		texto1elegido = texto_p10;
		ima1elegida = ima_p10;
		break;
	case 21:
		imaColor1elegida = imaColor_p11;
		texto1elegido = texto_p11;
		ima1elegida = ima_p11;
		break;
	case 22:
		imaColor1elegida = imaColor_p12;
		texto1elegido = texto_p12;
		ima1elegida = ima_p12;
		break;
	default:
		imaColor1elegida = imaColor_p1;
		texto1elegido = texto_p1;
		ima1elegida = ima_p1;
		break;
	}
	//Ver que dibujar
	if (!opcion1Seleccionada){
		dibujarTexto(texto1elegido, pColor);
	}
	else{
		cargarTexto(textoIngresado, colorDelTexto);
		texto1elegido.x = ima1elegida.x;
		texto1elegido.w = ima1elegida.w;
		dibujarTexto(texto1elegido, 240);
	}
	SDL_SetRenderTarget(renderer, texturaSeleccionPj1);
	SDL_RenderCopy(renderer, texturaSeleccionPj1, NULL, &ima1elegida);

	SDL_SetRenderTarget(renderer, personajesMuestra);
	SDL_RenderCopy(renderer, personajesMuestra, &imaColor1elegida, &muestra1);


	//Opciones de selección
	cargarTexto("P2", colorDelTexto);
	switch (opcionPj2){
	case 11:
		imaColor2elegida = imaColor_p1;
		texto2elegido = texto_p1;
		ima2elegida = ima_p1;
		break;
	case 12:
		imaColor2elegida = imaColor_p2;
		texto2elegido = texto_p2;
		ima2elegida = ima_p2;
		break;
	case 13:
		imaColor2elegida = imaColor_p3;
		texto2elegido = texto_p3;
		ima2elegida = ima_p3;
		break;
	case 14:
		imaColor2elegida = imaColor_p4;
		texto2elegido = texto_p4;
		ima2elegida = ima_p4;
		break;
	case 15:
		imaColor2elegida = imaColor_p5;
		texto2elegido = texto_p5;
		ima2elegida = ima_p5;
		break;
	case 16:
		imaColor2elegida = imaColor_p6;
		texto2elegido = texto_p6;
		ima2elegida = ima_p6;
		break;
	case 17:
		imaColor2elegida = imaColor_p7;
		texto2elegido = texto_p7;
		ima2elegida = ima_p7;
		break;
	case 18:
		imaColor2elegida = imaColor_p8;
		texto2elegido = texto_p8;
		ima2elegida = ima_p8;
		break;
	case 19:
		imaColor2elegida = imaColor_p9;
		texto2elegido = texto_p9;
		ima2elegida = ima_p9;
		break;
	case 20:
		imaColor2elegida = imaColor_p10;
		texto2elegido = texto_p10;
		ima2elegida = ima_p10;
		break;
	case 21:
		imaColor2elegida = imaColor_p11;
		texto2elegido = texto_p11;
		ima2elegida = ima_p11;
		break;
	case 22:
		imaColor2elegida = imaColor_p12;
		texto2elegido = texto_p12;
		ima2elegida = ima_p12;
		break;
	default:
		imaColor2elegida = imaColor_p12;
		texto2elegido = texto_p12;
		ima2elegida = ima_p12;
		break;
	}

	dibujarTexto(texto2elegido, pColor);

	//recuadro Pj2
	SDL_SetRenderTarget(renderer, texturaSeleccionPj2);
	SDL_RenderCopy(renderer, texturaSeleccionPj2, NULL, &ima2elegida);
	
	SDL_SetRenderTarget(renderer, personajesMuestra);
	SDL_RenderCopyEx(renderer, personajesMuestra, &imaColor2elegida, &muestra2,NULL,NULL,SDL_FLIP_HORIZONTAL);


	//Se actualiza la pantalla
	SDL_RenderPresent(renderer);

	if ((mov1 == G_BAJO) || (cliequeoEnCuadrado)){ 
		opcion1Seleccionada = true;
		textoIngresado = entradaTexto(texto1elegido, nombreTextoPj1);
	}

	if ((opcion1Seleccionada) && (opcion2Seleccionada)){
		personajes.seleccionados = SI;
		nombreDelP1 =  textoIngresado;
		nombreDelP2 = textoIngresado2;
	}
	if (opcionPj1 == opcionPj2)
		cambiarColor = true;
	else
		cambiarColor = false;
	
	personajes = traducirSeleccion(opcionPj1, opcionPj2, personajes);

	return personajes;
}

PJSELECCION Vista::traducirSeleccion(int opcion1, int opcion2, PJSELECCION personajes){
	
	//Opciones de selección
	switch (opcion1){
	case 11:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = ROJO;
		break;
	case 12:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = AZUL;
		break;
	case 13:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = BASE;
		break;
	case 14:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = VERDE;
		break;
	case 15:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = VIOLETA;
		break;
	case 16:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = GRIS;
		break;
	case 17:
		personajes.personajeUno = SCORPIONUNO;
		personajes.colorPjUno = NARANJA;
		break;
	case 18:
		personajes.personajeUno = LIUKANGUNO;
		personajes.colorPjUno = AMARILLO;
		break;
	case 19:
		personajes.personajeUno = LIUKANGUNO;
		personajes.colorPjUno = VERDE;
		break;
	case 20:
		personajes.personajeUno = LIUKANGUNO;
		personajes.colorPjUno = BASE;
		break;
	case 21:
		personajes.personajeUno = LIUKANGUNO;
		personajes.colorPjUno = AZUL;
		break;
	case 22:
		personajes.personajeUno = LIUKANGUNO;
		personajes.colorPjUno = ROJO;
		break;
	}

	switch (opcion2){
	case 11:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = ROJODOS;
		break;
	case 12:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = AZULDOS;
		break;
	case 13:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = BASEDOS;
		break;
	case 14:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = VERDEDOS;
		break;
	case 15:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = VIOLETADOS;
		break;
	case 16:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = GRISDOS;
		break;
	case 17:
		personajes.personajeDos = SCORPIONDOS;
		personajes.colorPjDos = NARANJADOS;
		break;
	case 18:
		personajes.personajeDos = LIUKANGDOS;
		personajes.colorPjDos = AMARILLODOS;
		break;
	case 19:
		personajes.personajeDos = LIUKANGDOS;
		personajes.colorPjDos = VERDEDOS;
		break;
	case 20:
		personajes.personajeDos = LIUKANGDOS;
		personajes.colorPjDos = BASEDOS;
		break;
	case 21:
		personajes.personajeDos = LIUKANGDOS;
		personajes.colorPjDos = AZULDOS;
		break;
	case 22:
		personajes.personajeDos = LIUKANGDOS;
		personajes.colorPjDos = ROJODOS;
		break;
	}
	return personajes;
}

SDL_Rect Vista::ajusteResolucionBase800x600(int x, int y, int anchoPx, int altoPx){

	int posX, posY, ancho, alto;

	//Parametros de la ventana
	int anchoVentanaPx = Parser::getInstancia().getVentana().getAnchoPx();
	int altoVentanaPx = Parser::getInstancia().getVentana().getAltoPx();

	posX = (x / (float)800)*anchoVentanaPx;
	posY = (y / (float)600)*altoVentanaPx;
	ancho = (anchoPx / (float)800)*anchoVentanaPx;
	alto = (altoPx / (float)600)*altoVentanaPx;

	SDL_Rect rectanguloResultado = { posX, posY, ancho, alto };
	return rectanguloResultado;
}

PJSELECCION Vista::elegirPersonajes(Controlador* unControlador, Controlador* otroControlador, PJSELECCION personajes, EventoDeMouse *unEventoDeMouse, std::string &nombreDelP1, std::string &nombreDelP2){
	MOV_TIPO mov1 = unControlador->getMovimientos().back()->getMovimiento();
	MOV_TIPO mov2 = otroControlador->getMovimientos().back()->getMovimiento();

	personajes.seleccionados = NO;
	bool cliequeoEnCuadrado = false;
	//Se limpia la pantalla
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Se dibujan el fondo y los personajes a seleccionar

	//Se dibuja el fondo
	SDL_SetRenderTarget(renderer, juegoModo);
	SDL_RenderCopy(renderer, juegoModo, NULL, &rectanguloVentana);

	//Se dibujan los personajes a elegir
	SDL_SetRenderTarget(renderer, texturaSeleccionPersonajes);
	SDL_RenderCopy(renderer, texturaSeleccionPersonajes, NULL, NULL);

	//alfa transparente
	Uint8 alfaSeleccion = 230;
	Uint8 pColor = alfaSeleccion;
		
	//Cuadrados de los personajes
	SDL_Rect ima_p1 = ajusteResolucionBase800x600(175, 431, 70, 69);
	SDL_Rect ima_p2 = ajusteResolucionBase800x600(254, 431, 70, 69);
	SDL_Rect ima_p3 = ajusteResolucionBase800x600(334, 431, 70, 69);
	SDL_Rect ima_p4 = ajusteResolucionBase800x600(413, 431, 70, 69);
	SDL_Rect ima_p5 = ajusteResolucionBase800x600(492, 431, 70, 69);
	SDL_Rect ima_p6 = ajusteResolucionBase800x600(571, 431, 70, 69);
	SDL_Rect ima_p7 = ajusteResolucionBase800x600(176, 508, 70, 69);
	SDL_Rect ima_p8 = ajusteResolucionBase800x600(254, 508, 70, 69);
	SDL_Rect ima_p9 = ajusteResolucionBase800x600(334, 508, 70, 69);
	SDL_Rect ima_p10 = ajusteResolucionBase800x600(413, 508, 70, 69);
	SDL_Rect ima_p11 = ajusteResolucionBase800x600(492, 508, 70, 69);
	SDL_Rect ima_p12 = ajusteResolucionBase800x600(571, 508, 70, 69);

	//Cuadrados de texto de los personajes
	SDL_Rect texto_p1 = ajusteResolucionBase800x600(203, 479, 16, 20);
	SDL_Rect texto_p2 = ajusteResolucionBase800x600(281, 479, 16, 20);
	SDL_Rect texto_p3 = ajusteResolucionBase800x600(361, 479, 16, 20);
	SDL_Rect texto_p4 = ajusteResolucionBase800x600(440, 479, 16, 20);
	SDL_Rect texto_p5 = ajusteResolucionBase800x600(519, 479, 16, 20);
	SDL_Rect texto_p6 = ajusteResolucionBase800x600(598, 479, 16, 20);
	SDL_Rect texto_p7 = ajusteResolucionBase800x600(202, 557, 16, 20);
	SDL_Rect texto_p8 = ajusteResolucionBase800x600(281, 557, 16, 20);
	SDL_Rect texto_p9 = ajusteResolucionBase800x600(361, 557, 16, 20);
	SDL_Rect texto_p10 = ajusteResolucionBase800x600(440, 557, 16, 20);
	SDL_Rect texto_p11 = ajusteResolucionBase800x600(519, 557, 16, 20);
	SDL_Rect texto_p12 = ajusteResolucionBase800x600(598, 557, 16, 20);

	//Cuadrados muestra Origen
	SDL_Rect imaColor_p1 = { 0, 0, 63, 129 };
	SDL_Rect imaColor_p2 = { 63, 0, 63, 129 };
	SDL_Rect imaColor_p3 = { 126, 0, 63, 129 };
	SDL_Rect imaColor_p4 = { 189, 0, 63, 129 };
	SDL_Rect imaColor_p5 = { 252, 0, 63, 129 };
	SDL_Rect imaColor_p6 = { 315, 0, 63, 129 };
	SDL_Rect imaColor_p7 = { 378, 0, 63, 129 };
	SDL_Rect imaColor_p8 = { 441, 0, 59, 125 };
	SDL_Rect imaColor_p9 = { 500, 0, 59, 125 };
	SDL_Rect imaColor_p10 = { 559, 0, 59, 125 };
	SDL_Rect imaColor_p11 = { 618, 0, 59, 125 };
	SDL_Rect imaColor_p12 = { 677, 0, 59, 125 };
	
	//Color del Texto
	SDL_Color colorDelTexto = { 255, 255, 255 };

	//Control del puntero del mouse
	if (!opcion2Seleccionada){
	if (estaEnRectangulo(ima_p1, unEventoDeMouse)){
		opcionPj2 = SCORPIONROJO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p2, unEventoDeMouse)){
		opcionPj2 = SCORPIONAZUL;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p3, unEventoDeMouse)){
		opcionPj2 = SCORPION_;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p4, unEventoDeMouse)){
		opcionPj2 = SCORPIONVERDE;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p5, unEventoDeMouse)){
		opcionPj2 = SCORPIONVIOLETA;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p6, unEventoDeMouse)){
		opcionPj2 = SCORPIONGRIS;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p7, unEventoDeMouse)){
		opcionPj2 = SCORPIONNARANJA;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p8, unEventoDeMouse)){
		opcionPj2 = LIUKANGAMARILLO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p9, unEventoDeMouse)){
		opcionPj2 = LIUKANGVERDE;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p10, unEventoDeMouse)){
		opcionPj2 = LIUKANG_;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p11, unEventoDeMouse)){
		opcionPj2 = LIUKANGAZUL;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	if (estaEnRectangulo(ima_p12, unEventoDeMouse)){
		opcionPj2 = LIUKANGROJO;
		if (unEventoDeMouse->getClick())
			cliequeoEnCuadrado = true;
	}
	}

	//TECLADO Opcion 1
	if (!opcion1Seleccionada){
	if (mov1 == DER)
		opcionPj1++;

	if (mov1 == IZQ)
		opcionPj1--;

	if (mov1 == ARRIBA){
		if (opcionPj1>16)
			opcionPj1 -= 6;
	}

	if (mov1 == ABAJO){
		if (!(opcionPj1>16))
			opcionPj1 += 6;
	}

	if (opcionPj1 < 11)
		opcionPj1 = 11;

	if (opcionPj1 > 22)
		opcionPj1 = 22;
	}

	//TECLADO Opcion 2
	if (!opcion2Seleccionada){
		if (mov2 == DER)
			opcionPj2++;

		if (mov2 == IZQ)
			opcionPj2--;

		if (mov2 == ARRIBA){
			if (opcionPj2 > 16)
				opcionPj2 -= 6;
		}

		if (mov2 == ABAJO){
			if (!(opcionPj2 > 16))
				opcionPj2 += 6;
		}

		if (opcionPj2 < 11)
			opcionPj2 = 11;

		if (opcionPj2 > 22)
			opcionPj2 = 22;
	}

	cargarTexto("P1", colorDelTexto);
	//Opciones de selección
	switch (opcionPj1){
	case 11:
		imaColor1elegida = imaColor_p1;
		texto1elegido = texto_p1;
		ima1elegida = ima_p1;
		break;
	case 12:
		imaColor1elegida = imaColor_p2;
		texto1elegido = texto_p2;
		ima1elegida = ima_p2;
		break;
	case 13:
		imaColor1elegida = imaColor_p3;
		texto1elegido = texto_p3;
		ima1elegida = ima_p3;
		break;
	case 14:
		imaColor1elegida = imaColor_p4;
		texto1elegido = texto_p4;
		ima1elegida = ima_p4;
		break;
	case 15:
		imaColor1elegida = imaColor_p5;
		texto1elegido = texto_p5;
		ima1elegida = ima_p5;
		break;
	case 16:
		imaColor1elegida = imaColor_p6;
		texto1elegido = texto_p6;
		ima1elegida = ima_p6;
		break;
	case 17:
		imaColor1elegida = imaColor_p7;
		texto1elegido = texto_p7;
		ima1elegida = ima_p7;
		break;
	case 18:
		imaColor1elegida = imaColor_p8;
		texto1elegido = texto_p8;
		ima1elegida = ima_p8;
		break;
	case 19:
		imaColor1elegida = imaColor_p9;
		texto1elegido = texto_p9;
		ima1elegida = ima_p9;
		break;
	case 20:
		imaColor1elegida = imaColor_p10;
		texto1elegido = texto_p10;
		ima1elegida = ima_p10;
		break;
	case 21:
		imaColor1elegida = imaColor_p11;
		texto1elegido = texto_p11;
		ima1elegida = ima_p11;
		break;
	case 22:
		imaColor1elegida = imaColor_p12;
		texto1elegido = texto_p12;
		ima1elegida = ima_p12;
		break;
	default:
		imaColor1elegida = imaColor_p1;
		texto1elegido = texto_p1;
		ima1elegida = ima_p1;
		break;
	}

	//Ver que dibujar
	if (!opcion1Seleccionada){
		dibujarTexto(texto1elegido, pColor);
	}
	else{
		cargarTexto(textoIngresado, colorDelTexto);
		texto1elegido.x = ima1elegida.x;
		texto1elegido.w = ima1elegida.w;
		dibujarTexto(texto1elegido, 240);
	}
	SDL_SetRenderTarget(renderer, texturaSeleccionPj1);
	SDL_RenderCopy(renderer, texturaSeleccionPj1, NULL, &ima1elegida);

	SDL_SetRenderTarget(renderer, personajesMuestra);
	SDL_RenderCopy(renderer, personajesMuestra, &imaColor1elegida, &muestra1);

	

	//Opciones de selección
	cargarTexto("P2", colorDelTexto);
	switch (opcionPj2){
	case 11:
		imaColor2elegida = imaColor_p1;
		texto2elegido = texto_p1;
		ima2elegida = ima_p1;
		break;
	case 12:
		imaColor2elegida = imaColor_p2;
		texto2elegido = texto_p2;
		ima2elegida = ima_p2;
		break;
	case 13:
		imaColor2elegida = imaColor_p3;
		texto2elegido = texto_p3;
		ima2elegida = ima_p3;
		break;
	case 14:
		imaColor2elegida = imaColor_p4;
		texto2elegido = texto_p4;
		ima2elegida = ima_p4;
		break;
	case 15:
		imaColor2elegida = imaColor_p5;
		texto2elegido = texto_p5;
		ima2elegida = ima_p5;
		break;
	case 16:
		imaColor2elegida = imaColor_p6;
		texto2elegido = texto_p6;
		ima2elegida = ima_p6;
		break;
	case 17:
		imaColor2elegida = imaColor_p7;
		texto2elegido = texto_p7;
		ima2elegida = ima_p7;
		break;
	case 18:
		imaColor2elegida = imaColor_p8;
		texto2elegido = texto_p8;
		ima2elegida = ima_p8;
		break;
	case 19:
		imaColor2elegida = imaColor_p9;
		texto2elegido = texto_p9;
		ima2elegida = ima_p9;
		break;
	case 20:
		imaColor2elegida = imaColor_p10;
		texto2elegido = texto_p10;
		ima2elegida = ima_p10;
		break;
	case 21:
		imaColor2elegida = imaColor_p11;
		texto2elegido = texto_p11;
		ima2elegida = ima_p11;
		break;
	case 22:
		imaColor2elegida = imaColor_p12;
		texto2elegido = texto_p12;
		ima2elegida = ima_p12;
		break;
	default:
		imaColor2elegida = imaColor_p12;
		texto2elegido = texto_p12;
		ima2elegida = ima_p12;
		break;
	}
	
	//Ver que dibujar
	if (!opcion2Seleccionada){
		dibujarTexto(texto2elegido, pColor);
	}
	else{
		cargarTexto(textoIngresado2, colorDelTexto);
		texto2elegido.x = ima2elegida.x;
		texto2elegido.w = ima2elegida.w;
		dibujarTexto(texto2elegido, 240);
	}

	SDL_SetRenderTarget(renderer, texturaSeleccionPj2);
	SDL_RenderCopy(renderer, texturaSeleccionPj2, NULL, &ima2elegida);
	SDL_SetRenderTarget(renderer, personajesMuestra);
	SDL_RenderCopyEx(renderer, personajesMuestra, &imaColor2elegida, &muestra2, NULL, NULL, SDL_FLIP_HORIZONTAL);

	SDL_SetRenderTarget(renderer, NULL);

	//Se actualiza la pantalla
	SDL_RenderPresent(renderer);
	
	if (mov1 == G_BAJO){
		opcion1Seleccionada = true;
		textoIngresado = entradaTexto(texto1elegido, nombreTextoPj1);
	}

	if ((mov2 == G_BAJO) || (cliequeoEnCuadrado)){
		opcion2Seleccionada = true;
		textoIngresado2 = entradaTexto(texto2elegido, nombreTextoPj2);
	}

	if (opcionPj1 == opcionPj2)
		cambiarColor = true;
	else
		cambiarColor = false;

	if ((opcion1Seleccionada) && (opcion2Seleccionada)){
		personajes.seleccionados = SI;
		nombreDelP1 = textoIngresado;
		nombreDelP2 = textoIngresado2;
	}
	personajes = traducirSeleccion(opcionPj1, opcionPj2, personajes);

	return personajes;
}

int Vista::elegirDificultad(Controlador* unControlador, EventoDeMouse *unEventoDeMouse){
	
	//Se limpia la pantalla
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Se dibuja el fondo
	SDL_RenderCopy(renderer, juegoModo, NULL, &rectanguloVentana);

	//alfa transparente
	Uint8 alfaNov = 128;
	Uint8 alfaMED = 128;
	Uint8 alfaEXP = 128;
	Uint8 alfaEXPERTO = 128;
	Uint8 alfaSeleccion = 235;

	//Cuadrados a dibujar texto
	SDL_Rect rectTextoNov = ajusteResolucionBase800x600(548, 370, 150, 25);
	SDL_Rect rectTextoMed = ajusteResolucionBase800x600(548, 395, 200, 25);
	SDL_Rect rectTextoExp = ajusteResolucionBase800x600(548, 420, 200, 25);
	SDL_Rect rectTextoExperto = ajusteResolucionBase800x600(548, 445, 200, 25);

	//Color del Texto
	SDL_Color colorDelTexto = { 255, 255, 255 };

	//Parametros ventana
	Ventana ventanaVista = Parser::getInstancia().getVentana();
	MOV_TIPO movimiento = unControlador->getMovimientos().back()->getMovimiento();

	//Parametros de la ventana
	int anchoVentanaPx = ventanaVista.getAnchoPx();
	int altoVentanaPx = ventanaVista.getAltoPx();

	//Eventos del mouse
	if (unEventoDeMouse->getClick()){
		if (estaEnRectangulo(rectTextoNov, unEventoDeMouse)){
			return NOVATO;
		}
		if (estaEnRectangulo(rectTextoMed, unEventoDeMouse)){
			return MED;
		}
		if (estaEnRectangulo(rectTextoExp, unEventoDeMouse)){
			return EXPERIMENTADO;
		}
		if (estaEnRectangulo(rectTextoExperto, unEventoDeMouse)){
			return EXP;
		}
	}

	//TECLADO
	if (movimiento == G_BAJO){
		return opcionTorre;
	}

	//MOUSE
	if (estaEnRectangulo(rectTextoNov, unEventoDeMouse))
		opcionTorre = 0;
	if (estaEnRectangulo(rectTextoMed, unEventoDeMouse))
		opcionTorre = 1;
	if (estaEnRectangulo(rectTextoExp, unEventoDeMouse))
		opcionTorre = 2;
	if (estaEnRectangulo(rectTextoExperto, unEventoDeMouse))
		opcionTorre = 3;

	//TECLADO
	if (movimiento == ABAJO)
		opcionTorre++;

	if (movimiento == ARRIBA)
		opcionTorre--;

	if (opcionTorre < 0)
		opcionTorre = 0;

	if (opcionTorre > 3)
		opcionTorre = 3;

	//Opciones de selección
	switch (opcionTorre){
	case 0:
		alfaNov = alfaSeleccion;
		SDL_RenderCopy(renderer, texturabebebubu, NULL, &rectDificultad);
		break;
	case 1:
		alfaMED = alfaSeleccion;
		SDL_RenderCopy(renderer, texturaMedio, NULL, &rectDificultad);
		break;
	case 2:
		alfaEXP = alfaSeleccion;
		SDL_RenderCopy(renderer, texturaExperimentado, NULL, &rectDificultad);
		break;
	case 3:
		alfaEXPERTO = alfaSeleccion;
		SDL_RenderCopy(renderer, texturaExperto, NULL, &rectDificultad);
		break;
	default:
		alfaNov = alfaSeleccion;
		SDL_RenderCopy(renderer, texturabebebubu, NULL, &rectDificultad);
		break;
	}

	cargarTexto("puedo jugar papi?", colorDelTexto);
	dibujarTexto(rectTextoNov, alfaNov);
	cargarTexto("no me peguen soy giordano", colorDelTexto);
	dibujarTexto(rectTextoMed, alfaMED);
	cargarTexto("el que deposito dolares recibira dolares", colorDelTexto);
	dibujarTexto(rectTextoExp, alfaEXP);
	cargarTexto("No es la primera vez que hago esto", colorDelTexto);
	dibujarTexto(rectTextoExperto, alfaEXPERTO);

	//Se actualiza la pantalla
	SDL_RenderPresent(renderer);

	return NINGUNA;
}

MODOS_DE_JUEGO Vista::actualizarMenu(Controlador* unControlador, EventoDeMouse *unEventoDeMouse)
{
	//Se limpia la pantalla
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Se dibuja el fondo
	SDL_RenderCopy(renderer, juegoModo, NULL, &rectanguloVentana);

	//alfa transparente
	Uint8 alfaMK = ALFASEMITRANSPARENTE;
	Uint8 alfaVS = ALFASEMITRANSPARENTE;
	Uint8 alfaArena = ALFASEMITRANSPARENTE;
	Uint8 alfaSeleccion = 235;

	//Cuadrados a dibujar texto
	SDL_Rect rectanguloM = ajusteResolucionBase800x600(548, 370, 116, 25);
	SDL_Rect rectanguloVS = ajusteResolucionBase800x600(548, 395, 60, 25 );
	SDL_Rect rectanguloArena = ajusteResolucionBase800x600(548, 420, 60, 25 );

	//Color del Texto
	SDL_Color colorDelTexto = { 255, 255, 255 };

	//Parametros ventana
	Ventana ventanaVista = Parser::getInstancia().getVentana();
	MOV_TIPO movimiento = unControlador->getMovimientos().back()->getMovimiento();

	MODOS_DE_JUEGO modoUso= NINGUNO;
	
	//Parametros de la ventana
	int anchoVentanaPx = ventanaVista.getAnchoPx();
	int altoVentanaPx = ventanaVista.getAltoPx();

	//MOUSE
	if (unEventoDeMouse->getClick()){
		if (estaEnRectangulo(rectanguloM,unEventoDeMouse)){
			modoUso = COM;
		}
		else {
			if (estaEnRectangulo(rectanguloVS, unEventoDeMouse)){
				modoUso = VS;
			}
			else {
				if (estaEnRectangulo(rectanguloArena, unEventoDeMouse)){
					modoUso = PRACTICA;
				}
				else modoUso = NINGUNO;
			}
		}
	}

	//TECLADO
	if (movimiento == G_BAJO){
		switch (opcion){
		case 0:
			modoUso = COM;
			break;
		case 1:
			modoUso = VS;
			break;
		case 2:
			modoUso = PRACTICA;
			break;
		default:
			modoUso = NINGUNO;
			break;
		}
	}

	//MOUSE
	if (estaEnRectangulo(rectanguloM,unEventoDeMouse))
		opcion = 0;
	if (estaEnRectangulo(rectanguloVS, unEventoDeMouse))		
		opcion = 1;
	if (estaEnRectangulo(rectanguloArena, unEventoDeMouse))		
		opcion = 2;
	

	//TECLADO
	if (movimiento == ABAJO)
		opcion++;

	if (movimiento == ARRIBA)
		opcion--;

	if (opcion < 0)
		opcion = 0;

	if (opcion > 2)
		opcion = 2;

	//Opciones de selección
	switch (opcion){
	case 0:
		alfaMK = alfaSeleccion;
		break;
	case 1:
		alfaVS = alfaSeleccion;
		break;
	case 2:
		alfaArena = alfaSeleccion;
		break;
	default:
		alfaMK = alfaSeleccion;
		break;
	}

	cargarTexto("mortal kombat", colorDelTexto);
	dibujarTexto(rectanguloM, alfaMK);
	cargarTexto("versus", colorDelTexto);
	dibujarTexto(rectanguloVS, alfaVS);
	cargarTexto("arena", colorDelTexto);
	dibujarTexto(rectanguloArena, alfaArena);


	//Se actualiza la pantalla
	SDL_RenderPresent(renderer);

	return modoUso;
}
	

SDL_Surface* Vista::cargarSuperficieOptimizada(std::string dirImagen)
{	
	SDL_Surface* superficieOptimizada = NULL;

	//cargo original
	SDL_Surface* superficieOriginal = IMG_Load(dirImagen.c_str());
	if (superficieOriginal == NULL)
		Log::getInstancia().logearMensajeEnModo("No se pudo cargar imagen" + dirImagen, Log::MODO_ERROR);	
	else 
	{	//Optimiza superfice a pantalla
		superficieOptimizada = SDL_ConvertSurface(superficieOriginal, superficieOriginal->format, NULL);
		if (superficieOptimizada == NULL)
			Log::getInstancia().logearMensajeEnModo("No se pudo optimizar imagen" + dirImagen + SDL_GetError(), Log::MODO_ERROR);

		//libero la original
		SDL_FreeSurface(superficieOriginal);
	}

	return superficieOptimizada;
}

void Vista::OrdenarCapas()
{	
	std::vector<Capa*> capasOrdenadas;
	int i = 0;
	while ( capasVista.size() > 0) {
		int minimo = 0;
		for (size_t j = 1; j < capasVista.size(); j++){
			if (capasVista[j]->getZIndex() < capasVista[minimo]->getZIndex()) {
				minimo = j;				
			}
		}		
		capasOrdenadas.push_back(capasVista[minimo]);
		capasVista.erase(capasVista.begin() + minimo);		
	}
	
	capasVista.clear();
	capasVista = capasOrdenadas;
}

void Vista::Dibujar(std::vector<Personaje*> personajesVista)
{
	Ventana ventanaVista = Parser::getInstancia().getVentana();

	float anchoVentana = ventanaVista.getAncho();
	int anchoVentanaPx = ventanaVista.getAnchoPx();
	int altoVentanaPx = ventanaVista.getAltoPx();
	float anchoEscenario = Parser::getInstancia().getEscenario().getAncho();

	DibujarCapasAnteriores(personajesVista, anchoVentana, anchoVentanaPx, altoVentanaPx, anchoEscenario);

	DibujarPersonajes(personajesVista);

	

	DibujarCapasPosteriores(personajesVista, anchoVentana, anchoVentanaPx, altoVentanaPx, anchoEscenario);
   
	DibujarEfectos();
	DibujarNombres();
}

void Vista::alfa(Uint8 alfa){
	AlfaVida = alfa;
}

void Vista::DibujarInput(Controlador* unControlador){
	ConversorAString* unConversor = new ConversorAString();

	SDL_Color colorBlancoTexto;
	unConversor->mantenerStringSegunSeparador(textoCombos, " - ");

	MOV_TIPO unMovimiento = unControlador->getUltimoMovTipo();

	if (unMovimiento != QUIETO){
		textoCombos += unConversor->getTeclaComoStringDelMovimientoParaElConversorDeEventos(unMovimiento, unControlador->getConversorDeEventos()) + " - ";
	}

	if (mantenerElColor != 0){
		colorBlancoTexto = { 255, 0, 0, 0 };
		mantenerElColor--;
	}
	else {
		colorBlancoTexto = { 255, 255, 255, 255 };
		mantenerElColor = 0;
	}

	if (refMundo->huboToma()){
		textoCombos += refMundo->getToma()->getNombre() + " - ";
		mantenerElColor = 100;
	}

	cargarTexto(textoCombos, colorBlancoTexto);

	//Dibujar texturas del texto
	cuadradoRedimension.w = anchoTexto;
	cuadradoRedimension.h = altoTexto;
	dibujarTexto(cuadradoRedimension, 255);
	delete unConversor;

}
void Vista::DibujarBarrasDeVida(std::vector<Personaje*> personajesVista)
{
	SDL_SetRenderTarget(renderer, texturaBarraDeVida);
	SDL_SetRenderDrawColor(renderer, 225, 224, 227, AlfaVida);
	SDL_SetTextureAlphaMod(texturaBarraDeVida, AlfaVida);

	int vidaPj1 = personajesVista.at(0)->getVida();
	int vidaPj2 = personajesVista.at(1)->getVida();

	int nuevoAnchoBarraDeVida1 = (vidaPj1 * anchoBarraDeVida1) / 100;
	int nuevoAnchoBarraDeVida2 = (vidaPj2 * anchoBarraDeVida2) / 100;

	int nuevoanchoImagenBarraDeVida1 = (vidaPj1 *anchoImagenBarraDeVida) / 100;
	int nuevoanchoImagenBarraDeVida2 = (vidaPj2 *anchoImagenBarraDeVida) / 100;

	barraDeVida1.w = nuevoAnchoBarraDeVida1;
	barraDeVida2.w = nuevoAnchoBarraDeVida2;

	barraDeVidaImagen1.w = nuevoanchoImagenBarraDeVida1;
	barraDeVidaImagen2.w = nuevoanchoImagenBarraDeVida2;
	barraDeVida1.x = posBarraDeVida1 + anchoBarraDeVida1 - nuevoAnchoBarraDeVida1;

	SDL_RenderCopyEx(renderer, texturaBarraDeVida, &barraDeVidaImagen1, &barraDeVida1, 0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopy(renderer, texturaBarraDeVida, &barraDeVidaImagen2, &barraDeVida2);
	SDL_SetRenderTarget(renderer, NULL);

	//Parte barra roja
	SDL_SetRenderTarget(renderer, texturaBarraDeVidaRoja);
	SDL_SetRenderDrawColor(renderer, 215, 12, 25, AlfaVida);
	SDL_SetTextureAlphaMod(texturaBarraDeVidaRoja, AlfaVida);

	barraRojaDeVida2.x = barraDeVida2.x + barraDeVida2.w;
	barraRojaDeVida1.w = anchoAnteriorBarra1 - nuevoAnchoBarraDeVida1;
	barraRojaDeVida2.w = anchoAnteriorBarra2 - nuevoAnchoBarraDeVida2;
	barraRojaDeVida1.x = barraDeVida1.x - barraRojaDeVida1.w;

	barraRojaDeVidaImagen1.x = barraDeVidaImagen1.x + barraDeVidaImagen1.w;
	barraRojaDeVidaImagen2.x = barraDeVidaImagen2.x + barraDeVidaImagen2.w;
	barraRojaDeVidaImagen1.w = anchoAnteriorBarraImagen1 - nuevoanchoImagenBarraDeVida1;
	barraRojaDeVidaImagen2.w = anchoAnteriorBarraImagen2 - nuevoanchoImagenBarraDeVida2;

	//Se setean atributos anteriores para el próximo cuadro.
	if (!(personajesVista.at(0)->getEstado().golpeado == GOLPEADO) && !(personajesVista.at(1)->getEstado().golpeado == GOLPEADO)){
	anchoAnteriorBarra1 = nuevoAnchoBarraDeVida1;
	anchoAnteriorBarra2 = nuevoAnchoBarraDeVida2;

	anchoAnteriorBarraImagen1 = nuevoanchoImagenBarraDeVida1;
	anchoAnteriorBarraImagen2 = nuevoanchoImagenBarraDeVida2;
	}

	SDL_RenderCopyEx(renderer, texturaBarraDeVidaRoja, &barraRojaDeVidaImagen1, &barraRojaDeVida1, 0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopy(renderer, texturaBarraDeVidaRoja, &barraRojaDeVidaImagen2, &barraRojaDeVida2);

	SDL_SetRenderTarget(renderer, NULL);
}


void Vista::DibujarCapasAnteriores(std::vector<Personaje*> personajesVista, float anchoVentana, int anchoVentanaPx, int altoVentanaPx, float anchoEscenario)
{
	SDL_Rect camara;
	camara = { 0, 0, anchoVentanaPx, altoVentanaPx };
	//Se cargan las capas anteriores al personaje
	for (size_t i = 0; i < capasVista.size(); i++)
	{
		if (capasVista.at(i)->getZIndex() <= personajesVista[0]->getZIndex()) {
			float anchoCapa = capasVista.at(i)->getAncho();
			camara.w = manejadorULog.darLongPixels(anchoCapa);
			// donde toma la camara a la capa parametrizado con el ancho del escenario	
			camara.x = manejadorULog.darLongPixels((camaraXLog)*(anchoCapa - anchoVentana) / (anchoEscenario - anchoVentana));

			SDL_RenderCopy(renderer, capasVista.at(i)->getTexturaSDL(), NULL, &camara);
		}
	}
}

void Vista::DibujarNombres(){
	SDL_Color colorNombres = { 255, 255, 255 };
	SDL_Rect rectanguloNombre1 = ajusteResolucionBase800x600(100, 20, 100, 20);
	SDL_Rect rectanguloNombre2 = ajusteResolucionBase800x600(550, 20, 100, 20);
	cargarTexto(Parser::getInstancia().getPersonajes().at(0)->getNombreActual(), colorNombres);
	dibujarTexto(rectanguloNombre1, 255);

	cargarTexto(Parser::getInstancia().getPersonajes().at(1)->getNombreActual(), colorNombres);
	dibujarTexto(rectanguloNombre2, 255);
}

void Vista::DibujarReloj(){
	//Parte de dibujado del reloj
	Uint32 segundosTranscurridos = Parser::getInstancia().getPelea()->getTiempoDelRound()-Parser::getInstancia().getPelea()->getSegundosTranscurridosDelRound();

	SDL_Color colorReloj = { 255, 0, 0 };
	SDL_Rect rectanguloReloj = ajusteResolucionBase800x600(386, 0, 25, 50);
	std::stringstream flujoRonda;
	flujoRonda << segundosTranscurridos;

	std::string tiempoRonda = flujoRonda.str();
	cargarTexto(tiempoRonda, colorReloj);
	dibujarTexto(rectanguloReloj, 255);

	//se limpia el flujo
	flujoRonda.str("");
}


void Vista::DibujarEfectos()
{
	SDL_Rect cuadLogoPelea = ajusteResolucionBase800x600(225, 165,360,164);
	SDL_Rect cuadLogoRonda = ajusteResolucionBase800x600(235, 172, 300, 150);

	if (contadorLogoPelea > 0){
		contadorLogoPelea--;
	}
	if ((contadorLogoPelea > 0) && (contadorLogoPelea < 55)){
		SDL_SetRenderTarget(renderer, texturaLogoPelea);
		SDL_RenderCopy(renderer, texturaLogoPelea, NULL, &cuadLogoPelea);
	}
	if ((contadorLogoPelea > 60) && (contadorLogoPelea < CONTADORLOGOPELEA-20)){
		SDL_SetRenderTarget(renderer, texturaRonda);
		SDL_RenderCopy(renderer, texturaRonda, NULL, &cuadLogoRonda);
	}
}

void Vista::resetContadorLogoPelea(){
	contadorLogoPelea = CONTADORLOGOPELEA;
}

void Vista::DibujarCapasPosteriores(std::vector<Personaje*> personajesVista, float anchoVentana, int anchoVentanaPx, int altoVentanaPx, float anchoEscenario)
{
	SDL_Rect camara;
	camara = { 0, 0, anchoVentanaPx, altoVentanaPx };
	//Se cargan las capas posteriores al personaje
	for (size_t i = 0; i < capasVista.size(); i++)
	{
		if (capasVista.at(i)->getZIndex() > personajesVista[0]->getZIndex()) {
			float anchoCapa = capasVista.at(i)->getAncho();
			camara.w = manejadorULog.darLongPixels(anchoCapa);
			// donde toma la camara a la capa parametrizado con el ancho del escenario
			camara.x = manejadorULog.darLongPixels((camaraXLog)*(anchoCapa - anchoVentana) / (anchoEscenario - anchoVentana));
			SDL_RenderCopy(renderer, capasVista.at(i)->getTexturaSDL(), NULL, &camara);
		}
	}
}

void Vista::DibujarPersonajes(std::vector<Personaje*> personajesVista)
{
	Sensor* sensoresCuerpo1 = refMundo->getCuerpo(0)->getSensores()->at(0);
	Sensor* sensoresCuerpo2 = refMundo->getCuerpo(1)->getSensores()->at(0);

	float posMediaPj1 = personajesVista.at(0)->getPosicionUn().first + personajesVista.at(0)->getAncho()/2;
	float posMediaPj2 = personajesVista.at(1)->getPosicionUn().first + personajesVista.at(1)->getAncho() / 2;

	//Parámetro para ver si el jugador esta a izquierda o derecha.
	bool invertido = (posMediaPj1 > posMediaPj2);

	//Parametros del personaje 1
	int anchoPjUnoPx = manejadorULog.darLongPixels(personajesVista[0]->getAncho());
	int altoPjUnoPx = manejadorULog.darLongPixels(personajesVista[0]->getAlto(), Parser::getInstancia().getVentana().getAltoPx(), Parser::getInstancia().getEscenario().getAlto());
	int xPjUnoPx = personajesVista[0]->getPosicionPx().first;
	int yPjUnoPx = personajesVista[0]->getPosicionPx().second;
	float anchoPjUno = personajesVista[0]->getAncho();

	//Parametros del personaje 2
	int anchoPjDosPx = manejadorULog.darLongPixels(personajesVista[1]->getAncho());
	int altoPjDosPx = manejadorULog.darLongPixels(personajesVista[1]->getAlto(), Parser::getInstancia().getVentana().getAltoPx(), Parser::getInstancia().getEscenario().getAlto());
	int xPjDosPx = personajesVista[1]->getPosicionPx().first;
	int yPjDosPx = personajesVista[1]->getPosicionPx().second;
	float anchoPjDos = personajesVista[1]->getAncho();

	// Posicion x del personaje dentro de la camara
	float xLogPjUnoEnCamara = xPjUno + camaraXLog;
	SDL_Rect personajeUno;

	// Posicion x del personaje dentro de la camara
	float xLogPjDosEnCamara = xPjDos + camaraXLog ;
	SDL_Rect personajeDos;

	ESTADO estadoAux;
	estadoAux.movimiento = PARADO;
	SDL_Rect* cuadroBase = personajesVista[0]->getSprite()->listaDeCuadros(estadoAux)->at(0);
	float relacionAnchoUno = (float)anchoPjUnoPx / (float)cuadroBase->w;
	float relacionAltoUno = (float)altoPjUnoPx / (float)cuadroBase->h;
	personajeUno.x = manejadorULog.darLongPixels(xLogPjUnoEnCamara);
	personajeUno.y = yPjUnoPx;
	
	// ancho y alto lo calcula cuadro a cuadro
	ESTADO estadoDelPersonajeUno = personajesVista[0]->getEstado();

	float relacionAnchoDos = (float)anchoPjDosPx / (float)cuadroBase->w;
	float relacionAltoDos = (float)altoPjDosPx / (float)cuadroBase->h;
	personajeDos.x = manejadorULog.darLongPixels(xLogPjDosEnCamara);
	personajeDos.y = yPjDosPx;

	ESTADO estadoDelPersonajeDos = personajesVista[1]->getEstado();

	//Se carga la lista de cuadros que corresponde acorde al estado del personaje.
	listaDeCuadrosUno = personajesVista[0]->getSprite()->listaDeCuadros(estadoDelPersonajeUno);
	tiempoSecuenciaSpritesUno = personajesVista[0]->getSprite()->getConstantes(estadoDelPersonajeUno);


	if ((estadoAnteriorPj1.movimiento != estadoDelPersonajeUno.movimiento) || (estadoAnteriorPj1.accion != estadoDelPersonajeUno.accion) || (estadoAnteriorPj1.golpeado != estadoDelPersonajeUno.golpeado)){
		numeroDeCuadroUno = 0;
		estadoAnteriorPj1 = estadoDelPersonajeUno;
	}

	if ((estadoAnteriorPj2.movimiento != estadoDelPersonajeDos.movimiento) || (estadoAnteriorPj2.accion != estadoDelPersonajeDos.accion) || (estadoAnteriorPj2.golpeado != estadoDelPersonajeDos.golpeado)){
		numeroDeCuadroDos = 0;
		estadoAnteriorPj2 = estadoDelPersonajeDos;
	}
	
	if ((numeroDeCuadroUno) > (tiempoSecuenciaSpritesUno*listaDeCuadrosUno->size()))
		numeroDeCuadroUno = 0;

	if (0 >= ((listaDeCuadrosUno->size() - 1) - (numeroDeCuadroUno / tiempoSecuenciaSpritesUno)))
		numeroDeCuadroUno = 0;

	//Renderizar el sprite
	SDL_Rect* cuadroActualUno;
	if (invertido){
		cuadroActualUno = listaDeCuadrosUno->at((listaDeCuadrosUno->size() - 1) - (numeroDeCuadroUno / tiempoSecuenciaSpritesUno));
	}
	else{
		cuadroActualUno = listaDeCuadrosUno->at(numeroDeCuadroUno / (tiempoSecuenciaSpritesUno));
	}

	numeroDeCuadroUno++;

	personajeUno.w = (int)round(relacionAnchoUno*cuadroActualUno->w);
	personajeUno.h = (int)round(relacionAltoUno*cuadroActualUno->h);

	//Se carga la lista de cuadros que corresponde acorde al estado del personaje.
	listaDeCuadrosDos = personajesVista[1]->getSprite()->listaDeCuadros(estadoDelPersonajeDos);
	tiempoSecuenciaSpritesDos = personajesVista[1]->getSprite()->getConstantes(estadoDelPersonajeDos);

	if ((numeroDeCuadroDos) > (tiempoSecuenciaSpritesDos*listaDeCuadrosDos->size()))
		numeroDeCuadroDos = 0;
	
	if (0 >= ((listaDeCuadrosDos->size() - 1) - (numeroDeCuadroDos / tiempoSecuenciaSpritesDos)))
		numeroDeCuadroDos = 0;

	//Renderizar el sprite
	SDL_Rect* cuadroActualDos;
	if (!invertido){
		cuadroActualDos = listaDeCuadrosDos->at((listaDeCuadrosDos->size()-1) - (numeroDeCuadroDos / tiempoSecuenciaSpritesDos));
	}
	else{
		cuadroActualDos = listaDeCuadrosDos->at(numeroDeCuadroDos / (tiempoSecuenciaSpritesDos));
	}
	numeroDeCuadroDos++; 
	
	personajeDos.w = (int)round(relacionAnchoDos*cuadroActualDos->w);
	personajeDos.h = (int)round(relacionAltoDos*cuadroActualDos->h);
	

	SDL_Rect proyectilUno;
	SDL_Rect proyectilDos;

	//Se cargan posiciones de los proyectiles
	if (!(invertido))
		proyectilUno.x = personajeUno.x + refMundo->getProyectil(1)->getPosicion().first;
	else
		proyectilUno.x = personajeUno.x + manejadorULog.darLongPixels(personajesVista.at(0)->getAncho()) - refMundo->getProyectil(1)->getPosicion().first - refMundo->getProyectil(1)->getAncho();
	proyectilUno.y = personajeUno.y + refMundo->getProyectil(1)->getPosicion().second;	
	// nacho: obtiene el sprite del disparo, tomo el ultimo de los sprites del disparo
	SDL_Rect* cuadroProyectilUnoSprite = personajesVista.at(0)->getSprite()->listaDeCuadros(DISPARO_DEFAULT)->back();
	// nacho: ancho y alto del dibujo del sprite
	proyectilUno.w = (int)refMundo->getProyectil(1)->getAncho();
	proyectilUno.h = (int)refMundo->getProyectil(1)->getAlto();

	if (!(invertido))
		proyectilDos.x = personajeDos.x + manejadorULog.darLongPixels(personajesVista.at(1)->getAncho()) - refMundo->getProyectil(2)->getPosicion().first - refMundo->getProyectil(2)->getAncho();
	else
		proyectilDos.x = personajeDos.x + refMundo->getProyectil(2)->getPosicion().first;
	proyectilDos.y = personajeDos.y + refMundo->getProyectil(2)->getPosicion().second;
	// nacho: obtiene el sprite del disparo, tomo el ultimo de los sprites del disparo
	SDL_Rect* cuadroProyectilDosSprite = personajesVista.at(1)->getSprite()->listaDeCuadros(DISPARO_DEFAULT)->back();
	proyectilDos.w = (int)refMundo->getProyectil(2)->getAncho(); 
	proyectilDos.h = (int)refMundo->getProyectil(2)->getAlto(); 
	
	float auxPj1 = (int)round(((relacionAnchoUno*cuadroActualUno->w) - manejadorULog.darLongPixels(personajesVista.at(0)->getAncho())));
	float auxPj2 = (int)round(((relacionAnchoDos*cuadroActualDos->w) - manejadorULog.darLongPixels(personajesVista.at(1)->getAncho())));

	SDL_Rect PersonajeUnoMovimiento = personajeUno;
	SDL_Rect PersonajeDosMovimiento = personajeDos;
	PersonajeUnoMovimiento.x = personajeUno.x - auxPj1;
	PersonajeDosMovimiento.x = personajeDos.x - auxPj2;

	//Se cargan ambos acorde a su posición relativa
	if (invertido){
		SDL_RenderCopyEx(renderer, texturaSpriteUno, cuadroActualUno, &personajeUno, 0, NULL, SDL_FLIP_HORIZONTAL);
		// nacho: dibuja el proyectil si está activado
		if (refMundo->getProyectil(1)->estaActivo())
			SDL_RenderCopyEx(renderer, texturaSpriteUno, cuadroProyectilUnoSprite, &proyectilUno, 0, NULL, SDL_FLIP_HORIZONTAL);
		
		SDL_RenderCopy(renderer, texturaSpriteDos, cuadroActualDos, &PersonajeDosMovimiento);
		// nacho: dibuja el proyectil si está activado
		if (refMundo->getProyectil(2)->estaActivo())
			SDL_RenderCopy(renderer, texturaSpriteDos, cuadroProyectilDosSprite, &proyectilDos);
	}
	else{
		SDL_RenderCopy(renderer, texturaSpriteUno, cuadroActualUno, &PersonajeUnoMovimiento);
		// nacho: dibuja el proyectil si está activado
		if (refMundo->getProyectil(1)->estaActivo())
			SDL_RenderCopy(renderer, texturaSpriteUno, cuadroProyectilUnoSprite, &proyectilUno);

		SDL_RenderCopyEx(renderer, texturaSpriteDos, cuadroActualDos, &personajeDos, 0, NULL, SDL_FLIP_HORIZONTAL);
		// nacho: dibuja el proyectil si está activado
		if (refMundo->getProyectil(2)->estaActivo())
			SDL_RenderCopyEx(renderer, texturaSpriteDos, cuadroProyectilDosSprite, &proyectilDos, 0, NULL, SDL_FLIP_HORIZONTAL);
	}

	// FATALITY
	//if (Parser::getInstancia().getPelea()->terminoLaPelea()) {
	//Personaje* ganador = Parser::getInstancia().getPelea()->getPersonajeGanador();
	if ((personajesVista[0]->getEstado().accion == FATALITY_EST) || (personajesVista[1]->getEstado().accion == FATALITY_EST)){
		if (!fatalityCreada){

			//if (ganador == personajesVista[0])
			fatality = new Fatality(personajesVista[0], refMundo->getCuerpo(0), personajesVista[1], refMundo->getCuerpo(1), renderer, personajeUno, personajeDos, colorPj1);
			//else
			//	fatality = new Fatality(personajesVista[1], refMundo->getCuerpo(1), personajesVista[0], refMundo->getCuerpo(0), renderer, personajeDos, personajeUno, colorPj2);
			fatalityCreada = true;
			// le paso el control de la textura uno a la fatility
			texturaSpriteUno = nullptr;
		}
		// ejecuta un paso de la fatality
		fatality->realizar();
		//}
	}

	// tamaño sensores
	proyectilUno.w = (int)refMundo->getProyectil(1)->getAncho();
	proyectilUno.h = (int)refMundo->getProyectil(1)->getAlto();

	proyectilDos.w = (int)refMundo->getProyectil(2)->getAncho(); 
	proyectilDos.h = (int)refMundo->getProyectil(2)->getAlto(); 

	if (MODO_DEBUG_SDL){
		SDL_Rect r;
		SDL_Rect r2;
		std::vector<Sensor*>* sensoresCuerpo1 = refMundo->getCuerpo(0)->getSensores();
		std::vector<Sensor*>* sensoresCuerpo2 = refMundo->getCuerpo(1)->getSensores();

		Uint8 Alfa = 128;
		SDL_SetRenderTarget(renderer, texturaVerde);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, Alfa);
		SDL_SetTextureAlphaMod(texturaVerde, Alfa);

		for (unsigned i = 0; i<sensoresCuerpo1->size(); i++){
			if (!(invertido)){
				r.x = sensoresCuerpo1->at(i)->getPosicion().first + personajeUno.x;
			}
			else{
				r.x = personajeUno.x + manejadorULog.darLongPixels(personajesVista.at(0)->getAncho()) - sensoresCuerpo1->at(i)->getPosicion().first - sensoresCuerpo1->at(i)->getAncho();
			}
			r.y = sensoresCuerpo1->at(i)->getPosicion().second + personajeUno.y;
			r.w = sensoresCuerpo1->at(i)->getAncho();
			r.h = sensoresCuerpo1->at(i)->getAlto();
			SDL_RenderCopy(renderer, texturaVerde, NULL, &r);
		}
		for (unsigned i = 0; i < sensoresCuerpo2->size(); i++){
			if (!(invertido)){
				r2.x = personajeDos.x + manejadorULog.darLongPixels(personajesVista.at(1)->getAncho()) - sensoresCuerpo2->at(i)->getPosicion().first - sensoresCuerpo2->at(i)->getAncho();
			}
			else{
				r2.x = sensoresCuerpo2->at(i)->getPosicion().first + personajeDos.x;
			}
			r2.y = sensoresCuerpo2->at(i)->getPosicion().second + personajeDos.y;
			r2.w = sensoresCuerpo2->at(i)->getAncho();
			r2.h = sensoresCuerpo2->at(i)->getAlto();
			SDL_RenderCopy(renderer, texturaVerde, NULL, &r2);
		}
		SDL_RenderCopy(renderer, texturaVerde, NULL, &proyectilUno);
		SDL_RenderCopy(renderer, texturaVerde, NULL, &proyectilDos);

		SDL_SetRenderTarget(renderer, NULL);
	}

}


void Vista::habilitarVibracion(){
	vibracion = true;
}


void Vista::deshabilitarVibracion(){
	vibracion = false;
}


Vista::~Vista()
{
	//TTF_CloseFont(this->fuente);
	SDL_DestroyTexture(texturaSpriteUno);
	SDL_DestroyTexture(texturaSpriteDos);
	SDL_DestroyTexture(texturaVerde);
	SDL_DestroyTexture(texturaBarraDeVida);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(ventana);	
	SDL_FreeSurface(SuperficieUno);
	SDL_FreeSurface(SuperficieDos);
	//TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
