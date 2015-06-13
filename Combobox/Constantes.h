#pragma once

// Salida del gameloop
#define CONTINUAR	0
#define REINICIAR	1
#define FIN   		2
#define CAMBIAR_ROUND 3
#define REINICIAR_PELEA 4
#define INICIAR 5

//Salida del gameloop de menu
#define MODOVS 1
#define MODOVSCOM 2
#define MODOPRACTICA 3

#define MODO_DEBUG_SDL false
#define MODO_COLOR false
#define MODO_SONIDO true
#define MODO_INMORTAL false //xjose lo uso para que nadie muera y poder testear tranquilo
#define MODO_RAPIDO false

//JSON
#define PRUEBA_JSON "prueba.json"
#define PRUEBA_JSON_DEFECTO "pruebaDefecto.json"

#define VIBRACIONES 3

//MODO DE JUEGO
enum MODOS_DE_JUEGO { VS, COM, PRACTICA, NINGUNO };
//Alfa de modos
#define ALFATRANSPARENTE 0
#define ALFASEMITRANSPARENTE 35

//Superficies Vista personajes
#define SUPERFICIEPJ1 0
#define SUPERFICIEPJ2 1

//Dificultades
#define NOVATO 0
#define MED 1
#define EXPERIMENTADO 2
#define EXP 3
#define NINGUNA 999


//PERSONAJES
#define NINGUNPJ 999
#define SCORPIONROJO 11
#define SCORPIONAZUL 12
#define SCORPION_ 13
#define SCORPIONVERDE 14
#define SCORPIONVIOLETA 15
#define SCORPIONGRIS 16
#define SCORPIONNARANJA 17
#define LIUKANGAMARILLO 18
#define LIUKANGVERDE 19
#define LIUKANG_ 20
#define LIUKANGAZUL 21
#define LIUKANGROJO 22

//Contadores
#define CONTADORLOGOPELEA 170

enum Personaje1
{
	LIUKANGUNO,SCORPIONUNO
};

enum Personaje2
{
	LIUKANGDOS, SCORPIONDOS
};

enum ColorPj1
{
	BASE,ROJO,VERDE,AZUL,VIOLETA,GRIS,NARANJA,AMARILLO
};

enum ColorPj2
{
	BASEDOS, ROJODOS, VERDEDOS, AZULDOS, VIOLETADOS, GRISDOS, NARANJADOS, AMARILLODOS
};

enum Seleccionados
{
	SI,NO
};

struct PJSELECCION {
	Personaje1 personajeUno;
	Personaje2 personajeDos;
	ColorPj1 colorPjUno;
	ColorPj2 colorPjDos;
	Seleccionados seleccionados;
};

//Constantes para la vista
//VENTANA
#define ANCHO_MIN 50
#define ALTO_MIN 50
#define ANCHO_PX_VENTANA 800
#define ALTO_PX_VENTANA 600
#define ANCHO_VENTANA 200.0f
#define TITULO_VENTANA "ComboBox"
#define ICONO "ima/Bbx_icono.png"

//FISICA
//
#define FPS 60.0f
const float MSxCUADRO = 1000.0f / FPS;
//FISICA - MOVIMIENTOS
#define tiempoCaminarParaAdelante 1000.0f
#define tiempoCaminarParaAtras 1000.0f
#define tiempoQuieto 1700.0f
#define tiempoSalto 500.0f
#define tiempoSaltoDiagonal 800.0f
#define tiempoSaltoDiagonalIzq 500.0f
#define tiempoAgacharse 200.0f
//FISICA - GOLPES PARADO
#define tiempoPatadaAlta 500.0f
#define tiempoPatadaBaja 500.0f
#define tiempoGolpeAlto 400.0f
#define tiempoGolpeBajo 500.0f
#define tiempoDefensa 500.0f
//FISICA - GOLPES AEREOS
#define tiempoSaltoPatada 500.0f
#define tiempoSaltoGolpe 800.0f  // es el mismo para saltodiagonal
#define tiempoSaltoDiagonalPatada 800.0f
//FISICA - GOLPES AGACHADO
#define tiempoAgachadoPatadaAlta 500.0f
#define tiempoAgachadoPatadaBaja 500.0f
#define tiempoGancho 500.0f
#define tiempoAgachadoGolpeBajo 500.0f
#define tiempoAgachadoDefensa 500.0f
//FISICA - GOLPEADO
#define tiempoGolpeado 500.0f
#define tiempoSaltoGolpeado 1400.0f 
#define tiempoAgachadoGolpeado 500.0f
//FISICA - ARMAS
#define tiempoArmaArrojable 700.0f

// FISICA - FATALITY
#define tiempoFatality 5000.0f


//ESCENARIO
#define ANCHO_ESCENARIO 1000.5f
#define MAX_ANCHO_ESCENARIO 4000.0f
#define ALTO_ESCENARIO 150.0f
#define MAX_ALTO_ESCENARIO 2000.0f
#define Y_PISO_ESCENARIO 15.0f

//CAPA
#define ANCHO_CAPA 500.0f
#define ZINDEX_CAPA 0

//PERSONAJE
#define ANCHO_PERSONAJE 80.0f
#define MAX_ANCHO_PERSONAJE 1000.0f
#define ALTO_PERSONAJE 90.0f
#define MAX_ALTO_PERSONAJE 1000.0f
#define MAX_VIDA 100
#define VIDA_INFINITA 999

#define ZINDEX 1
#define ORIENTACION_PERSONAJE "DER"
#define SONIDOS_DEFAULT "son/scorpion/scorpion.json"
#define FATALITY_DEFAULT "ima/fatality/scorpion.json"
#define FATALITY_IMG_DEFAULT "ima/fatality/scorpion.png"
#define FATALITY_DIST_DEFAULT 50.0f
#define SONIDO_FX_DEFAULT "./son/scorpion/golpeado.wav"
#define SPRITE_DEFAULT "ima/sprites/player_default.png"
#define FONDO_DEFAULT "ima/bkg/default_capa.png" // al ser varias capas la idea esa usar esta por que va a permitir que se vean el resto de las que si estan
#define CAMINARPARAADELANTE_DEFAULT "CaminarParaAdelante"
#define CAMINARPARAATRAS_DEFAULT "CaminarParaAtras"
#define QUIETO_DEFAULT "Quieto"
#define SALTO_DEFAULT "Salto"
#define SALTODIAGONAL_DEFAULT "SaltoDiagonal"
#define CAIDA_DEFAULT "Caida"

#define PELEA_INICIAL "scorpion VS scorpion"

#define P1_POS_INI -90
#define P2_POS_INI 10

#define H_INICIAL 40.0f
#define H_FINAL 70.0f
#define DESPLAZAMIENTO 162.0f

#define GOLPEADO_DEFAULT "Golpeado"
#define AGACHARSE_DEFAULT "Agacharse"
#define PATADAALTA_DEFAULT "PatadaAlta"
#define DISPARO_DEFAULT "Disparo"

#define LIU_KANG "LiuKang"

//CONTROLADOR
enum MOV_TIPO { ARRIBA, ABAJO, DER, IZQ, DEFENSA, G_BAJO, G_ALTO, P_BAJA, P_ALTA, ARMA, CERRAR, RECARGAR, SALTOIZQ, SALTODER, DEFENSA_AGACHADO, G_ABAJO, G_GANCHO, P_BAJA_ABAJO, P_ALTA_ABAJO, QUIETO, FATALITY, COMBO2 };
#define MOSTRAR_MOVIMIENTOS false
#define CANTIDAD_MAXIMA_EVENTOS 30
#define ARRIBA_DEFAULT_1 "up"
#define ABAJO_DEFAULT_1 "down"
#define IZQUIERDA_DEFAULT_1 "left"
#define DERECHA_DEFAULT_1 "right"
#define DEFENSA_DEFAULT_1 "z"
#define ARMA_DEFAULT_1 "x"
#define G_BAJO_DEFAULT_1 "c"
#define G_ALTO_DEFAULT_1 "v"
#define P_BAJA_DEFAULT_1 "b"
#define P_ALTA_DEFAULT_1 "n"
#define ARRIBA_DEFAULT_2 "w"
#define ABAJO_DEFAULT_2 "s"
#define IZQUIERDA_DEFAULT_2 "a"
#define DERECHA_DEFAULT_2 "d"
#define DEFENSA_DEFAULT_2 "j"
#define ARMA_DEFAULT_2 "u"
#define G_BAJO_DEFAULT_2 "k"
#define G_ALTO_DEFAULT_2 "l"
#define P_BAJA_DEFAULT_2 "i"
#define P_ALTA_DEFAULT_2 "o"
#define REINICIAR_DEFAULT "r"
#define REINICIAR_PELEA_DEFAULT "f1"
#define SALIR_DEFAULT "esc"

//COMBOS
#define NOMBRE_COMBO_1 "Poder"  //arma todos
#define NOMBRE_COMBO_2 "iceshoot" //scorpion
#define NOMBRE_COMBO_3 "Teleport" //scorpion
#define NOMBRE_COMBO_4 "firewall"  //liu kang
#define NOMBRE_COMBO_5 "bicicleta" //liu kang
#define NOMBRE_COMBO_6 "flykick" //liu kang
#define NOMBRE_COMBO_7 "subbarrida" //scorpion
#define NOMBRE_COMBO_8 "powerball" //scorpion
#define NOMBRE_COMBO_9 "arpon" //scorpion
#define NOMBRE_COMBO_10 "fatality"
#define NOMBRE_COMBO_11 "fatality2"
#define NOMBRE_COMBO_12 "fatality3"

//PELEA
#define TIEMPO_INFINITO 999
#define ROUNDS_MAXIMOS 3
#define TIEMPO_DE_PELEA 99  //xjose pongo ***** para poder testear tranquilo

//FIN
