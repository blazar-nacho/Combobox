#include "stdafx.h"
#include "Sonido.h"


bool Sonido::Iniciar(){
	// Se inicializa la librería SDL_Mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
		Log::getInstancia().logearMensajeEnModo(("SDL_mixer no se pudo iniciar! SDL_mixer Error: %s\n", SDL_GetError()), Log::MODO_ERROR);
		return false;
	}
	return true;
}

void Sonido::Cerrar(){
	Mix_CloseAudio();
}


Sonido::Sonido(std::string sonidoJsonDir){
	acciones = VALORES_ACCION;
	hayAccionAnterior = ACCION_CONT;

	Json::Value raiz = ParsearSonidos(sonidoJsonDir);
	// Jason Value de sonidos 
	Json::Value sonidosJV = raiz["sonidos"]["accion"];

	if (!MODO_SONIDO)
		volumen = 0;

	
	for (size_t i = 0; i < acciones.size(); i++) {
		
		std::string nombreAccion = acciones.at(i);
		std::vector<Mix_Chunk*>* sonidoArchs = new std::vector<Mix_Chunk*>();
		for (size_t j = 0; j < sonidosJV[nombreAccion].size(); j++) {
			Mix_Chunk *sonidoAgregado = obtenerSonido(sonidosJV[nombreAccion][j].get("archivo", SONIDO_FX_DEFAULT).asString());
			sonidoArchs->push_back(sonidoAgregado);
		}
		
		Sonidos.insert(std::pair<std::string, std::vector<Mix_Chunk*>*>(nombreAccion, sonidoArchs));
	}
}


Json::Value	Sonido::ParsearSonidos(std::string sonidoJsonDir)
{
	Json::Value raiz;

	Json::Reader lector(Json::Features::strictMode());
	std::ifstream archivo(sonidoJsonDir.c_str(), std::ifstream::binary);

	lector.parse(archivo, raiz, true);
	archivo.close();

	return raiz;
}

Mix_Chunk *Sonido::obtenerSonido(std::string archivoSonido)
{
	// Se carga un sonido
	Mix_Chunk *sonidoWav;

	sonidoWav = Mix_LoadWAV(archivoSonido.c_str());

	if (sonidoWav == NULL)
		Log::getInstancia().logearMensajeEnModo("Falla al cargar el sonido" + archivoSonido, Log::MODO_WARNING);

	return sonidoWav;
}

void Sonido::reproducir(Mix_Chunk *sonidoWav)
{			

	Mix_VolumeChunk(sonidoWav, volumen);

	// Se establece 4 canales , o cantidad de sonidos simultaneos
	Mix_AllocateChannels(4);

	// Se reproduce el sonido en el canal 1
	// 0 para reproducir una sola vez 
	// o -1 para reproducir loop infinito
	Mix_PlayChannel(-1, sonidoWav, 0);

	
}

void Sonido::reproducirSonido(ESTADO nuevoEstado)
{
	// si esta reproduciendo un estado, espera
	if (hayAccionAnterior > 0) {
		hayAccionAnterior--;
		return;
	}


	// TODO: seria buena otra forma de tomar accion anterior
	if (nuevoEstado.golpeado == GOLPEADO) {
		reproducir(Sonidos["golpeado"]->at(rand() % (Sonidos["golpeado"]->size())));
	}

	if ((nuevoEstado.accion >= GANCHO) && (nuevoEstado.accion <= GOLPE_BAJO)){
		reproducir(Sonidos["golpear"]->at(rand() % (Sonidos["golpear"]->size())));
		reproducir(Sonidos["ataque"]->at(rand() % (Sonidos["ataque"]->size())));
	}

	if (nuevoEstado.movimiento >= SALTO) {
		reproducir(Sonidos["saltar"]->at(0));
		// TODO: esto no esta bueno.. 
		hayAccionAnterior = 3 * ACCION_CONT;
		return;
	}

	// si no asigna otra cosa, asigna contador
	hayAccionAnterior = ACCION_CONT;

}


void Sonido::setVolumen(int volumenNuevo)
{
	if ((volumenNuevo >= 0) && (volumenNuevo <= 100))
		volumen = volumenNuevo;

}

Sonido::~Sonido()
{
	// defino iterador de map
	std::map<std::string, std::vector<Mix_Chunk*>*>::iterator it;
	// recorro los vectores de sonidos dentro del map y
	// voy liberando los sonidos
	for (it = Sonidos.begin(); it != Sonidos.end(); ++it) {
		for (size_t j = 0; j < it->second->size(); j++)
			Mix_FreeChunk(it->second->at(j));
		it->second->clear();
	}
	
	Sonidos.clear();
}