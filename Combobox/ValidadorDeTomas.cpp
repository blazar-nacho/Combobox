#include "stdafx.h"
#include "ValidadorDeTomas.h"
#include "ConversorAString.h"

ValidadorDeTomas::ValidadorDeTomas()
{
	comboPoder = std::vector<MOV_TIPO>();
	comboPoder.push_back(DER);
	comboPoder.push_back(DER);
	comboPoder.push_back(G_BAJO);

	comboTeletransportacion = std::vector<MOV_TIPO>();
	comboTeletransportacion.push_back(IZQ);
	comboTeletransportacion.push_back(DER);
	comboTeletransportacion.push_back(IZQ);
	comboTeletransportacion.push_back(DER);
	comboTeletransportacion.push_back(DEFENSA);
}


void ValidadorDeTomas::validarTomaDeseParaControladorDeTomas(Json::Value combo, std::string nombre, ControladorDeTomas* controladorDeTomas){
	Toma* unaToma = new Toma();
	unaToma->setNombre(nombre);
	std::vector<MOV_TIPO> vecMovimientos;
	if (unaToma->getMovimientos()->empty()){
		if (nombre == NOMBRE_COMBO_1) vecMovimientos = comboPoder;
		if (nombre == NOMBRE_COMBO_2) vecMovimientos = comboTeletransportacion;
			for (size_t k = 0; k < vecMovimientos.size(); k++){
			unaToma->setMovimiento(vecMovimientos.at(k));
		}
	}

	if (!combo){
		Log::getInstancia().logearMensajeEnModo("  [BAD] Fallo el parseo del combo " + unaToma->getNombre(), Log::MODO_WARNING);
		Log::getInstancia().logearMensajeEnModo("Se carga el combo por defecto " + unaToma->getNombre(), Log::MODO_WARNING);
		return;
	}
		
	//ERROR
	if (combo.isMember("error") && combo.get("error", 0).isNumeric() && (combo.get("error", 0).asInt() >= 0)){
			unaToma->setError(combo.get("error", 0).asInt());
		}
		else Log::getInstancia().logearMensajeEnModo("Se carga error del combo " + unaToma->getNombre() + " por defecto", Log::MODO_WARNING);
		
		//TIEMPO
		if (combo.isMember("tiempo") && combo.get("tiempo", 2000).isNumeric() && (combo.get("tiempo", 2000).asInt() >= 0)){
			unaToma->setTiempo(combo.get("tiempo", 2000).asInt());
		}
		else Log::getInstancia().logearMensajeEnModo("Se carga tiempo del combo " + unaToma->getNombre() + " por defecto", Log::MODO_WARNING);
		
		//MOVIMIENTOS
		if (combo.isMember("movimientos")){
			
			Json::Value movimientos = combo.get("movimientos", NULL);
			
			if (!movimientos || movimientos.empty()){
				Log::getInstancia().logearMensajeEnModo("  [BAD] Fallo el parseo de los movimientos del combo " + unaToma->getNombre(), Log::MODO_WARNING);
				Log::getInstancia().logearMensajeEnModo("Se cargan movimientos del combo " + unaToma->getNombre() + "  por defecto", Log::MODO_WARNING);
				return;
			}

			unaToma->getMovimientos()->clear();
			MOV_TIPO unMovimiento;
			ConversorAString* conversor = new ConversorAString();

			for (size_t j = 0; j < movimientos.size(); j++){
				if (movimientos[j].isMember("movimiento") && movimientos[j].get("movimiento", "").isString()){
					unMovimiento = conversor->getMovimientoParaElString(movimientos[j].get("movimiento", "").asString());
					if (unMovimiento != QUIETO)	unaToma->setMovimiento(unMovimiento);
					else Log::getInstancia().logearMensajeEnModo("Movimiento " + movimientos[j].get("movimiento", "").asString() + " del combo " + unaToma->getNombre() + " es incorrecto, quedara sin efecto", Log::MODO_WARNING);
				}
			}

			if (unaToma->getMovimientos()->empty()){
				for (size_t n = 0; n < vecMovimientos.size(); n++){
					unaToma->setMovimiento(vecMovimientos.at(n));
				}
			}

			delete conversor;

			controladorDeTomas->setToma(unaToma);
			Log::getInstancia().logearMensajeEnModo("Se cargo el combo " + unaToma->getNombre() + " correctamente", Log::MODO_DEBUG);
			return;
		}
	Log::getInstancia().logearMensajeEnModo("No hay movimientos para el combo", Log::MODO_WARNING);
	Log::getInstancia().logearMensajeEnModo("Se cargan movimientos del combo correctamente", Log::MODO_WARNING);
}


ValidadorDeTomas::~ValidadorDeTomas()
{
	comboPoder.clear();
	comboTeletransportacion.clear();
}
