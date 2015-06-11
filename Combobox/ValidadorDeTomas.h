#pragma once
#include "ControladorDeTomas.h"

class ValidadorDeTomas
{
public:
	ValidadorDeTomas();
	void validarTomaDeseParaControladorDeTomas(Json::Value combo, std::string nombre, ControladorDeTomas* controladorDeTomas);
	~ValidadorDeTomas();

private:
	std::vector<MOV_TIPO> comboTeletransportacion;
	std::vector<MOV_TIPO> comboPoder;
};

