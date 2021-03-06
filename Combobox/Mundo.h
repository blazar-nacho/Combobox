#pragma once
#include "Controlador.h"
#include "Cuerpo.h"
#include "Escenario.h"
#include "Sprites.h"

#define SALTO_Y 580.0f
#define SALTO_X 180.0f
#define GRAVEDAD_Y -8.0f
#define DISTANCIA 2.0f
#define VELOCIDADPROYECTIL 20.0f
#define FACTOR_DIST_REVERSA 0.6f
#define DEFAULT_ESTA_GOLPEADO false// sacarla, esta para testear rapido
class Cuerpo;

class Mundo
{
public:
	Mundo();
	~Mundo();
	Mundo(const vector2D& valorGravedad);
	void agregarCuerpo(Cuerpo *unCuerpo);
	int Paso(float difTiempo);
	ESTADO Resolver(float difTiempo, Cuerpo *unCuerpo);
	float getYPiso() const;
	void FrenarCuerpos();
	void LiberarCuerpos();
	Cuerpo* getCuerpo(size_t pos);
	Sensor* getProyectil(size_t pos);
	Sensor* sensorEngloba(Sensor* sensorUno, Sensor* sensorDos);
	bool huboToma();
	void setResolver(ESTADO, Cuerpo*);
	Toma* getToma();
	void limpiarCuerpos();
	void reiniciar();
	void setModoActual(MODOS_DE_JUEGO unModoActual);

private:
	Timer* unReloj;
	Toma* ultimaToma;
	vector2D gravedad;
	ESTADO estadoAnteriorPj1;
	ESTADO estadoAnteriorPj2;
	ESTADO estadoResolver;
	Cuerpo* resolverRefCuerpo;
	bool resolverSeteado;
	float yPiso;
	bool cambioGolpeAlto;
	std::vector<Cuerpo*> Cuerpos;
	bool hayInterseccion(std::pair<float, float> unaPosicion, float unAncho, float unAlto, std::pair<float, float> otraPos, float otroAncho, float otroAlto);
	bool haySuperposicion(Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, bool invertido);
	MODOS_DE_JUEGO modoActual;

	ESTADO ResolverArma(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, Sensor* proyectil, bool invertido, ESTADO nuevoEstado);
	void resolverChoque(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, Sensor* proyectilUno, Sensor* proyectilDos, bool invertido);

	ESTADO moverCuerpos(Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, bool invertido, std::vector<Movimiento*>* movimientos, ESTADO nuevoEstado);

	ESTADO ResolverAtaques(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, ESTADO nuevoEstado, Sensor* proyectilUno, Sensor* proyectilDos, bool invertido);
	ESTADO ResolverGolpes(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, bool invertido, ESTADO nuevoEstado);
	ESTADO ResolverColisiones(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, bool invertido, ESTADO nuevoEstado);
	ESTADO ResolverSaltos(float difTiempo, Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos);
	ESTADO ResolverAcciones(float difTiempo, Cuerpo *unCuerpo, Cuerpo *otroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos);
	ESTADO ResolverTomas(float difTiempo, Cuerpo *unCuerpo, Cuerpo* otroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos, Toma* unaToma);
	ESTADO ResolverDemorasEspeciales(float difTiempo, Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, ESTADO nuevoEstado, bool invertido);
	ESTADO ResolverBatalla(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, ESTADO nuevoEstado, bool invertido,bool SinVida);
};