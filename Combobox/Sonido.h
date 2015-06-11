#pragma once
#include <map>

#define VALORES_ACCION {"golpear", "golpeado", "ataque", "saltar"}
#define ACCION_CONT 22


// por dependencia circular declaro y despues 
// se define en Personaje.h
struct ESTADO;

// TODO
class Sonido {
public:
	Sonido(std::string sonidoJsonDir);

	// Inicia sonido SDL_Mixer global
	static bool Iniciar();
	// Cierra sonido SDL_Mixer global
	static void Cerrar();

	// reproducir un sonido o no dependiendo del estado
	void reproducirSonido(ESTADO nuevoEstado);

	void setVolumen(int volumenNuevo);

	~Sonido();
private:
	Json::Value	ParsearSonidos(std::string sonidoJsonDir);	
	std::map<std::string, std::vector<Mix_Chunk*>*> Sonidos;
	// volumen de reproducción
	int volumen = 100;

	// hayAccionAnterior evita que se reproduzca varias veces 
	// el mismo sonido para la misma accion
	int hayAccionAnterior;
	std::vector<std::string> acciones;
	void reproducir(Mix_Chunk *sonidoWav);

	// devuelve el archivo de sonido reproducible por SDL_mixer
	// a partir de su nombre de archivo
	Mix_Chunk *Sonido::obtenerSonido(std::string archivoSonido);

};