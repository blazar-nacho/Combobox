#include "Parser.h"
#include "ConversorDeEventos.h"
#include "stdafx.h"

Parser& Parser::getInstancia() {

	static Parser instancia;
	// Se garantiza que será destruido.
	// Se instancia en el primer uso.
	return instancia;
}

Parser::Parser() { 
}


bool Parser::parsear(std::string nombreDelArchivo)
{	
	Json::Value raiz;
	//Se coloca al lector de Json en modo estricto para que loguee todos los mensajes de errores sintácticos
	Json::Reader lector(Json::Features::strictMode());
	std::ifstream prueba(nombreDelArchivo, std::ifstream::binary);

	bool parseoExitoso;

	//chequea que exista el archivo prueba.json y se pudeda abrir
	if (!prueba.is_open()){
		Log::getInstancia().logearMensajeEnModo("  [BAD] No se pudo abrir el json de prueba o no existe", Log::MODO_WARNING);
		parseoExitoso = false;
	}
	else //el archivo existe, lo parseo
	{
		parseoExitoso = lector.parse(prueba, raiz, true);
	}

	

	if (!parseoExitoso) {
		std::string mensaje = "  [BAD]  Fallo el parseo, se carga json por defecto" + lector.getFormattedErrorMessages();
		Log::getInstancia().logearMensajeEnModo(mensaje, Log::MODO_WARNING);
		std::ifstream prueba(PRUEBA_JSON_DEFECTO, std::ifstream::binary);
		if (!prueba.is_open()){
			Log::getInstancia().logearMensajeEnModo("  [BAD] No se pudo abrir el json por defecto, se cerrara el programa...", Log::MODO_ERROR);
			return false;
		}
		bool parseoExitoso = lector.parse(prueba, raiz, true);
		if (!parseoExitoso) {
			Log::getInstancia().logearMensajeEnModo("  [BAD] Fallo el parseo del json por defecto, se cerrara el programa...", Log::MODO_ERROR);
			return false;
		}
		Log::getInstancia().logearMensajeEnModo("Parseo del archivo json por defecto correcto", Log::MODO_DEBUG);
	}else Log::getInstancia().logearMensajeEnModo("Parseo del archivo json correcto", Log::MODO_DEBUG);


	//VENTANA
	Json::Value ventana;
	ventana = raiz["ventana"];

	validadorDeVentana = new ValidadorDeVentana();
	validadorDeVentana->validarVentanaDesde(ventana);

	//ESCENARIO
	Json::Value escenario;
	escenario = raiz["escenario"];

	validadorDeEscenario = new ValidadorDeEscenario();
	validadorDeEscenario->validarEscenarioDesde(escenario);

	//CAPAS
	Json::Value capas;
	capas = raiz["capas"];

	validadorDeCapas = new ValidadorDeCapas();
	if (!validadorDeCapas->validarCapasDesde(capas)) return false;

	//PERSONAJES
	Json::Value personajes;
	personajes = raiz["personajes"];
	
	validadorDePersonajes = new ValidadorDePersonajes();
	if (!validadorDePersonajes->validarPersonajesDesdeConEscenarioYventana(personajes, validadorDeEscenario->getEscenario(), validadorDeVentana->getVentana()))
		return false;
	
	validadorDeEscenario->validaryPisoCon(validadorDePersonajes->getPersonajes()->at(0)->getAlto());

	//PELEA
	Json::Value pelea;
	pelea = raiz["pelea"];

	validadorDePelea = new ValidadorDePelea();
	validadorDePelea->validarPeleaDesdeParaLosPeronajes(pelea, validadorDePersonajes->getPersonajes());

	//COLOR  
	Json::Value color_alternativo;
	color_alternativo = raiz["color_alternativo"];

	validadorDelColor = new ValidadorDelColor();
	validadorDelColor->validarColorDesde(color_alternativo);
		
	//PARSEO DE LOS CONTROLES
	validadorDelControlador = new ValidadorDelControlador();

	//CONTROL 1
	Json::Value control_1;
	control_1 = raiz["control_1"];

	validadorDelControlador->validarControladorDesde(control_1);

	//CONTROL 2
	Json::Value control_2;
	control_2 = raiz["control_2"];

	validadorDelControlador->validarControladorDesde(control_2);

	//TOMA 1
	Json::Value combo_1;
	combo_1 = raiz["combo_1"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_1, NOMBRE_COMBO_1, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_1, NOMBRE_COMBO_1, validadorDelControlador->getControlador2()->getControladorDeTomas());
	
	//TOMA 2
	Json::Value combo_2;
	combo_2 = raiz["combo_2"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_2, NOMBRE_COMBO_2, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_2, NOMBRE_COMBO_2, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 3  
	Json::Value combo_3;
	combo_3 = raiz["combo_3"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_3, NOMBRE_COMBO_3, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_3, NOMBRE_COMBO_3, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 4  
	Json::Value combo_4;
	combo_4 = raiz["combo_4"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_4, NOMBRE_COMBO_4, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_4, NOMBRE_COMBO_4, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 5  
	Json::Value combo_5;
	combo_5 = raiz["combo_5"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_5, NOMBRE_COMBO_5, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_5, NOMBRE_COMBO_5, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 6 
	Json::Value combo_6;
	combo_6 = raiz["combo_6"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_6, NOMBRE_COMBO_6, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_6, NOMBRE_COMBO_6, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 7  
	Json::Value combo_7;
	combo_7 = raiz["combo_7"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_7, NOMBRE_COMBO_7, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_7, NOMBRE_COMBO_7, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 8  
	Json::Value combo_8;
	combo_8 = raiz["combo_8"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_8, NOMBRE_COMBO_8, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_8, NOMBRE_COMBO_8, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 9  
	Json::Value combo_9;
	combo_9 = raiz["combo_9"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_9, NOMBRE_COMBO_9, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_9, NOMBRE_COMBO_9, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 10  fatality 1
	Json::Value combo_10;
	combo_10 = raiz["combo_10"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_10, NOMBRE_COMBO_10, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_10, NOMBRE_COMBO_10, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 11  fatality 2
	Json::Value combo_11;
	combo_11 = raiz["combo_11"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_11, NOMBRE_COMBO_11, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_11, NOMBRE_COMBO_11, validadorDelControlador->getControlador2()->getControladorDeTomas());

	//TOMA 12  fatality 3
	Json::Value combo_12;
	combo_12 = raiz["combo_12"];

	validadorDeTomas = new ValidadorDeTomas();
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_12, NOMBRE_COMBO_12, validadorDelControlador->getControlador1()->getControladorDeTomas());
	validadorDeTomas->validarTomaDeseParaControladorDeTomas(combo_12, NOMBRE_COMBO_12, validadorDelControlador->getControlador2()->getControladorDeTomas());


	return true;
}

Escenario& Parser::getEscenario() const
{
	return *(validadorDeEscenario->getEscenario());
}

Ventana& Parser::getVentana() const
{
	return *(validadorDeVentana->getVentana());
}

Controlador* Parser::getControlador1(){
	return validadorDelControlador->getControlador1();
}

Controlador* Parser::getControlador2(){
	return validadorDelControlador->getControlador2();
}

Pelea* Parser::getPelea()const
{
	return validadorDePelea->getPelea();
}

std::string Parser::getPeleaComoString()const{
	return validadorDePelea->getPeleaComoString();
}

std::vector<Personaje*> Parser::getPersonajes() const
{
	return *(validadorDePersonajes->getPersonajes());
}

std::vector<Capa*> Parser::getCapas() const
{
	return *(validadorDeCapas->getCapas());

}
std::vector<double> Parser::getColorAlternativo()
{
	return validadorDelColor->getColorAlternativo();
}

void Parser::FreeInstancia()
{
	delete getInstancia().validadorDeVentana;
	delete  getInstancia().validadorDeEscenario;
	delete getInstancia().validadorDePersonajes;
	delete getInstancia().validadorDeCapas;
	delete getInstancia().validadorDelColor;
	delete getInstancia().validadorDePelea;
	//delete getInstancia().validadorDeTomas;
}

Parser::~Parser()
{
}
