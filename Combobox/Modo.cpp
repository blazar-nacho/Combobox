#include "stdafx.h"
#include "Modo.h"


Modo::Modo(Vista* unaVista)
{
	laVista = unaVista;
	modosDeJuego = std::vector<ModoDeJuego*>();
	modosDeJuego.push_back(new Modo1vs1());
	modosDeJuego.push_back(new Modo1vsCom());
	modosDeJuego.push_back(new ModoPractica());
}

Vista* Modo::getRefVista(){
	return laVista;
}

void Modo::seleccionarModo(MODOS_DE_JUEGO elModoActual, int unNivel){
	if (elModoActual == COM){
		Modo1vsCom* modoCom = (Modo1vsCom*)(modosDeJuego.at(COM));
		modoCom->setNivelDeDificultad(unNivel);
	}
	modoActual = elModoActual;
}


MODOS_DE_JUEGO Modo::getModoActual(){
	return modoActual;
}


Controlador* Modo::getControlador1(){
	return (modosDeJuego.at(modoActual)->getControlador1());
}


Controlador* Modo::getControlador2(){
	return (modosDeJuego.at(modoActual)->getControlador2());
}

void Modo::cambiarColorPersonajes(){


}


void Modo::setPersonajes(PJSELECCION personajes){

	delete personaje1Seleccionado;
	delete personaje2Seleccionado;
	personaje1Seleccionado = new Personaje();
	personaje2Seleccionado = new Personaje();

	std::vector<Personaje*> personajesParser = Parser::getInstancia().getPersonajes();

	std::string personajeUnoABuscar;
	std::string personajeDosABuscar;
		
	std::vector<double> colorPj1;
	std::vector<double> colorPj2;

	Personaje* unPersonaje;
	std::string unNombre;

	if (personajes.personajeUno == LIUKANGUNO){
		personajeUnoABuscar = "Liu Kang";
	//h_inicio 
		colorPj1.push_back(340);
	//h_final 
	colorPj1.push_back(5);
	}
	else{
		personajeUnoABuscar = "Scorpion";
		colorPj1.push_back(40);
		colorPj1.push_back(70);
	}

	if (personajes.personajeDos == LIUKANGDOS){
		personajeDosABuscar = "Liu Kang";
		colorPj2.push_back(340);
		colorPj2.push_back(5);
	}
	else{
		personajeDosABuscar = "Scorpion";
		colorPj2.push_back(40);
		colorPj2.push_back(70);
	}


	switch (personajes.colorPjUno){
	case (ROJO) :
		if (personajes.personajeUno == LIUKANGUNO)
		colorPj1.push_back(365);
		else
		colorPj1.push_back(304);
		break;
	case (VERDE) :
		if (personajes.personajeUno == LIUKANGUNO)
		colorPj1.push_back(110);
		else
		colorPj1.push_back(54);
		break;
	case (AZUL) :
		if (personajes.personajeUno == LIUKANGUNO)
			colorPj1.push_back(218);
		else
		colorPj1.push_back(162);
		break;
	case (VIOLETA) :
		if (personajes.personajeUno == LIUKANGUNO)
			colorPj1.push_back(246);
		else
		colorPj1.push_back(190);
		break;
	case (GRIS) :
		if (personajes.personajeUno == LIUKANGUNO)
			colorPj1.push_back(194);
		else
			colorPj1.push_back(138);
		break;
	case (NARANJA) :
		if (personajes.personajeUno == LIUKANGUNO)
			colorPj1.push_back(386);
		else
		colorPj1.push_back(330);
		break;
	case (AMARILLO) :
		if (personajes.personajeUno == LIUKANGUNO)
			colorPj1.push_back(56);
		else
		colorPj1.push_back(0);
		break;
	case (BASE) :
			colorPj1.push_back(0);
		break;
	}

	switch (personajes.colorPjDos){
	case (ROJODOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(365);
		else
		colorPj2.push_back(304);
		break;
	case (VERDEDOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(110);
		else
		colorPj2.push_back(54);
		break;
	case (AZULDOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(218);
		else
		colorPj2.push_back(162);
		break;
	case (VIOLETADOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(246);
		else
		colorPj2.push_back(190);
		break;
	case (GRISDOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(194);
		else
		colorPj2.push_back(138);
		break;
	case (NARANJADOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(386);
		else
		colorPj2.push_back(330);
		break;
	case (AMARILLODOS) :
		if (personajes.personajeDos == LIUKANGDOS)
			colorPj2.push_back(56);
		else
		colorPj2.push_back(0);
		break;
	case (BASEDOS) :
		colorPj2.push_back(0);
		break;
	}

	if (laVista->cambiaColorPersonaje())
		colorPj2.at(2) += 220;

	
	laVista->setColorPj1(colorPj1);
	laVista->setColorPj2(colorPj2);

	for (size_t i = 0; i < personajesParser.size(); i++){
		unPersonaje = personajesParser.at(i);
		unNombre = unPersonaje->getNombre();
		if (unNombre == personajeUnoABuscar){
			personaje1Seleccionado->copiarAtributosDe(unPersonaje);
			Parser::getInstancia().getPelea()->setPersonaje1(personaje1Seleccionado);
		}
		if (unNombre == personajeDosABuscar){
			personaje2Seleccionado->copiarAtributosDe(unPersonaje);
			Parser::getInstancia().getPelea()->setPersonaje2(personaje2Seleccionado);
		}
	}

	
}

void Modo::iniciarModo(){
	modosDeJuego.at(modoActual)->iniciar();
}


bool Modo::esModoPractica(){
	return (modosDeJuego.at(modoActual)->esModoPractica());
}


Modo::~Modo()
{
	for (size_t i = 0; i < modosDeJuego.size(); i++){
		delete modosDeJuego.at(i);
	}
	modosDeJuego.clear();
}
