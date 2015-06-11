#pragma once
class Movimiento
{
public:
	Movimiento();
	void setMovimiento(MOV_TIPO unMovimiento);
	void setTiempo(Uint32 unTiempo);
	MOV_TIPO getMovimiento();
	Uint32 getTiempo();
	~Movimiento();

private:
	MOV_TIPO movimiento;
	Uint32 tiempo;
};

