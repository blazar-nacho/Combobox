#include "stdafx.h"
#include "Fatality.h"

Fatality::Fatality(Personaje* jugadorGanadorNuevo, Cuerpo* cuerpoGanadorNuevo, SDL_Texture* texturaGanadorNueva, Personaje* jugadorPerdedorNuevo, Cuerpo* cuerpoPerdedorNuevo, SDL_Texture* texturaPerdedorNueva, SDL_Renderer* rendererSDL, SDL_Rect cuadroGanadorNuevo, SDL_Rect cuadroPerdedorNuevo, Mundo* refMundoNueva, std::vector<double> colorGanador)
{
	jugadorGanador = jugadorGanadorNuevo;
	cuerpoGanador = cuerpoGanadorNuevo;
	jugadorPerdedor = jugadorPerdedorNuevo;
	cuerpoPerdedor = cuerpoPerdedorNuevo;
	renderer = rendererSDL;
	retraso = RETRASO_SPRT;
	parsearFatality();
	distanciaCorrecta = false;
	refMundo = refMundoNueva;
	fatalityFinalizada = false;

	texturaGanador = texturaGanadorNueva;
	texturaPerdedor = texturaPerdedorNueva;

	xJugGanador = jugadorGanador->getPosicionUn().first;
	xJugPerdedor = jugadorPerdedor->getPosicionUn().first;

	ESTADO caminarEst;
	caminarEst.accion = SIN_ACCION;
	caminarEst.movimiento = CAMINARDER;
	caminarEst.golpeado = NOGOLPEADO;

	Caminar = jugadorGanador->getSprite()->listaDeCuadros(caminarEst);

	distanciaMax = distancia + RANGO_DIST;
	distanciaMin = distancia - RANGO_DIST;

	cargarTextura(colorGanador);

	cuadroActualGanador = 0;
	cuadroGanador = cuadroGanadorNuevo;
	cuadroPerdedor = cuadroPerdedorNuevo;
	cuadroActualPerdedor = 0;

}

void Fatality::realizar()
{
	// TODO:
	// chequear distancia
	// acomodar jugador ganador a la distancia correcta
	// desaparecer sprites originales Vista->setTexturaSpriteUno...
	// reproducir secuenciaSpritesGanador en posicion del jugador ganador
	// reproducir secuenciaSpritesPerdedor en posicion del jugador perdedor


	// si movio al jugador retorna para volver y hacer otro paso
	// sino se avanza a la fatality
	if (!distanciaCorrecta){
		ubicarGanador();
		return;
	}

	// creo estado invisible
	ESTADO InvisibleEst;
	InvisibleEst.accion = FATALITY_EST;
	InvisibleEst.movimiento = INVISIBLE;
	InvisibleEst.golpeado = NOGOLPEADO;

	// seteo invisible al ganador
	refMundo->setResolver(InvisibleEst, cuerpoGanador);

	SDL_RenderCopy(renderer, texturaSDL, fatalityGanador->at(cuadroActualGanador), &cuadroGanador);

	if (delayPerdedor == 0) {
		// seteo invisible al perdedor
		refMundo->setResolver(InvisibleEst, cuerpoPerdedor);
		SDL_RenderCopy(renderer, texturaSDL, fatalityPerdedor->at(cuadroActualPerdedor), &cuadroPerdedor);

		if ((cuadroActualPerdedor < fatalityPerdedor->size() - 1) && (retraso == 0))
			cuadroActualPerdedor++;
	}
	else
		delayPerdedor--;


	if ((cuadroActualGanador < fatalityGanador->size() - 1) && (retraso == 0))
		cuadroActualGanador++;

	if ((cuadroActualGanador == fatalityGanador->size() - 1) && (cuadroActualPerdedor == fatalityPerdedor->size() - 1))
		fatalityFinalizada = true;

	if (retraso == 0)
		retraso = RETRASO_SPRT;
	else
		retraso--;

}

bool Fatality::finalizo()
{
	return fatalityFinalizada;
}

SDL_Texture* Fatality::getTexturaGanador()
{
	return texturaGanador;
}

void Fatality::ubicarGanador()
{
	// chuequear distancia
	float distanciaJugadores = abs(xJugGanador - xJugPerdedor);

	// acomodar jugador ganador a la distancia correcta
	if (distanciaJugadores > distanciaMax) {
		if (xJugGanador < xJugPerdedor) {
			xJugGanador += DIST;
			cuadroGanador.x += DIST;
			SDL_RenderCopy(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), &cuadroGanador);
		}
		else {
			xJugGanador -= DIST;
			cuadroGanador.x -= DIST;
			SDL_RenderCopyEx(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), &cuadroGanador, NULL, NULL, SDL_FLIP_HORIZONTAL);
		}

	}
	else if (distanciaJugadores < distanciaMin) {
		if (xJugGanador > xJugPerdedor) {
			xJugGanador += DIST;
			cuadroGanador.x += DIST;
			SDL_RenderCopyEx(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), &cuadroGanador, NULL, NULL, SDL_FLIP_HORIZONTAL);
		}
		else {
			xJugGanador -= DIST;
			cuadroGanador.x -= DIST;
			SDL_RenderCopy(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), &cuadroGanador);
		}
	}
	else {
		distanciaCorrecta = true;
		retraso = RETRASO_SPRT;
	}

	if ((cuadroActualCaminar < Caminar->size() - 1) && (retraso == 0))
		cuadroActualCaminar++;

	if (retraso == 0)
		retraso = RETRASO_SPRT;
	else
		retraso--;

}

void Fatality::cargarTextura(std::vector<double> colorGanador)
{

	SDL_Surface* superficieFatality = IMG_Load(imagenDir.c_str());

	// desplazo el matiz para los cuadros de la fatality del personaje ganador
	MatizColor matiz(superficieFatality);
	for (int i = 0; i < fatalityGanador->size(); i++)
		matiz.desplazarMatiz(colorGanador.at(0), colorGanador.at(1), colorGanador.at(2), fatalityGanador->at(i));


	texturaSDL = SDL_CreateTextureFromSurface(renderer, superficieFatality);
	// se queda con la textura libero la superficie
	SDL_FreeSurface(superficieFatality);
}

std::string Fatality::getImagenDir() const{
	return imagenDir;
}

void Fatality::parsearFatality()
{
	fatalityGanador = new std::vector<SDL_Rect*>();
	fatalityPerdedor = new std::vector<SDL_Rect*>();

	Json::Value raiz = ParsearRaizJson(jugadorGanador->getFatalityDir());

	// Fatality aleatoria
	size_t posAleatoria = rand() % (raiz["fatality"]["coordenadas"].size());

	imagenDir = raiz["fatality"].get("imagen", FATALITY_IMG_DEFAULT).asString();
	distancia = raiz["fatality"]["coordenadas"][posAleatoria].get("distancia", FATALITY_DIST_DEFAULT).asFloat();
	delayPerdedor = raiz["fatality"]["coordenadas"][posAleatoria].get("delayPerdedor", 0).asInt();


	// Jason Value de fatality 
	Json::Value fatalityJV = raiz["fatality"]["coordenadas"];

	// carga los cuadros de los sprites del vencedor
	Json::Value estadoSprites = raiz["fatality"]["coordenadas"][posAleatoria]["secuenciaSpritesGanador"];
	for (size_t i = 0; i < estadoSprites.size(); i++)
		fatalityGanador->push_back(crearCuadro(estadoSprites[i]));

	jugadorGanador->getSprite()->setFatality(fatalityGanador);

	// carga los cuadros de los sprites del vencido
	estadoSprites = raiz["fatality"]["coordenadas"][posAleatoria]["secuenciaSpritesPerdedor"];
	for (size_t i = 0; i < estadoSprites.size(); i++)
		fatalityPerdedor->push_back(crearCuadro(estadoSprites[i]));

	jugadorPerdedor->getSprite()->setFatality(fatalityPerdedor);

}



Json::Value	Fatality::ParsearRaizJson(std::string fatalityJsonDir)
{
	Json::Value raiz;

	Json::Reader lector(Json::Features::strictMode());
	std::ifstream archivo(fatalityJsonDir.c_str(), std::ifstream::binary);

	lector.parse(archivo, raiz, true);
	archivo.close();

	return raiz;
}

// levanta sus propios sprites
SDL_Rect* Fatality::crearCuadro(Json::Value cuadro){
	SDL_Rect* unCuadro = new SDL_Rect;
	unCuadro->x = cuadro.get("x", 0).asInt();
	unCuadro->y = cuadro.get("y", 0).asInt();
	unCuadro->w = cuadro.get("ancho", 0).asInt();
	unCuadro->h = cuadro.get("alto", 0).asInt();
	return unCuadro;
}

Fatality::~Fatality(){

}