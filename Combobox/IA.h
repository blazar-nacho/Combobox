#pragma once
class IA
{
public:
	enum NIVEL{ BAJO, MEDIO, DIFICIL, EXPERTO };

	IA();
	MOV_TIPO getMovimiento();
	void setPersonajeRival(Personaje* unPersonaje);
	void setPersonajeControlado(Personaje* unPersonaje);
	bool tienePersonajeControlado();
	bool tienePersonajeRival();
	void setNivel(NIVEL unNivel);
	~IA();

private:
	Personaje* personajeRival;
	Personaje* personajeControlado;
	std::vector<std::vector<MOV_TIPO>> movimientosIdeales;
	int umbralDeProbabilidad;
	unsigned int cantidadDeMovimientosAdelante;
	unsigned int cantidadDeMovimientosAtras;
};

