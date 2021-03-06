#pragma once
#include "Sprites.h"
#include "Sonido.h"

enum Movimientos
{
	PARADO, CAMINARDER, CAMINARIZQ, AGACHADO, SALTO, SALTODIAGIZQ, SALTODIAGDER, INVISIBLE
};

enum Acciones
{
	SIN_ACCION, GUARDIA, GANCHO, PATADA_ALTA, PATADA_BAJA, GOLPE_ALTO1, GOLPE_ALTO2, GOLPE_BAJO, ARMA_ARROJABLE, ICESHOOT,TELEPORT,FIREWALL,
	BICICLETA, FLYKICK, SUBBARRIDA, POWERBALL, ARPON, FATALITY_RUN, FATALITY_END
};

/*
#define NOMBRE_COMBO_1 "Poder"  //todos
#define NOMBRE_COMBO_2 "iceshoot"  //scorpion
#define NOMBRE_COMBO_3 "Teleport" //scorpion
#define NOMBRE_COMBO_4 "firewall"  //liukang //TODOS INVISIBLE
#define NOMBRE_COMBO_5 "bicicleta" //liukang
#define NOMBRE_COMBO_6 "flykick"  //liukang
#define NOMBRE_COMBO_7 "subbarrida" //scorpion
#define NOMBRE_COMBO_8 "powerball" //scorpion
#define NOMBRE_COMBO_9 "ARCADE" //AMBOS
#define NOMBRE_COMBO_10 "FIRE"  // scorpion
#define NOMBRE_COMBO_11 "GODHAND" //scorpion 
#define NOMBRE_COMBO_12 "FIREKANG" //liukang

*/







enum Golpeado
{
	NOGOLPEADO, GOLPEADO, FALLECIDO,FALLECIDO_ROUND,DIZZY,TUMBANDOSE, TUMBADO,
};

enum TipoDeArmas
{
	PROYECTIL, HIELO, ACIDO
};

enum Fatalitys
{
	ARCADE, FIRE, FIREKANG, GODHAND, BABALITY
};

struct ESTADO {
	Movimientos movimiento;
	Acciones accion;
	Golpeado golpeado;
	TipoDeArmas tipoarma;
	Fatalitys fatality;
};

class Personaje
{
public:
	std::string nombre;
	Personaje();
	void copiarAtributosDe(Personaje* unPersonaje);
	Personaje::Personaje(float anchoPersonaje, float altoPersonaje, int zIndexPersonaje, std::string unaorientacion, std::string spritesDirPersonaje, std::string CaminarParaAdelante, std::string CaminarParaAtras, std::string QuietoPersonaje, std::string SaltoPersonaje, std::string SaltoDiagonalPersonaje, std::string CaidaPersonaje, std::string PatadaAltaPersonaje, std::string GolpeadoPersonaje, std::string AgachadoPersonaje, std::string nombrePersonaje, std::string sonidosDirPersonaje, std::string fatalityDirPersonaje);
	float getAncho() const;
	float getAlto() const;
	int getZIndex() ;
	int descontarVida(ESTADO estadoPj, ESTADO estadoPj2);
	int getVida();
	std::string getOrientacion() const;
	std::string getFatalityDir() const;
	std::string getSpriteDir() const;	
	Sprite* getSprite() const;
	std::string getSonidosDir() const;
	Sonido* getSonidos() const;
	std::string getCaminarParaAdelante() const;
	std::string getCaminarParaAtras() const;
	std::string getQuieto() const;
	std::string getSalto() const;
	std::string getSaltoDiagonal() const;
	std::string getCaida() const;
	std::string getNombre() const;
	std::string getNombreActual() const;
	std::string getPatadaAlta() const;
	std::string getAgacharse() const;
	std::string getGolpeado() const;
	std::pair<int, int> getPosicionPx() const;
	std::pair<int, int> Personaje::getPosicionPx(int altoSprite) const;
	void setPosicionUn(float x, float y);
	ESTADO getEstado() const;
	void setEstado(ESTADO nuevoEstado);
	float getDeltaX() const;	
	virtual void actualizar(float xNuevo, float yNuevo, ESTADO estadoJugador);	
	std::pair<float, float> getPosicionUn() const;	
	int getZIndex() const;
	void reiniciarAtributos();

	//SETTERS
	void setVida(int unaVida);
	void setDeltaX(float unDeltaX);
	void setAncho(float unAncho);
	void setAlto(float unAlto);
	void setZIndex(int unZindex);
	void setOrientacion(std::string unaOrientacion);
	void setSprites(std::string unosSprites);
	void setSonidos(std::string unosSonidos);
	void setFatality(std::string unaFatality);
	void setCaminarParaAdelante(std::string unCaminarParaAdelante);
	void setCaminarParaAtras(std::string unCaminarParaAtras);
	void setQuieto(std::string unQuieto);
	void setSalto(std::string unSalto);
	void setSaltoDiagonal(std::string unSaltoDiagonal);
	void setCaida(std::string UnaCaida);
	void setNombre(std::string unNombre);
	void setPatadaAlta(std::string unaPatadaAlta);
	void setAgacharse(std::string unAgacharse);
	void setGolpeado(std::string unGolpeado);
	void setNombreActual(std::string unNombre);
	


	~Personaje();

private:
	bool vidaInfinita;
	std::string nombreActual;
	float ancho;
	float alto;
	int zIndex;
	int vida;	
	std::string orientacion;
	std::string spritesDir;
	std::string caminaradelante;
	std::string caminaratras;
	std::string quieto;
	std::string salto;
	std::string saltodiagonal;
	std::string caida;
	std::string nombreP;
	std::string patadaalta;
	std::string agachado;
	std::string golpeado;
	std::string fatalityDir;
	std::string sonidosDir;
	Sonido* sonidos;
	Sprite* sprites;
	std::pair<float, float> posicionUn;
	float deltaX;	
	ESTADO estadoActual;
};

