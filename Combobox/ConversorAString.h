#pragma once
class ConversorAString
{
public:
	ConversorAString();
	std::string getTeclaComoStringDelMovimientoParaElConversorDeEventos(MOV_TIPO unMovimiento, ConversorDeEventos* conversorDeEventos);
	MOV_TIPO getMovimientoParaElString(std::string unMovimiento);
	void mantenerStringSegunSeparador(std::string& unString, std::string unSeparador);
	~ConversorAString();
};

