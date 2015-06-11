#pragma once
class ModoDeJuego
{
public:
	ModoDeJuego();
	virtual Controlador* getControlador1();
	virtual Controlador* getControlador2();
	virtual Personaje* getPersonaje1();
	virtual Personaje* getPersonaje2();
	virtual bool esModoPractica();
	virtual void iniciar();
	~ModoDeJuego();

protected:
	Controlador* controlador1;
	Controlador* controlador2;
	Personaje* personaje1;
	Personaje* personaje2;
};

