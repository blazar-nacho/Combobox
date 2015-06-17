#include "stdafx.h"
#include "Mundo.h"
#include "Parser.h"

Mundo::Mundo()
{

}

Mundo::Mundo(const vector2D& valorGravedad)
{
	yPiso = Parser::getInstancia().getEscenario().getYPiso();
	gravedad = valorGravedad;
	Cuerpos = std::vector<Cuerpo*>();
	cambioGolpeAlto = false;
	unReloj = new Timer();
}

void Mundo::agregarCuerpo(Cuerpo *unCuerpo)
{
	Cuerpos.push_back(unCuerpo);
	ESTADO parado;
	parado.movimiento = PARADO;
	unCuerpo->SetSensorActivoStr(parado);
}

void Mundo::reiniciar(){
	vector2D vecGravedad(0.0f, GRAVEDAD_Y);
	yPiso = Parser::getInstancia().getEscenario().getYPiso();
	gravedad = vecGravedad;
	ESTADO parado;
	parado.movimiento = PARADO;
	//Cuerpo 1
	Cuerpos.at(0)->reiniciarAtributos();
	Cuerpos.at(0)->SetPosicion(vector2D((P1_POS_INI + Parser::getInstancia().getEscenario().getAncho() / 2), (Parser::getInstancia().getEscenario().getYPiso())));
	Cuerpos.at(0)->SetSensorActivoStr(parado);
	//Cuerpo2
	Cuerpos.at(1)->reiniciarAtributos();
	Cuerpos.at(1)->SetPosicion(vector2D((float)(P2_POS_INI + Parser::getInstancia().getEscenario().getAncho() / 2), (Parser::getInstancia().getEscenario().getYPiso())));
	Cuerpos.at(1)->SetSensorActivoStr(parado);
	cambioGolpeAlto = false;
	unReloj->stop();
}


bool Mundo::huboToma(){
	if (ultimaToma != nullptr) return true;
	return false;
}

Toma* Mundo::getToma(){
	return ultimaToma;
}

Cuerpo* Mundo::getCuerpo(size_t pos)
{
	if (pos < Cuerpos.size())
		return Cuerpos.at(pos);

	return nullptr;
}

Sensor* Mundo::getProyectil(size_t pos)
{
	if (pos == 1)
		return Cuerpos.at(0)->getSensoresProyectil().at(0);
	if (pos == 2)
		return Cuerpos.at(1)->getSensoresProyectil().at(0);
	return nullptr;
}

int Mundo::Paso(float difTiempo)
{
	ultimaToma = nullptr;
	int estadoJuego = CONTINUAR;
	for (unsigned int i = 0; i < Cuerpos.size(); i++)
	{
		estadoJuego = Cuerpos.at(i)->notificarObservadores(Resolver(difTiempo, Cuerpos.at(i)));
		if (estadoJuego != CONTINUAR)
			break;
	}
	return estadoJuego;
}

void Mundo::FrenarCuerpos()
{
	for (unsigned int i = 0; i < Cuerpos.size(); i++)
	{
		Cuerpos.at(i)->Frenar();
	}
}

void Mundo::limpiarCuerpos(){
	Cuerpos.clear();
}


void Mundo::LiberarCuerpos()
{
	for (unsigned int i = 0; i < Cuerpos.size(); i++)
	{
		Cuerpos.at(i)->Liberar();
	}
}

void Mundo::setModoActual(MODOS_DE_JUEGO unModoActual){
	modoActual = unModoActual;
}

std::pair<float, float> getPosicionAbsSensor(std::pair<float, float> posSensor, Cuerpo* unCuerpo, float anchoDelSensor, float altoDelSensor, bool invertido){
	Sprite* elSprite = unCuerpo->getSprite();

	std::pair<float, float> posicionOtroCuerpo;
	float posX, posY, posFinPj;

	ManejadorULogicas manejadorUnidades;

	if (!(invertido)){
		posX = manejadorUnidades.darLongUnidades(posSensor.first) + unCuerpo->getPosicion().x;
	}
	else{
		posFinPj = unCuerpo->getPosicion().x + unCuerpo->getRefPersonaje()->getAncho();
		posX = posFinPj - manejadorUnidades.darLongUnidades(posSensor.first) - manejadorUnidades.darLongUnidades(anchoDelSensor);
	}

	posY = unCuerpo->getPosicion().y + unCuerpo->getRefPersonaje()->getAlto() - manejadorUnidades.darLongUnidades(posSensor.second) - manejadorUnidades.darLongUnidades(altoDelSensor);
	posicionOtroCuerpo.first = posX;
	posicionOtroCuerpo.second = posY;
	return posicionOtroCuerpo;
}

bool Mundo::hayInterseccion(std::pair<float, float> unaPosicion, float unAncho, float unAlto, std::pair<float, float> otraPos, float otroAncho, float otroAlto){
	if ((unaPosicion.first + unAncho < otraPos.first) || (unaPosicion.first > otroAncho + otraPos.first) || (unaPosicion.second + unAlto < otraPos.second) || (unaPosicion.second > otraPos.second + otroAlto)){
		return false;
	}
	return true;
}




ESTADO Mundo::ResolverColisiones(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, bool invertido, ESTADO nuevoEstado){
	

	ESTADO estadoanterior = unCuerpo->getEstadoAnterior();

	std::vector<Sensor*>* sensoresCuerpo = unCuerpo->getSensores();
	std::vector<Sensor*>* sensoresOtroCuerpo = elOtroCuerpo->getSensores();

	std::pair<float, float> posAbsSensoresOtroCuerpo;
	std::pair<float, float> posAbsSensoresCuerpo;

	//si otrocuerpo esta haciendo algo y yo no estoy golpeado, evaluo
	if ((elOtroCuerpo->getEstado().accion != SIN_ACCION) && estadoanterior.golpeado == NOGOLPEADO){
		for (unsigned i = 0; i < sensoresCuerpo->size(); i++){
			for (unsigned j = 0; j < sensoresOtroCuerpo->size(); j++){
				ManejadorULogicas manejadorUnidades;
				posAbsSensoresOtroCuerpo = getPosicionAbsSensor(sensoresOtroCuerpo->at(j)->getPosicion(), elOtroCuerpo, sensoresOtroCuerpo->at(j)->getAncho(), sensoresOtroCuerpo->at(j)->getAlto(), !invertido);
				posAbsSensoresCuerpo = getPosicionAbsSensor(sensoresCuerpo->at(i)->getPosicion(), unCuerpo, sensoresCuerpo->at(i)->getAncho(), sensoresCuerpo->at(i)->getAlto(), invertido);


				if ((sensoresCuerpo->at(i)->getHitbox()) && !(sensoresOtroCuerpo->at(j)->getHitbox()) && hayInterseccion(posAbsSensoresCuerpo, manejadorUnidades.darLongUnidades(sensoresCuerpo->at(i)->getAncho()), manejadorUnidades.darLongUnidades(sensoresCuerpo->at(i)->getAlto()), posAbsSensoresOtroCuerpo, manejadorUnidades.darLongUnidades(sensoresOtroCuerpo->at(j)->getAncho()), manejadorUnidades.darLongUnidades(sensoresOtroCuerpo->at(j)->getAlto()))){
					//evalua varias veces pero...

					//aca entra solo una vez
					if (nuevoEstado.golpeado == NOGOLPEADO ){  
						nuevoEstado = ResolverGolpes(unCuerpo, elOtroCuerpo, invertido, nuevoEstado);
						break;
					}
					
				}
			}
		}
	}

	return nuevoEstado;
}


//determina si mi personaje es golpeado o no
// 4 cconsideraciones,   con que me golpearon?, que estado tengo? estoy en modo defensa???
ESTADO Mundo::ResolverGolpes(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, bool invertido, ESTADO nuevoEstado){

	ESTADO estadoAnterior = unCuerpo->getEstadoAnterior();
	ESTADO estadoEnemigo = elOtroCuerpo->getEstado();
	Sprite* elSprite = unCuerpo->getSprite();


	if (!unCuerpo->estaEnPiso()){ //si esta en el aire no tiene defensa,
		nuevoEstado.golpeado = GOLPEADO;

		if (estadoEnemigo.accion == GANCHO || (estadoEnemigo.accion == PATADA_ALTA && !(elOtroCuerpo->estaEnPiso()))){

			//aca hay que aplicar un impulso
			if (!(invertido)){
				unCuerpo->aplicarImpulso(vector2D(2*-SALTO_X, (0.5)*SALTO_Y));
				elOtroCuerpo->mover(3*DISTANCIA);
				
			}
			else{
				unCuerpo->aplicarImpulso(vector2D(2*SALTO_X, (0.5)*SALTO_Y));
				elOtroCuerpo->mover(3*-DISTANCIA);

			}

			if (!elOtroCuerpo->estaEnPiso()){
				elOtroCuerpo->SetVelocidadX(0);
				elOtroCuerpo->setDemoraAire(10);

			}

		}
		else if (estadoEnemigo.accion != GUARDIA && estadoEnemigo.accion!=BICICLETA){ // aca como no lo arroja el impulso tiene que ser un toque


			if (!(invertido)){
				unCuerpo->aplicarImpulso(vector2D(-SALTO_X, SALTO_Y / 10));
				elOtroCuerpo->mover(2*DISTANCIA);
				
				
			}
			else{
				unCuerpo->aplicarImpulso(vector2D(SALTO_X, SALTO_Y / 10));
				elOtroCuerpo->mover(2*-DISTANCIA);
				
				
			}


				
		}

	}
	else{ // esta en piso o agachado
		nuevoEstado.golpeado = GOLPEADO;
		if (estadoAnterior.movimiento == AGACHADO){ //no estoy teniendo en cuenta si son golpes fuertes o ganchos......

			if (estadoAnterior.accion == GUARDIA){ //poca demora, poco desplazamiento
				nuevoEstado.accion = GUARDIA;
				if (!(invertido)){
					unCuerpo->mover(2 * -DISTANCIA);
					elOtroCuerpo->mover(2 * DISTANCIA);
				}
				else{
					unCuerpo->mover(2 * DISTANCIA);
					elOtroCuerpo->mover(2 * -DISTANCIA);
					//unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
				}
			}
			else{

				if (estadoEnemigo.accion == GANCHO){

					//aca hay que aplicar un impulso
					if (!(invertido)){
						unCuerpo->aplicarImpulso(vector2D(2 * -SALTO_X, (0.5)*SALTO_Y));
						elOtroCuerpo->mover(3 * DISTANCIA);
					}
					else{
						unCuerpo->aplicarImpulso(vector2D(2 * SALTO_X, (0.5)*SALTO_Y));
						elOtroCuerpo->mover(3 * -DISTANCIA);
					}

				}


				else{

					if (estadoEnemigo.accion != BICICLETA){

						if (!(invertido)){
							unCuerpo->mover(3 * -DISTANCIA);
							elOtroCuerpo->mover(2 * DISTANCIA);
						}
						else{
							unCuerpo->mover(3 * DISTANCIA);
							elOtroCuerpo->mover(2 * -DISTANCIA);
							//unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
						}
					}
				}

			}
			nuevoEstado.movimiento = AGACHADO;
		}

		else // caso el tipo en el piso
		{

			nuevoEstado.golpeado = GOLPEADO;

			if (estadoAnterior.accion == GUARDIA){ //poca demora, poco desplazamiento
				nuevoEstado.accion = GUARDIA;
				if (!(invertido))
					unCuerpo->mover(-DISTANCIA);
				
				else
					unCuerpo->mover(DISTANCIA);
				//unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
			}
			else{// no esta en guardia analizar golpes
				
				if (estadoEnemigo.accion == GANCHO || (estadoEnemigo.accion == PATADA_ALTA && (!(elOtroCuerpo->estaEnPiso())))){

					//aca hay que aplicar un impulso
					if (!(invertido)){

						unCuerpo->aplicarImpulso(vector2D((0.5)*-SALTO_X, (0.5)*SALTO_Y));
					}
					else{
							unCuerpo->aplicarImpulso(vector2D((0.5)*SALTO_X, (0.5)*SALTO_Y));
					}
					if (!elOtroCuerpo->estaEnPiso()){
						elOtroCuerpo->SetVelocidadX(0);
						elOtroCuerpo->setDemoraAire(15);

					}
				}
				else if (estadoEnemigo.accion != GUARDIA && estadoEnemigo.accion != BICICLETA){ // aca como no lo arroja el impulso tiene que ser un toque

					if (!(invertido))
						unCuerpo->mover(3*-DISTANCIA);

					else
						unCuerpo->mover(3*DISTANCIA);
					//unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));

				}

			}

		}




	}

	//XJOSE 
	if (estadoEnemigo.accion == BICICLETA){

		//aca hay que aplicar un impulso
		if (!(invertido)){
			if (estadoAnterior.accion != GUARDIA){
				unCuerpo->mover(4 * -DISTANCIA);
			}
	

		}
		else{
			if (estadoAnterior.accion != GUARDIA){
				unCuerpo->mover(4 * DISTANCIA);
			}
			
		}
	}
	
	//XJOSE
	if (estadoEnemigo.accion == FLYKICK){

		//aca hay que aplicar un impulso
		if (!(invertido)){
			
				unCuerpo->mover( -DISTANCIA);
				elOtroCuerpo->aplicarImpulso(vector2D(7 * SALTO_X, 0));
			


		}
		else{
			
				unCuerpo->mover( DISTANCIA);
				elOtroCuerpo->aplicarImpulso(vector2D(7 * -SALTO_X, 0));
			

		}
	}











	
	return nuevoEstado;
}

ESTADO Mundo::moverCuerpos(Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, bool invertido, std::vector<Movimiento*>* movimientos, ESTADO nuevoEstado) {


	if (invertido){
		if (unCuerpo->getEstado().movimiento != CAMINARIZQ){
			nuevoEstado.movimiento = CAMINARDER;

			if ((movimientos->back()->getMovimiento() == DEFENSA) || (movimientos->back()->getMovimiento() == DEFENSA_AGACHADO)){
					unCuerpo->mover(-0.1f);
					elOtroCuerpo->mover(0.1f);
			}

			else{
					unCuerpo->mover(4*DISTANCIA);
					elOtroCuerpo->mover(4*-DISTANCIA);
			}
		}
	}
	else{
		if (unCuerpo->getEstado().movimiento != CAMINARDER){
			nuevoEstado.movimiento = CAMINARIZQ;

			if ((movimientos->back()->getMovimiento() == DEFENSA) || (movimientos->back()->getMovimiento() == DEFENSA_AGACHADO)){
					unCuerpo->mover(0.1f);
					elOtroCuerpo->mover(-0.1f);
			}
			else{
					unCuerpo->mover(4*-DISTANCIA);
					elOtroCuerpo->mover(4*DISTANCIA);
			}

		}
	}


	return nuevoEstado;
}



//logica de saltos
//el tipo en el aire -----> mantenerle el estado de salto, sacarle velocidad si se esta por chocar y la logica de bordes
//el tipo tocando piso recien -->chequear superposicion
ESTADO Mundo::ResolverSaltos(float difTiempo, Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos){

	ESTADO estadoAnterior = unCuerpo->getEstadoAnterior();


	// si no esta en piso, mantenele el movimiento anterior
	if (!unCuerpo->estaEnPiso()){
		nuevoEstado.movimiento = estadoAnterior.movimiento;  //si el flaco metio una accion en el aire, esta tiene una demora mas chica aplicada que es la que lo mantiene
		//si fue golpeado en el aire antes deberia mantenerlo una demora y si fue recien nose....

	}

	// si el flaco acaba de llegar al piso y estaba haciendo algo en el aire pero no esta golpeado entonces suspendele la accion cuando llega al piso (evito patada voladora en piso)
	if (unCuerpo->estaEnPiso() && (estadoAnterior.movimiento == SALTO || estadoAnterior.movimiento == SALTODIAGDER || estadoAnterior.movimiento == SALTODIAGIZQ)){

		if ((estadoAnterior.accion != SIN_ACCION) && nuevoEstado.golpeado == NOGOLPEADO){
			unCuerpo->setDemora(0);
		}

	}

	return nuevoEstado;
}




ESTADO Mundo::ResolverAcciones(float difTiempo, Cuerpo *unCuerpo, Cuerpo* otroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos)
{
	ESTADO estadoAnterior = unCuerpo->getEstadoAnterior();
	Sprite* elSprite = unCuerpo->getSprite();



	//
	//*************************************************************************************************//
	//                                EL EL AIRE SOLO SE PUEDEN REALIZAR 4 GOLPES
	//
	// SEBA, EVALUA QUE SPRITE APLICAR dependiendo de si estado.movimiento es (SALTO) o  (SALTODIAGDER SALTODIAGIZQ)
	//************************************************************************************************//
	if (!unCuerpo->estaEnPiso())
	{


		if (movimientos->back()->getMovimiento() == G_BAJO){
			nuevoEstado.accion = GOLPE_BAJO;
			unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));

		}
		if (movimientos->back()->getMovimiento() == P_ALTA) {
			nuevoEstado.accion = PATADA_ALTA;
			unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
		}

		if (movimientos->back()->getMovimiento() == P_BAJA) {
			nuevoEstado.accion = PATADA_BAJA;
			unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
		}

		if (movimientos->back()->getMovimiento() == G_ALTO){
			nuevoEstado.accion = GOLPE_ALTO1;
			unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
		}


	}





	//*************************************************************************************************//
	//                               ESTA EN EL PISO, EVALUO AGACHADO Y PARADO
	//
	//************************************************************************************************//
	else{

		//*************************************************************************************************//
		//                            CASO TIPO AGACHADO
		//
		//************************************************************************************************//
		if (estadoAnterior.movimiento == AGACHADO)
		{

			if (movimientos->back()->getMovimiento() == ABAJO) {
				nuevoEstado.movimiento = AGACHADO;
			}

			if (movimientos->back()->getMovimiento() == DEFENSA_AGACHADO){
				nuevoEstado.movimiento = AGACHADO;
				nuevoEstado.accion = GUARDIA;
				//unCuerpo->setDemora((elSprite->getConstantes(unCuerpo->getEstado()))*(elSprite->listaDeCuadros(unCuerpo->getEstado())->size()));
			}

			//*************************************************************************************************//
			//                                GOLPES AGACHADO
			//************************************************************************************************//

			if (movimientos->back()->getMovimiento() == G_ABAJO){

				if (estadoAnterior.accion != GOLPE_BAJO){
					nuevoEstado.movimiento = AGACHADO;
					nuevoEstado.accion = GOLPE_BAJO;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
				}
			}
			if (movimientos->back()->getMovimiento() == P_ALTA_ABAJO) {

				if (estadoAnterior.accion != PATADA_ALTA){

					nuevoEstado.movimiento = AGACHADO;
					nuevoEstado.accion = PATADA_ALTA;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
				}
			}

			if (movimientos->back()->getMovimiento() == P_BAJA_ABAJO) {

				if (estadoAnterior.accion != PATADA_BAJA){
					nuevoEstado.movimiento = AGACHADO;
					nuevoEstado.accion = PATADA_BAJA;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
				}
			}

			if (movimientos->back()->getMovimiento() == G_GANCHO){

				if (estadoAnterior.accion != GANCHO){
					nuevoEstado.movimiento = AGACHADO;
					nuevoEstado.accion = GANCHO;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
				}
			}






		}

		//*************************************************************************************************//
		//                            CASO TIPO PARADO
		//
		//************************************************************************************************//
		else
		{

				//DESPLAZAMIENTOS

				if ((movimientos->back()->getMovimiento() == DER)){
					nuevoEstado.movimiento = CAMINARDER;

					if (!(invertido)){
						if (!unCuerpo->EstaSuperpuesto()){
								unCuerpo->mover(DISTANCIA);
						}
						else{//esta superpueso
							if (!otroCuerpo->estaEnBorde()){
								if (!otroCuerpo->getEstado().accion == GUARDIA){
										unCuerpo->mover(DISTANCIA);
										otroCuerpo->mover(DISTANCIA);
								}
								else{
										unCuerpo->mover(DISTANCIA / 10);
										otroCuerpo->mover(DISTANCIA / 10);
								}
							}

						}
					}//no esta invertido
					else
							unCuerpo->mover(DISTANCIA*FACTOR_DIST_REVERSA);
				}


				if ((movimientos->back()->getMovimiento() == IZQ)){
					nuevoEstado.movimiento = CAMINARIZQ;
					if (!invertido){
						if (!unCuerpo->EstaSuperpuesto()){
								unCuerpo->mover(-DISTANCIA*FACTOR_DIST_REVERSA);
						}
						else{
							if (!otroCuerpo->estaEnBorde()){
								if (!otroCuerpo->getEstado().accion == GUARDIA){
										unCuerpo->mover(-DISTANCIA*FACTOR_DIST_REVERSA);
										otroCuerpo->mover(DISTANCIA*FACTOR_DIST_REVERSA);
								}
								else{
										unCuerpo->mover(-DISTANCIA*FACTOR_DIST_REVERSA / 10);
										otroCuerpo->mover(DISTANCIA*FACTOR_DIST_REVERSA / 10);

								}
							}
							else{
									unCuerpo->mover(-DISTANCIA*FACTOR_DIST_REVERSA);
							}
						}
					}// el tipo no esta adelante mio
					else{
						if (!unCuerpo->EstaSuperpuesto()){
								unCuerpo->mover(-DISTANCIA);
						}
						else{

							if (!otroCuerpo->estaEnBorde()){
								if (!otroCuerpo->getEstado().accion == GUARDIA){
										unCuerpo->mover(-DISTANCIA);
										otroCuerpo->mover(-DISTANCIA);
								}
								else{
										unCuerpo->mover(-DISTANCIA / 10);
										otroCuerpo->mover(-DISTANCIA / 10);

								}
							}
							else{
								if (!otroCuerpo->estaEnBorde()) unCuerpo->mover(DISTANCIA);
							}
						}//cierra else

					}
				}
			


			//SALTOS

			if (movimientos->back()->getMovimiento() == ARRIBA) {
				nuevoEstado.movimiento = SALTO;
				unCuerpo->aplicarImpulso(vector2D(0.0f, SALTO_Y));
			}


			if (movimientos->back()->getMovimiento() == SALTODER){
				nuevoEstado.movimiento = SALTODIAGDER;
				unCuerpo->aplicarImpulso(vector2D(SALTO_X, SALTO_Y));
			}

			if (movimientos->back()->getMovimiento() == SALTOIZQ){
				nuevoEstado.movimiento = SALTODIAGIZQ;
				unCuerpo->aplicarImpulso(vector2D(-SALTO_X, SALTO_Y));
			}

			//AGACHARSE

			if (movimientos->back()->getMovimiento() == ABAJO){
				nuevoEstado.movimiento = AGACHADO;
			}

			if (movimientos->back()->getMovimiento() == DEFENSA_AGACHADO) { //ESTO ESTA ASI A PROPOSITO. en el mk si esta bloqueado y se agacha, tarda en bloquearse agachado
				nuevoEstado.movimiento = AGACHADO;
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
			}

			// GOLPES


			// aca no deberia ir (! arma arrojable) por que deberia tener una demora que le impide entrar a este resolver
			// pero como no sabemos que demora va a tener el arma por que le van a aumentar la velocidad.... dejemos la redundancia por las dudas

			if (movimientos->back()->getMovimiento() == DEFENSA) {
				nuevoEstado.accion = GUARDIA;
				
			}


			if (movimientos->back()->getMovimiento() == G_BAJO){
				nuevoEstado.accion = GOLPE_BAJO;
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
			}
			if (movimientos->back()->getMovimiento() == P_ALTA) {
				nuevoEstado.accion = PATADA_ALTA;
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
			}

			if (movimientos->back()->getMovimiento() == P_BAJA) {
				nuevoEstado.accion = PATADA_BAJA;
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
			}
			if (movimientos->back()->getMovimiento() == G_ALTO) {

				nuevoEstado.accion = GOLPE_ALTO1;
				
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
				

				/*
				if (cambioGolpeAlto){

					nuevoEstado.accion = GOLPE_ALTO1;
					cambioGolpeAlto = false;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
					unCuerpo->setDemoraGolpe(0);
					
					
				}
				
				else{
					nuevoEstado.accion = GOLPE_ALTO2;
					cambioGolpeAlto = true;
					unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
					unCuerpo->setDemoraGolpe((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size())+20);
				}
				*/


			}

			

			////////////////////////////////////////////////////////////////////////////////////////////
			//TOMAS

			Toma* unaToma;
			unaToma = unCuerpo->getControlador()->getControladorDeTomas()->getTomaSegunOrientacion(invertido);
			
			if (unaToma != nullptr){
				nuevoEstado = ResolverTomas(difTiempo, unCuerpo, otroCuerpo, nuevoEstado, invertido, movimientos, unaToma);
			}


		}


	}

	return nuevoEstado;
}

ESTADO Mundo::ResolverTomas(float difTiempo, Cuerpo *unCuerpo, Cuerpo* otroCuerpo, ESTADO nuevoEstado, bool invertido, std::vector<Movimiento*>* movimientos, Toma* unaToma){

	ESTADO estadoAnterior = unCuerpo->getEstadoAnterior();
	Sprite* elSprite = unCuerpo->getSprite();

	//COMBO 1---PODER (ARMA ARROJABLE)
	if ((unaToma->getNombre() == NOMBRE_COMBO_1) && !(unCuerpo->getEstado().accion == ARMA_ARROJABLE)){
		ultimaToma = unaToma;
		nuevoEstado.accion = ARMA_ARROJABLE;
		unCuerpo->getSensoresProyectil().at(0)->activarSensor();
		unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
	}


	//COMBO 4---TEST
	if ((unaToma->getNombre() == NOMBRE_COMBO_4) ){
		ultimaToma = unaToma;

		nuevoEstado.golpeado = DIZZY;
		unCuerpo->setDemora(5*(elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
		

	}
	
	

	//COMBO 5--- BICICLETA
	if (unaToma->getNombre() == NOMBRE_COMBO_5 && (unCuerpo->getRefPersonaje()->getNombre() == "Liu Kang")){
		ultimaToma = unaToma;
		nuevoEstado.accion = BICICLETA;

		if (!invertido){
			
			unCuerpo->aplicarImpulso(vector2D(SALTO_X, 7 * SALTO_Y));
		}
		else {

			unCuerpo->aplicarImpulso(vector2D(-SALTO_X, 7 * SALTO_Y));
		}
		unCuerpo->setDemora(2 * (elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
		unCuerpo->setInvertidoAux(invertido);
	}

	//COMBO 6--- FLYKICK
	if ((unaToma->getNombre() == NOMBRE_COMBO_6) && (unCuerpo->getRefPersonaje()->getNombre() == "Liu Kang")){
		ultimaToma = unaToma;
		nuevoEstado.accion = FLYKICK;

		if (!invertido){

			unCuerpo->aplicarImpulso(vector2D(SALTO_X, 7* SALTO_Y));
		}
		else {

			unCuerpo->aplicarImpulso(vector2D(-SALTO_X,7 * SALTO_Y));
		}
		unCuerpo->setInvertidoAux(invertido);
		unCuerpo->setDemora(6 * (elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()));
	}



	//COMBO 9 - FATALITY ARCADE
	if ((unaToma->getNombre() == NOMBRE_COMBO_9) && (otroCuerpo->getEstado().golpeado == DIZZY || otroCuerpo->getEstadoAnterior().golpeado == DIZZY)){
		ultimaToma = unaToma;

		nuevoEstado.accion = FATALITY_RUN;
		nuevoEstado.fatality = ARCADE;
		unCuerpo->setDemora(INT_MAX);
		otroCuerpo->setDemora(INT_MAX);
	}

	//COMBO 10 - FATALITY FIRE
	if ((unaToma->getNombre() == NOMBRE_COMBO_10) && (unCuerpo->getRefPersonaje()->getNombre() == "Scorpion") && (otroCuerpo->getEstado().golpeado == DIZZY || otroCuerpo->getEstadoAnterior().golpeado == DIZZY)){
		ultimaToma = unaToma;

		nuevoEstado.accion = FATALITY_RUN;
		nuevoEstado.fatality = FIRE;
		unCuerpo->setDemora(INT_MAX);
		otroCuerpo->setDemora(INT_MAX);
	}

	//COMBO 11 - FATALITY GODHAND
	if ((unaToma->getNombre() == NOMBRE_COMBO_11) && (unCuerpo->getRefPersonaje()->getNombre() == "Scorpion") && (otroCuerpo->getEstado().golpeado == DIZZY || otroCuerpo->getEstadoAnterior().golpeado == DIZZY)){
		ultimaToma = unaToma;

		nuevoEstado.accion = FATALITY_RUN;
		nuevoEstado.fatality = GODHAND;
		unCuerpo->setDemora(INT_MAX);
		otroCuerpo->setDemora(INT_MAX);
	}

	//COMBO 12 - FATALITY FIREKANG
	if ((unaToma->getNombre() == NOMBRE_COMBO_12) && (unCuerpo->getRefPersonaje()->getNombre() == "Liu Kang") && (otroCuerpo->getEstado().golpeado == DIZZY || otroCuerpo->getEstadoAnterior().golpeado == DIZZY)){
		ultimaToma = unaToma;

		nuevoEstado.accion = FATALITY_RUN;
		nuevoEstado.fatality = FIREKANG;
		unCuerpo->setDemora(INT_MAX);
		otroCuerpo->setDemora(INT_MAX);
	}



	/*
	if ((unaToma->getNombre() == NOMBRE_COMBO_2) && !(unCuerpo->getEstado().accion == TELETRANSPORTACION)){
	nuevoEstado.accion = TELETRANSPORTACION;
	vector2D unVector = otroCuerpo->getPosicion();
	if (!invertido){
	unCuerpo->SetPosicion(unVector);
	}
	else {
	unVector.x -= 10;
	unCuerpo->SetPosicion(unVector);
	}
	//implementarlo cuando esten los sprites
	}
	*/
		

	return nuevoEstado;
}
             
ESTADO Mundo::ResolverDemorasEspeciales(float difTiempo, Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, ESTADO nuevoEstado, bool invertido){
	Sprite* elSprite = unCuerpo->getSprite();

	if (nuevoEstado.accion == BICICLETA)
	{
		if (!unCuerpo->GetInvertidoAux()){
			unCuerpo->SetVelocidad(vector2D(SALTO_X / 6, -gravedad.y * difTiempo));
			if (elOtroCuerpo->getEstadoAnterior().golpeado != NOGOLPEADO){
				unCuerpo->SetVelocidadX(2 * DISTANCIA);
			}

		}
		else {
			unCuerpo->SetVelocidad(vector2D(-SALTO_X / 6, -gravedad.y * difTiempo));
			if (elOtroCuerpo->getEstadoAnterior().golpeado != NOGOLPEADO){
				unCuerpo->SetVelocidadX(2 * -DISTANCIA);
			}

		}
		//......
		if (elOtroCuerpo->estaEnBorde() && (elOtroCuerpo->getEstado().golpeado == GOLPEADO))
		{
			unCuerpo->SetVelocidadX(0);
		}

		if (elOtroCuerpo->getEstado().golpeado == GOLPEADO && elOtroCuerpo->getEstado().accion == GUARDIA)
		{
			unCuerpo->SetVelocidadX(0);
		}
		//...
		// si la demora esta por terminar
		if (unCuerpo->GetDemora() < (0.3*(elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()))){

			unCuerpo->SetVelocidadY(-20.f);
		}

	}

	//tomas 6

	if (nuevoEstado.accion == FLYKICK)
	{
		if (!unCuerpo->GetInvertidoAux()){  //caso invertido


			if (elOtroCuerpo->getEstado().golpeado != NOGOLPEADO){ //si te pegue

				unCuerpo->SetVelocidad(vector2D(0, 0));


				unCuerpo->setDemora((0.3*(elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size())));

			}

			else{  //si no te pegue

				unCuerpo->SetVelocidad(vector2D(SALTO_X / 6, -gravedad.y * difTiempo));

			}



		}
		else {

			if (elOtroCuerpo->getEstado().golpeado != NOGOLPEADO){
				unCuerpo->SetVelocidad(vector2D(0, 0));
				unCuerpo->setDemora((0.3*(elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size())));

			}

			else{

				unCuerpo->SetVelocidad(vector2D(-SALTO_X / 6, -gravedad.y * difTiempo));

			}

		}

		//......
		//if (elOtroCuerpo->estaEnBorde() && (elOtroCuerpo->getEstado().golpeado == GOLPEADO))
		//{
		//	unCuerpo->SetVelocidad(vector2D(0, 0));
		//}

		//if (elOtroCuerpo->getEstado().golpeado == GOLPEADO && elOtroCuerpo->getEstado().accion == GUARDIA)
		//{
		//		unCuerpo->SetVelocidad(vector2D(0, 0));
		//}


		//...
		// si la demora esta por terminar
		if (unCuerpo->GetDemora() < (0.1*(elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros((nuevoEstado))->size()))){

			unCuerpo->SetVelocidadY(-40.f);
			if (unCuerpo->estaEnPiso()) unCuerpo->setDemora(0);
		}

	}




	return nuevoEstado;
}

bool Mundo::haySuperposicion(Cuerpo *unCuerpo, Cuerpo *elOtroCuerpo, bool invertido)
{
	std::vector<Sensor*>* sensoresCuerpo = unCuerpo->getSensores();
	std::vector<Sensor*>* sensoresOtroCuerpo = elOtroCuerpo->getSensores();

	std::pair<float, float> posAbsSensoresOtroCuerpo;
	std::pair<float, float> posAbsSensoresCuerpo;
	for (unsigned i = 0; i < sensoresCuerpo->size(); i++){
		for (unsigned j = 0; j < sensoresOtroCuerpo->size(); j++){
			ManejadorULogicas manejadorUnidades;
			posAbsSensoresOtroCuerpo = getPosicionAbsSensor(sensoresOtroCuerpo->at(j)->getPosicion(), elOtroCuerpo, sensoresOtroCuerpo->at(j)->getAncho(), sensoresOtroCuerpo->at(j)->getAlto(), !invertido);
			posAbsSensoresCuerpo = getPosicionAbsSensor(sensoresCuerpo->at(i)->getPosicion(), unCuerpo, sensoresCuerpo->at(i)->getAncho(), sensoresCuerpo->at(i)->getAlto(), invertido);
			if (hayInterseccion(posAbsSensoresCuerpo, manejadorUnidades.darLongUnidades(sensoresCuerpo->at(i)->getAncho()), manejadorUnidades.darLongUnidades(sensoresCuerpo->at(i)->getAlto()), posAbsSensoresOtroCuerpo, manejadorUnidades.darLongUnidades(sensoresOtroCuerpo->at(j)->getAncho()), manejadorUnidades.darLongUnidades(sensoresOtroCuerpo->at(j)->getAlto()))){
				return true;
			}
		}
	}
	return false;
}


ESTADO Mundo::ResolverArma(Cuerpo* elOtroCuerpo, Cuerpo* unCuerpo, Sensor* proyectil, bool invertido, ESTADO nuevoEstado){
	Sprite* elSprite = unCuerpo->getSprite();
	ManejadorULogicas manejadorUnidades;

	std::vector<Sensor*>* sensoresCuerpo = unCuerpo->getSensores();
	std::pair<float, float> posAbsSensorProyectil;
	std::pair<float, float> posAbsSensoresCuerpo;

	std::pair<float, float> posProyectilEngloba;

	posProyectilEngloba.first = elOtroCuerpo->getXProyectilAnterior();
	posProyectilEngloba.second = manejadorUnidades.darLongPixels(elOtroCuerpo->getPosicion().y + (3 / 4)*elOtroCuerpo->getRefPersonaje()->getAlto());

	float anchoEngloba = proyectil->getPosicion().first - elOtroCuerpo->getXProyectilAnterior() + proyectil->getAncho();

	posAbsSensorProyectil = getPosicionAbsSensor(posProyectilEngloba, elOtroCuerpo, anchoEngloba, proyectil->getAlto(), !invertido);

	for (unsigned j = 0; j < sensoresCuerpo->size(); j++){
		posAbsSensoresCuerpo = getPosicionAbsSensor(sensoresCuerpo->at(j)->getPosicion(), unCuerpo, sensoresCuerpo->at(j)->getAncho(), sensoresCuerpo->at(j)->getAlto(), invertido);
		if (hayInterseccion(posAbsSensorProyectil, manejadorUnidades.darLongUnidades(anchoEngloba), manejadorUnidades.darLongUnidades(proyectil->getAlto()), posAbsSensoresCuerpo, manejadorUnidades.darLongUnidades(sensoresCuerpo->at(j)->getAncho()), manejadorUnidades.darLongUnidades(sensoresCuerpo->at(j)->getAlto()))){
			nuevoEstado.golpeado = GOLPEADO;
			unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
			proyectil->desactivarSensor();

			//aplico reacciones
			//////////////////////////////////

			if (!(invertido)){
				
				unCuerpo->mover(5 * -DISTANCIA);

			}
			else{
				
				unCuerpo->mover(5 * DISTANCIA);

			}
			////////////////////////////////////




			break;
		}
	}
	return nuevoEstado;
}

//Solución del choque entre proyectiles.
void Mundo::resolverChoque(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, Sensor* proyectilUno, Sensor* proyectilDos, bool invertido){

	ManejadorULogicas manejadorUnidades;

	std::pair<float, float> posAbsSensorProyectil1;
	std::pair<float, float> posAbsSensorProyectil2;

	std::pair<float, float> posProyectilEngloba1;
	std::pair<float, float> posProyectilEngloba2;

	posProyectilEngloba1.first = unCuerpo->getXProyectilAnterior();
	posProyectilEngloba1.second = proyectilUno->getPosicion().second;

	posProyectilEngloba2.first = elOtroCuerpo->getXProyectilAnterior();
	posProyectilEngloba2.second = proyectilDos->getPosicion().second;

	float anchoEngloba1 = proyectilUno->getPosicion().first - unCuerpo->getXProyectilAnterior() + proyectilUno->getAncho();
	float anchoEngloba2 = proyectilDos->getPosicion().first - elOtroCuerpo->getXProyectilAnterior() + proyectilDos->getAncho();

	posAbsSensorProyectil1 = getPosicionAbsSensor(posProyectilEngloba1, unCuerpo, anchoEngloba1, proyectilUno->getAlto(), invertido);
	posAbsSensorProyectil2 = getPosicionAbsSensor(posProyectilEngloba2, elOtroCuerpo, anchoEngloba2, proyectilDos->getAlto(), !invertido);

	if (hayInterseccion(posAbsSensorProyectil1, manejadorUnidades.darLongUnidades(anchoEngloba1), manejadorUnidades.darLongUnidades(proyectilUno->getAlto()), posAbsSensorProyectil2, manejadorUnidades.darLongUnidades(anchoEngloba2), manejadorUnidades.darLongUnidades(proyectilDos->getAlto()))){
		unCuerpo->getSensoresProyectil().at(0)->desactivarSensor();
		elOtroCuerpo->getSensoresProyectil().at(0)->desactivarSensor();
	}
}


//resuelve todo
//para entrar aca ninguno de los 2 puede estar golpeado y al menos uno tiene que estar hacieno algo
ESTADO Mundo::ResolverAtaques(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, ESTADO nuevoEstado, Sensor* proyectilUno, Sensor* proyectilDos, bool invertido){

	//**********************************************************
	// resuelve proyeciles

	if ((proyectilUno->estaActivo()) && (proyectilDos->estaActivo())){
		resolverChoque(unCuerpo, elOtroCuerpo, proyectilUno, proyectilDos, invertido);
	}
	else {

		if (proyectilDos->estaActivo()){
			nuevoEstado = ResolverArma(elOtroCuerpo, unCuerpo, proyectilDos, invertido, nuevoEstado);
		}
	}

	//********************************
	//resuelve golpes
	nuevoEstado = Mundo::ResolverColisiones(unCuerpo, elOtroCuerpo, invertido, nuevoEstado);

	return nuevoEstado;
	}

void Mundo::setResolver(ESTADO resolverNuevo, Cuerpo* refCuerpo)
{
	estadoResolver = resolverNuevo;
	resolverSeteado = true;
	resolverRefCuerpo = refCuerpo;
}


ESTADO Mundo::ResolverBatalla(Cuerpo* unCuerpo, Cuerpo* elOtroCuerpo, ESTADO nuevoEstado, bool invertido, bool SinVida){
	Sprite* elSprite = unCuerpo->getSprite();
	ESTADO estadoanterior = unCuerpo->getEstadoAnterior();

	//CASO 1 SIN TIEMPO
	//Si me quede sin tiempo resuelvo quien gano el round
		
	if (Parser::getInstancia().getPelea()->getSegundosTranscurridosDelRound() >= Parser::getInstancia().getPelea()->getTiempoDelRound()){
		int numeroDeRound = Parser::getInstancia().getPelea()->getRoundActual()->getNumeroDeRound();
		std::stringstream stream;
		Personaje* personaje = Parser::getInstancia().getPelea()->getPersonajeGanador();
		stream << numeroDeRound;

		if (unCuerpo->getRefPersonaje()->getVida() < elOtroCuerpo->getRefPersonaje()->getVida()) {
			Log::getInstancia().logearMensajeEnModo("Round " + stream.str() + " para " + elOtroCuerpo->getRefPersonaje()->getNombreActual(), Log::MODO_DEBUG);
			Parser::getInstancia().getPelea()->personajeGanoElRound(elOtroCuerpo->getRefPersonaje());
		}

		if (unCuerpo->getRefPersonaje()->getVida() > elOtroCuerpo->getRefPersonaje()->getVida()) {
			Log::getInstancia().logearMensajeEnModo("Round " + stream.str() + " para " + unCuerpo->getRefPersonaje()->getNombreActual(), Log::MODO_DEBUG);
			Parser::getInstancia().getPelea()->personajeGanoElRound(unCuerpo->getRefPersonaje());
		}

		nuevoEstado.golpeado = FALLECIDO_ROUND;
	}

	




	// CASO 2 ALGUIEN SE QUEDO SIN VIDA 

	//xjose, guarda, aca entra una sola vez, una vez que estas en dizzy si le vuelven a pegar va a descontar vida y esto va a dar true
	// hay que agregar algo mas
	//estado.anterior!= dizzy   ??????

	//entra solo una vez
	if (SinVida && !Parser::getInstancia().getPelea()->terminoLaPelea()){
		
		//logueo
		int numeroDeRound = Parser::getInstancia().getPelea()->getRoundActual()->getNumeroDeRound();
		std::stringstream stream;
		stream << numeroDeRound;
		Log::getInstancia().logearMensajeEnModo("Round " + stream.str() + " para " + elOtroCuerpo->getRefPersonaje()->getNombreActual(), Log::MODO_DEBUG);

		//notifico quien gano y determina si termino la pelea
		Parser::getInstancia().getPelea()->personajeGanoElRound(elOtroCuerpo->getRefPersonaje());

		if (!Parser::getInstancia().getPelea()->terminoLaPelea())
		nuevoEstado.golpeado = FALLECIDO_ROUND;
	}
		
		
	if (Parser::getInstancia().getPelea()->terminoLaPelea() && estadoanterior.accion != FATALITY_RUN){


			//asigno
			if (Parser::getInstancia().getPelea()->getPersonajeGanador() != nullptr){
				
				//esto es para que no entre el otro personaje
				if (nuevoEstado.golpeado == GOLPEADO   && estadoanterior.golpeado != DIZZY && estadoanterior.golpeado != TUMBANDOSE && estadoanterior.golpeado != TUMBADO){
				
					nuevoEstado.golpeado = DIZZY;
					unCuerpo->setDemora(5 * (elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size()));
				}
				else{ 
					if (estadoanterior.golpeado == TUMBANDOSE && nuevoEstado.golpeado == NOGOLPEADO){
						
						nuevoEstado.golpeado = TUMBADO;
						unCuerpo->setDemora(10000);
					}

					if (estadoanterior.golpeado == TUMBADO && nuevoEstado.golpeado == NOGOLPEADO){

						nuevoEstado.golpeado = FALLECIDO;
					}

					if (nuevoEstado.golpeado == GOLPEADO && estadoanterior.golpeado != TUMBADO && estadoanterior.golpeado != TUMBANDOSE){
						
						nuevoEstado.golpeado = TUMBANDOSE;
						unCuerpo->setDemora(10000);

					}

					if (estadoanterior.accion == FATALITY_END && nuevoEstado.accion!=FATALITY_END){
						nuevoEstado.golpeado = FALLECIDO;
						
					}
					
				}


			}
			else{
			Log::getInstancia().logearMensajeEnModo("Pelea empatada", Log::MODO_DEBUG);
			nuevoEstado.golpeado = FALLECIDO;
			}
		}



		/*
		
		if (unReloj->getTicks() >= TIEMPO_DIZZY){
		unReloj->stop();
		*/


		//}unReloj->start();

		/*
		if (estadoanterior.golpeado == DIZZY ){
		if (unReloj->getTicks() >= TIEMPO_DIZZY){
		unReloj->stop();
		Log::getInstancia().logearMensajeEnModo("Gano personaje " + Parser::getInstancia().getPelea()->getPersonajeGanador()->getNombreActual(), Log::MODO_DEBUG);
		nuevoEstado.golpeado = FALLECIDO;
		}


		}/*
		if (estadoanterior.golpeado == DIZZY || nuevoEstado.golpeado == DIZZY){
		if (unReloj->getTicks() < TIEMPO_DIZZY){
		nuevoEstado.movimiento = PARADO;
		nuevoEstado.accion = SIN_ACCION;
		nuevoEstado.golpeado = DIZZY;
		}
		else{
		unReloj->stop();
		Log::getInstancia().logearMensajeEnModo("Gano personaje " + Parser::getInstancia().getPelea()->getPersonajeGanador()->getNombreActual(), Log::MODO_DEBUG);
		nuevoEstado.golpeado = FALLECIDO;
		}
		}*/


	

	return nuevoEstado;
}


ESTADO Mundo::Resolver(float difTiempo, Cuerpo *unCuerpo)
{
	ESTADO estadoanterior = unCuerpo->getEstadoAnterior();
	ESTADO nuevoEstado;  //defino estado por defecto Si no es golpeado, si no vas a hacer nada y si no estas en el aire, devuelve esto
	nuevoEstado.movimiento = PARADO;
	nuevoEstado.accion = SIN_ACCION;
	nuevoEstado.golpeado = NOGOLPEADO;
	bool SinVida = false;
	////////////////////////////////////////////
	/*
	cambioGolpeAlto = false;
	if (unCuerpo->HayDemoraGolpe())
	{
		unCuerpo->DisminuirDemoraGolpe();
		if (unCuerpo->HayDemoraGolpe()){

			cambioGolpeAlto = true;
			}
	}
	*/
	/////////////////////////////////////////////
	if (resolverRefCuerpo == unCuerpo)
	if (resolverSeteado) { resolverSeteado = false; return estadoResolver; }
	/////////////////////////////////////////////////////////////////
	//esto lo usamos si necesitamos inmovilizarlo en y un tiempo determinado
	if (unCuerpo->HayDemoraAire())
	{
		unCuerpo->DisminuirDemoraAire();
		unCuerpo->SetVelocidadY(0);
		if (!unCuerpo->HayDemoraAire()){ unCuerpo->setDemora(0); }

	}
    ///////////////////////////////////////////////////////////////////////

	Sprite* elSprite = unCuerpo->getSprite();

	//Se setea de que cuerpo se esta tratando.
	Cuerpo* elOtroCuerpo;

	if (unCuerpo == Cuerpos.at(0)){
		elOtroCuerpo = Cuerpos.at(1);
	}
	else{
		elOtroCuerpo = Cuerpos.at(0);
	}


	Sensor* proyectilUno = unCuerpo->getSensoresProyectil().at(0);
	Sensor* proyectilDos = elOtroCuerpo->getSensoresProyectil().at(0);


	/////////////////////////////////////////////////////////////////////////////
	bool invertido;

	float posMedia1 = unCuerpo->getPosicion().x + unCuerpo->getRefPersonaje()->getAncho() / 2;
	float posMedia2 = elOtroCuerpo->getPosicion().x + elOtroCuerpo->getRefPersonaje()->getAncho() / 2;

	if (posMedia2 > posMedia1){
		invertido = false;
		unCuerpo->getRefPersonaje()->setOrientacion("DER");
	}
	else{
		invertido = true;
		unCuerpo->getRefPersonaje()->setOrientacion("IZQ");
	}
	//////////////////////////////////////////////////////////////////////////////

	std::vector<Movimiento*> movimientos = unCuerpo->getControlador()->getMovimientos();
	std::vector<Movimiento*> movimientosOtro = elOtroCuerpo->getControlador()->getMovimientos();
		
	
	//********************************************************************************************************************************************************************
	// LOGICA DE SEPARACION, GOLPES, ACCIONES
	//*******************************************************************************************************************************************************************
	if (unCuerpo->EstaSuperpuesto() && ((!(unCuerpo->estaEnPiso() && elOtroCuerpo->estaEnPiso())) || (estadoanterior.golpeado == GOLPEADO) || estadoanterior.accion==ARMA )){

		nuevoEstado = Mundo::moverCuerpos(unCuerpo, elOtroCuerpo, invertido, &movimientosOtro, nuevoEstado);
		if (invertido){
			if (unCuerpo->getEstado().movimiento != CAMINARIZQ){
				nuevoEstado = unCuerpo->getEstadoAnterior();
			}
		}
		else{
			if (unCuerpo->getEstado().movimiento != CAMINARDER){
				nuevoEstado = unCuerpo->getEstadoAnterior();
			}

		}

	}
	else{// NO HAY SUPERPOSICION, LO SIGUIENTE A RESOLVER ES LA COLISION
		
		//PRE: si el tipo ya esta golpeado o el otro no vuelvo a evaluar
		// ademas, alguno tiene que estar haciendo algo.. habria que desconsiderar las guardias......
		// es sucio pero es una optimizacion, sino aca va a entrar siempre para el otro cuerpo no golpeado
		//POST: esto deja personaje estadoactual.golpeado=golpeado si hubo colision  y le aplica demora o si no hubo no setea nada
		if (estadoanterior.golpeado != GOLPEADO && elOtroCuerpo->getEstado().golpeado != GOLPEADO && ((estadoanterior.accion != SIN_ACCION) || (elOtroCuerpo->getEstado().accion != SIN_ACCION))){
			
			nuevoEstado = Mundo::ResolverAtaques(unCuerpo, elOtroCuerpo, nuevoEstado, proyectilUno, proyectilDos, invertido);
			if ((nuevoEstado.golpeado == GOLPEADO) && (estadoanterior.golpeado != GOLPEADO)){
				
				unCuerpo->setDemora((elSprite->getConstantes(nuevoEstado))*(elSprite->listaDeCuadros(nuevoEstado)->size())); //demora general para todo
				SinVida = unCuerpo->getRefPersonaje()->descontarVida(unCuerpo->getEstado(), elOtroCuerpo->getEstado()); //descuento vida
			}

		}

		

		// PASO 3 RESOLVEMOS LA LOGICA DE SALTOS

		//ahora hay que resolver la logica de altura
		//el tipo puede estar saltando, pudo haber sido golpeado, pude estar superponiendose en el aire
		//tanto golpeado como golpeando no nos importa por que analizamos en principio la parte de .movimiento
		// al llegar al piso primero suspender accion de golpe por si esta con una patada boladora
		// y chequear superposicion
		nuevoEstado = Mundo::ResolverSaltos(difTiempo, unCuerpo, elOtroCuerpo, nuevoEstado, invertido, &movimientos);


		// PASO 4  AHORA HAY QUE ANALIZAR SI HAY DEMORA, LA DEMORA PUEDE SER POR UN UN ESTADO ANTERIOR O POR QUE SE APLICO EN EL ESTADO ACTUAL UN GOLPEADO
		
		//casos://llevando a cabo una accion // estoy golpeado 	// me acaban de golpear //si hay demora
		if (unCuerpo->HayDemora())
		{
			if ((!(nuevoEstado.golpeado == GOLPEADO && unCuerpo->getEstadoAnterior().golpeado == NOGOLPEADO)) && nuevoEstado.golpeado != FALLECIDO){
				unCuerpo->DisminuirDemora();
				nuevoEstado = unCuerpo->getEstadoAnterior();
			} //en el caso de que se cumpla, el tipo sale de aca con el estado actual que es golpeado!
			//si estadoactual=golpeado y estado anterior no lo es no disminuir


			//tomas especiales
			nuevoEstado = Mundo::ResolverDemorasEspeciales(difTiempo, unCuerpo, elOtroCuerpo, nuevoEstado, invertido);
			
			
		}// cierra el if demora

		else
		{//no hay demora,// si llega aca, no esta golpeado y no esta haciendo nada (y voy a evaluar si ahora si va a hacer algo (puede estar en el aire o en piso)

			nuevoEstado = ResolverAcciones(difTiempo, unCuerpo, elOtroCuerpo, nuevoEstado, invertido, &movimientos);

		}


	} //cierra el else

	//********************************************************************************************************************************************************************
	// LOGICA DE SEPARACION POST ACCIONES
	//*******************************************************************************************************************************************************************
	
	//Si estan superpuestos y los 2 sin ninguna accion, salvo guardia y caminando
	if (haySuperposicion(unCuerpo, elOtroCuerpo, invertido) && (unCuerpo->getEstado().accion == SIN_ACCION || unCuerpo->getEstado().accion == GUARDIA || unCuerpo->getEstado().golpeado == GOLPEADO || unCuerpo->getEstado().movimiento == CAMINARDER || unCuerpo->getEstado().movimiento == CAMINARIZQ) && (elOtroCuerpo->getEstado().accion == SIN_ACCION || elOtroCuerpo->getEstado().accion == GUARDIA || elOtroCuerpo->getEstado().movimiento == CAMINARIZQ || elOtroCuerpo->getEstado().movimiento == CAMINARDER)){
		unCuerpo->Superponer();
	}
	else{
		unCuerpo->Separados();
	}



	//********************************************************************************************************************************************************************
	// LOGICA DE PROYECTILES
	//*******************************************************************************************************************************************************************

	if (!(nuevoEstado.accion == ARMA_ARROJABLE)){
		unCuerpo->getSensoresProyectil().at(0)->resetearPosicionInicial();
		unCuerpo->getSensoresProyectil().at(0)->desactivarSensor();
	}

	//TODO aca tiene que ser estado anterior********
	if ((nuevoEstado.accion == ARMA_ARROJABLE) && (unCuerpo->getSensoresProyectil().at(0)->estaActivo())){
		unCuerpo->setposProyectilAnterior(unCuerpo->getSensoresProyectil().at(0)->getPosicion().first);
		unCuerpo->getSensoresProyectil().at(0)->moverProyectil(VELOCIDADPROYECTIL);
	}

	
	//********************************************************************************************************************************************************************
	// LOGICA DE MANEJO DE PELEA  (QUIEN GANO ROUND, FIN DE PELEA, ETC)
	//*******************************************************************************************************************************************************************
    
	nuevoEstado = ResolverBatalla(unCuerpo, elOtroCuerpo, nuevoEstado, invertido, SinVida);
		
	//********************************************************************************************************************************************************************
	// LOGICA DE BORDES PARA ALGUNOS PODERES
	//*******************************************************************************************************************************************************************
	if (unCuerpo->EstaFrenado() ){
		
		if (!((nuevoEstado.accion == BICICLETA && estadoanterior.accion != BICICLETA) || (nuevoEstado.accion == FLYKICK && estadoanterior.accion != FLYKICK))){

			unCuerpo->SetVelocidadX(0.0f);
		}
		else{ // acabas de lanzar la bicicleta en una punta
			unCuerpo->Liberar();
			if (!invertido){
				vector2D unVector = unCuerpo->getPosicion();
				unVector.x += 60;
				unCuerpo->SetPosicion(unVector);
			}
			else {
				vector2D unVector = unCuerpo->getPosicion();
				unVector.x -= 60;
				unCuerpo->SetPosicion(unVector);
			}

		}
		
	}
	
	
		
	//********************************************************************************************************************************************************************
	// LOGICA DE CALCULO DE VELOCIDADES Y POSICIONES // SENSORES TAMBIEN
	//*******************************************************************************************************************************************************************	
	

	unCuerpo->SetSensorActivoStr(nuevoEstado);
	unCuerpo->setEstadoAnterior(nuevoEstado);

	// velocidades
	
	vector2D unaVelocidad = unCuerpo->getVelocidad();  //obtengo la velocidad actual
	unCuerpo->sumarPosicion(unaVelocidad * difTiempo); //cambio la posicion
	unCuerpo->sumarVelocidad(gravedad * difTiempo); // aplico gravedad
	
	return nuevoEstado;
}


float Mundo::getYPiso() const
{
	return yPiso;
}

Mundo:: ~Mundo(){
	delete unReloj;
	ultimaToma = nullptr;
	Cuerpos.clear();
}
