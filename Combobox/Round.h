#pragma once
class Round
{
public:
	Round(int unNumeroDeRound);
	void setPersonajeGanador(Personaje* unPersonajeGanador);
	Personaje* getPersonajeGanador();
	int getNumeroDeRound();
	~Round();

private:
	int numeroDeRound;
	Personaje* personajeGanador;
};

