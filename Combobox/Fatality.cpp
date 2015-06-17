#include "stdafx.h"
#include "Fatality.h"

Fatality::Fatality(Personaje* jugadorGanadorNuevo, Cuerpo* cuerpoGanadorNuevo, SDL_Texture* texturaGanadorNueva, Personaje* jugadorPerdedorNuevo, Cuerpo* cuerpoPerdedorNuevo, SDL_Texture* texturaPerdedorNueva, SDL_Renderer* rendererSDL, Mundo* refMundoNueva, std::vector<double> colorGanador)
{
	jugadorGanador = jugadorGanadorNuevo;
	cuerpoGanador = cuerpoGanadorNuevo;
	jugadorPerdedor = jugadorPerdedorNuevo;
	cuerpoPerdedor = cuerpoPerdedorNuevo;
	fatalitNum = obtenerNumFatality(jugadorGanador->getEstado());
	renderer = rendererSDL;
	retraso = RETRASO_SPRT;
	retrasoExtra = 2*RETRASO_SPRT;
	xExtra = 0;
	yVelExtra = 0;
	vibrando = false;
	tiempoVibracion = TIEMPO_VIBRACION;

	contador = CONTADOR_INI; //TIEMPOFATALITYFIN;

	extraFXDest = new SDL_Rect();

	parsearFatality();
	distanciaCorrecta = false;
	refMundo = refMundoNueva;
	fatalityFinalizada = false;

	texturaGanador = texturaGanadorNueva;
	texturaPerdedor = texturaPerdedorNueva;
	texturaPerdedorBloqueada = false;

	xJugGanador = jugadorGanador->getPosicionUn().first;
	xJugPerdedor = jugadorPerdedor->getPosicionUn().first;

	estaInvertido = xJugGanador > xJugPerdedor;

	ESTADO caminarEst;
	caminarEst.accion = SIN_ACCION;
	caminarEst.movimiento = CAMINARDER;
	caminarEst.golpeado = NOGOLPEADO;

	Caminar = jugadorGanador->getSprite()->listaDeCuadros(caminarEst);

	caminarEst.movimiento = CAMINARIZQ;
	CaminarAtras = jugadorGanador->getSprite()->listaDeCuadros(caminarEst);
	
		

	distanciaMax = distancia + RANGO_DIST;
	distanciaMin = distancia - RANGO_DIST;

	cargarTextura(colorGanador);

	cuadroActualGanador = 0;	
	cuadroActualPerdedor = 0;
	cuadroActualExtraFX = 0;

}

void Fatality::realizar(SDL_Rect *cuadroGanadorActual, SDL_Rect *cuadroPerdedorActual)
{
	// hace:
	// chequear distancia
	// acomodar jugador ganador a la distancia correcta
	// desaparecer sprites originales Vista->texturaSpriteUno...
	// reproducir secuenciaSpritesGanador en posicion del jugador ganador
	// reproducir secuenciaSpritesPerdedor en posicion del jugador perdedor

	cuadroGanador = cuadroGanadorActual;
	cuadroPerdedor = cuadroPerdedorActual;

	// si movio al jugador retorna para volver y hacer otro paso
	// sino se avanza a la fatality
	if (!distanciaCorrecta){
		ubicarGanador();
		return;
	}

	// tiempo de fatality
	contador--;

	
	

	if (!estaInvertido)
		SDL_RenderCopy(renderer, texturaSDL, fatalityGanador->at(cuadroActualGanador), cuadroGanador);
	else
		SDL_RenderCopyEx(renderer, texturaSDL, fatalityGanador->at(cuadroActualGanador), cuadroGanador, NULL, NULL, SDL_FLIP_HORIZONTAL);


	if (delayPerdedor == 0) {
		// seteo invisible al perdedor
		texturaPerdedorBloqueada = true;		

		if (!estaInvertido)
		SDL_RenderCopy(renderer, texturaSDL, fatalityPerdedor->at(cuadroActualPerdedor), cuadroPerdedor);
	else
		SDL_RenderCopyEx(renderer, texturaSDL, fatalityPerdedor->at(cuadroActualPerdedor), cuadroPerdedor, NULL, NULL, SDL_FLIP_HORIZONTAL);
		
		if ((cuadroActualPerdedor < fatalityPerdedor->size() - 1) && (retraso == 0))
			cuadroActualPerdedor++;
	}
	else
		delayPerdedor--;

	dibujarExtraFX();

	if ((cuadroActualGanador < fatalityGanador->size() - 1) && (retraso == 0))
		cuadroActualGanador++;

	if ((cuadroActualGanador == fatalityGanador->size() - 1) && (cuadroActualPerdedor == fatalityPerdedor->size() - 1)) {
		fatalityEfectuada = true;
		if (contador < 1) {
			fatalityFinalizada = true;
			texturaPerdedorBloqueada = false;
		}
	}

	if (retraso == 0)
		retraso = RETRASO_SPRT;
	else
		retraso--;

}

bool Fatality::finalizo()
{
	return fatalityFinalizada;
}

bool Fatality::efectuada()
{
	return fatalityEfectuada;
}

SDL_Texture* Fatality::getTexturaGanador()
{
	return texturaGanador;
}

SDL_Texture* Fatality::getTexturaPerdedor()
{
	if (!texturaPerdedorBloqueada)
		return texturaPerdedor;

	return nullptr;
}



void Fatality::ubicarGanador()
{
	// chuequear distancia
	float distanciaJugadores = abs(xJugGanador - xJugPerdedor);

	// acomodar jugador ganador a la distancia correcta
	if (distanciaJugadores > distanciaMax) {
		if (xJugGanador < xJugPerdedor) {
			xJugGanador += DIST/4;
			cuerpoGanador->mover(DIST / 4);
			
			SDL_RenderCopy(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), cuadroGanador);
		}
		else {
			xJugGanador -= DIST/4;
			cuerpoGanador->mover(-DIST / 4);
			
			SDL_RenderCopyEx(renderer, texturaGanador, Caminar->at(cuadroActualCaminar), cuadroGanador, NULL, NULL, SDL_FLIP_HORIZONTAL);
		}

	}
	else if (distanciaJugadores < distanciaMin) {
		if (xJugGanador > xJugPerdedor) {
			xJugGanador += DIST/4;
			cuerpoGanador->mover(DIST / 4);
			
			SDL_RenderCopyEx(renderer, texturaGanador, CaminarAtras->at(cuadroActualCaminar), cuadroGanador, NULL, NULL, SDL_FLIP_HORIZONTAL);
		}
		else {
			xJugGanador -= DIST/4;
			cuerpoGanador->mover(-DIST / 4);
			
			SDL_RenderCopy(renderer, texturaGanador, CaminarAtras->at(cuadroActualCaminar), cuadroGanador);
		}
	}
	else {
		distanciaCorrecta = true;
		retraso = RETRASO_SPRT;
	}

	if ((cuadroActualCaminar < Caminar->size() - 1) && (retraso == 0))
		cuadroActualCaminar++;
	else if (cuadroActualCaminar == Caminar->size() - 1)
		cuadroActualCaminar = 0;

	if (retraso == 0)
		retraso = RETRASO_SPRT;
	else
		retraso--;

}

void Fatality::dibujarExtraFX()
{	
	if (delayExtra > 0) {
		delayExtra--;
		return;
	}	

	yExtra += 2*DISTANCIA;
	extraFXDest->y = cuadroGanador->y + yExtra;		


	if (!estaInvertido) {
		if (xIniExtraEsGanador) {
			xExtra += 5*DISTANCIA;
			extraFXDest->x = cuadroGanador->x + xExtra + X_EXTRA_DESP;
			if (extraFXDest->x > cuadroPerdedor->x) return;
		}
		else {
			if ((fatalitNum == 0) || (fatalitNum == 1) || (fatalitNum == 3)) {
				extraFXDest->x = cuadroPerdedor->x + X_EXTRA_DESP / 2;
				extraFXDest->y = yInicialExtra;
				extraFXDest->w = cuadroGanador->w;
				extraFXDest->h = 2*cuadroGanador->h;
				if (fatalitNum == 3)
					extraFXDest->h = cuadroGanador->h;
			}
			else {
				extraFXDest->x = cuadroPerdedor->x + X_EXTRA_DESP / 2;
				yVelExtra += Y_EXTRA_VEL_INC;
				extraFXDest->y = yInicialExtra + yVelExtra;
				if (extraFXDest->y >= cuadroPerdedor->y) {
					extraFXDest->y = cuadroPerdedor->y;
					if (tiempoVibracion > 0) {
						vibrando = true;
						tiempoVibracion--;
					}
					else
						vibrando = false;
				}
			}
			
		}
		SDL_RenderCopy(renderer, texturaSDL, extraFX->at(cuadroActualExtraFX), extraFXDest);
	}
	else {
		if (xIniExtraEsGanador) {
			xExtra -= 5 * DISTANCIA;
			extraFXDest->x = cuadroGanador->x + xExtra + X_EXTRA_DESP_INV;
			if (extraFXDest->x < cuadroPerdedor->x + X_EXTRA_FIN_INV) return;
		}
		else {
			if ((fatalitNum == 0) || (fatalitNum == 1) || (fatalitNum == 3)){
				extraFXDest->x = cuadroPerdedor->x + X_EXTRA_DESP / 2;
				extraFXDest->y = yInicialExtra;
				extraFXDest->w = cuadroGanador->w;
				extraFXDest->h = 2 * cuadroGanador->h;
				if (fatalitNum == 3)
					extraFXDest->h = cuadroGanador->h;
			}
			else {
				extraFXDest->x = cuadroPerdedor->x + X_EXTRA_DESP / 2;
				yVelExtra += Y_EXTRA_VEL_INC;
				extraFXDest->y = yInicialExtra + yVelExtra;
				if (extraFXDest->y >= cuadroPerdedor->y) {
					extraFXDest->y = cuadroPerdedor->y;
					if (tiempoVibracion > 0) {
						vibrando = true;
						tiempoVibracion--;
					}
					else
						vibrando = false;
				}
			}
		}
		SDL_RenderCopyEx(renderer, texturaSDL, extraFX->at(cuadroActualExtraFX), extraFXDest, NULL, NULL, SDL_FLIP_HORIZONTAL);
	}

	if ((cuadroActualExtraFX < extraFX->size() - 1) && (retrasoExtra == 0))
		cuadroActualExtraFX++;

	if (retrasoExtra == 0)
		retrasoExtra = 2 * RETRASO_SPRT;
	else
		retrasoExtra--;
}


void Fatality::cargarTextura(std::vector<double> colorGanador)
{

	SDL_Surface* superficieFatality = IMG_Load(imagenDir.c_str());

	// desplazo el matiz para los cuadros de la fatality del personaje ganador
	MatizColor *matiz = new MatizColor(superficieFatality);
	for (int i = 0; i < fatalityGanador->size(); i++)
		matiz->desplazarMatiz(colorGanador.at(0), colorGanador.at(1), colorGanador.at(2), fatalityGanador->at(i));

	delete matiz;
	texturaSDL = SDL_CreateTextureFromSurface(renderer, superficieFatality);
	// se queda con la textura libero la superficie
	SDL_FreeSurface(superficieFatality);
}

std::string Fatality::getImagenDir() const{
	return imagenDir;
}

bool Fatality::vibrar(){
	return vibrando;
}

void Fatality::parsearFatality()
{
	fatalityGanador = new std::vector<SDL_Rect*>();
	fatalityPerdedor = new std::vector<SDL_Rect*>();
	extraFX = new std::vector<SDL_Rect*>();

	Json::Value raiz = ParsearRaizJson(jugadorGanador->getFatalityDir());

	// Fatality aleatoria
	size_t posAleatoria = rand() % (raiz["fatality"]["coordenadas"].size());	
	//fatalitNum = 3;

	imagenDir = raiz["fatality"].get("imagen", FATALITY_IMG_DEFAULT).asString();


	if (raiz["fatality"]["coordenadas"].size() > fatalitNum) {
		distancia = raiz["fatality"]["coordenadas"][fatalitNum].get("distancia", FATALITY_DIST_DEFAULT).asFloat();
		delayPerdedor = raiz["fatality"]["coordenadas"][fatalitNum].get("delayPerdedor", 0).asInt();
	}
	// esto no debería pasar, si se le paso una fatality con un numero mayor a lo posible devuelve una aleatoria
	else {
		distancia = raiz["fatality"]["coordenadas"][posAleatoria].get("distancia", FATALITY_DIST_DEFAULT).asFloat();
		delayPerdedor = raiz["fatality"]["coordenadas"][posAleatoria].get("delayPerdedor", 0).asInt();
	}


	// Jason Value de fatality 
	Json::Value fatalityJV = raiz["fatality"]["coordenadas"];
	Json::Value estadoSprites;
	// carga los cuadros de los sprites del vencedor
	if (raiz["fatality"]["coordenadas"].size() > fatalitNum) 
		estadoSprites = raiz["fatality"]["coordenadas"][fatalitNum]["secuenciaSpritesGanador"];
	else 
		estadoSprites = raiz["fatality"]["coordenadas"][posAleatoria]["secuenciaSpritesGanador"];

	for (size_t i = 0; i < estadoSprites.size(); i++)
		fatalityGanador->push_back(crearCuadro(estadoSprites[i]));

	jugadorGanador->getSprite()->setFatality(fatalityGanador);

	// carga los cuadros de los sprites del vencido
	if (raiz["fatality"]["coordenadas"].size() > fatalitNum)
		estadoSprites = raiz["fatality"]["coordenadas"][fatalitNum]["secuenciaSpritesPerdedor"];
	else
		estadoSprites = raiz["fatality"]["coordenadas"][posAleatoria]["secuenciaSpritesPerdedor"];

	for (size_t i = 0; i < estadoSprites.size(); i++)
		fatalityPerdedor->push_back(crearCuadro(estadoSprites[i]));

	// carga los cuadros de los sprites extras, efectos
	if (raiz["fatality"]["coordenadas"].size() > fatalitNum)
		estadoSprites = raiz["fatality"]["coordenadas"][fatalitNum]["extraFX"];
	else
		estadoSprites = raiz["fatality"]["coordenadas"][posAleatoria]["extraFX"];

	yInicialExtra = estadoSprites.get("yInicial", 100).asFloat();
	xInicialExtra = estadoSprites.get("xInicial", "ganador").asString();
	xIniExtraEsGanador = (xInicialExtra == "ganador");
	delayExtra = estadoSprites.get("delay", 20).asInt();
	estadoSprites = estadoSprites["secuencia"];
	for (size_t i = 0; i < estadoSprites.size(); i++)
		extraFX->push_back(crearCuadro(estadoSprites[i]));
	
	extraFXDest->w = 2*extraFX->at(0)->w;
	extraFXDest->h = 2*extraFX->at(0)->h;
	yExtra = yInicialExtra;

	jugadorPerdedor->getSprite()->setFatality(fatalityPerdedor);

}


size_t Fatality::obtenerNumFatality(ESTADO estadoPersonaje)
{
	if (estadoPersonaje.fatality == FIRE)
		return 0;
	if (estadoPersonaje.fatality == FIREKANG)
		return 0;
	if (estadoPersonaje.fatality == GODHAND)
		return 1;
	if (estadoPersonaje.fatality == ARCADE)
		return 2;
	if (estadoPersonaje.fatality == BABALITY)
		return 3;

	// default
	return 0;
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
	for (size_t i = 0; i < fatalityGanador->size(); i++)
		delete fatalityGanador->at(i);
	fatalityGanador->clear();
	delete fatalityGanador;

	for (size_t i = 0; i < fatalityPerdedor->size(); i++)
		delete fatalityPerdedor->at(i);
	fatalityPerdedor->clear();
	delete fatalityPerdedor;

	for (size_t i = 0; i < extraFX->size(); i++)
		delete extraFX->at(i);
	extraFX->clear();
	delete extraFX;

	delete extraFXDest;

}