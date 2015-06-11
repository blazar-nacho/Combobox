#pragma once
#include "ModoDeJuego.h"

class ModoPractica: public ModoDeJuego
{
public:
	ModoPractica();
	bool esModoPractica();
	void iniciar();
	~ModoPractica();
};

