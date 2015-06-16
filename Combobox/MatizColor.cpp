#include "stdafx.h"

MatizColor::MatizColor(SDL_Surface* superficieNueva)
{
	superficie = superficieNueva;
}


SDL_Surface* MatizColor::desplazarMatiz(double hInicial, double hFinal, double desplazamiento)
{
	// cuadro de toda la superficie
	unCuadro = new SDL_Rect;
	unCuadro->x = 0;
	unCuadro->y = 0;
	unCuadro->w = superficie->w;
	unCuadro->h = superficie->h;
	return desplazarMatiz(hInicial, hFinal, desplazamiento, unCuadro);
}

SDL_Surface* MatizColor::desplazarMatiz(double hInicial, double hFinal, double desplazamiento, SDL_Rect* unCuadro)
{	
	int xIni = unCuadro->x;
	int yIni = unCuadro->y;
	int ancho = xIni + unCuadro->w;
	int alto = yIni + unCuadro->h;	
	Uint8 rojo, verde, azul, alfa;
	double croma = 0;
	int minRGBRef;
	Uint8* punteroPixel;


	if (SDL_MUSTLOCK(superficie))
		SDL_LockSurface(superficie);

	for (int x = xIni; x < ancho; x++)
		for (int y = yIni; y < alto; y++)
		{

			Uint32 pixelXY = obtenerPixel(x, y, &punteroPixel);

			// tomo los valores rojo, verde, azul, alfa
			SDL_GetRGBA(pixelXY, superficie->format, &rojo, &verde, &azul, &alfa);

			if (alfa > ALFA_MIN) {
				// calculo el matiz
				double hOriginal = obtenerMatiz(pixelXY, rojo, verde, azul, &croma, &minRGBRef);

				// cambiar el matiz, dependiendo de las condiciones iniciales y finales
				setMatizSelectivo(&pixelXY, desplazamiento, hOriginal, hInicial, hFinal, rojo, verde, azul, croma, &minRGBRef);
				*((Uint32*)punteroPixel) = pixelXY;
			}


		}

	if (SDL_MUSTLOCK(superficie))
		SDL_UnlockSurface(superficie);

	return superficie;

}

double MatizColor::obtenerMatiz(Uint32 pixel, Uint8 rojo, Uint8 verde, Uint8 azul, double* croma, int* minRGBRef)
{
	int maxRGB = maximo(rojo, verde, azul);
	int minRGB = minimo(rojo, verde, azul);
	*croma = (double)(maxRGB - minRGB);
	*minRGBRef = minRGB;
	double HObtenido = H_INDEF;

	if (*croma == 0) 
		return HObtenido;

	if (maxRGB == rojo)
		HObtenido = 60.0f * (restoReal(((verde - azul) / *croma), 6));

	else if (maxRGB == verde)
		HObtenido = 60.0f * (((azul - rojo) / *croma) + 2);

	else if (maxRGB == azul)
		HObtenido = 60.0f * (((rojo - verde) / *croma) + 4);
		

	if (HObtenido > 360)		
		HObtenido = restoReal(HObtenido, 360);
	else if (HObtenido < 0)
		//lo ubica dentro del rango[0, -360), negativo , y le suma 360 para que quede
		// en el rango [0, 360), positivo
		HObtenido = 360 + restoReal(HObtenido, 360);
	
	return HObtenido;
}

Uint32 MatizColor::obtenerPixel(int x, int y, Uint8** punteroPixel)
{
	int bpp = superficie->format->BytesPerPixel;	
	*punteroPixel = (Uint8 *)superficie->pixels + y * superficie->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return **punteroPixel;
		break;

	case 2:
		return *(Uint16 *)*punteroPixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return *punteroPixel[0] << 16 | *punteroPixel[1] << 8 | *punteroPixel[2];
		else
			return *punteroPixel[0] | *punteroPixel[1] << 8 | *punteroPixel[2] << 16;
		break;

	case 4:
		return *(Uint32 *)*punteroPixel;
		break;

	default:
		return 0;      
	}
}

void MatizColor::setMatizSelectivo(Uint32* pixel, double desplazamiento, double hOriginal, double hInicial, double hFinal, Uint8 rojo, Uint8 verde, Uint8 azul, double croma, int *minRGBRef)
{
	// salteo los casos fuera del rango dado
	if (hFinal < hInicial){
		if ((hOriginal < hInicial) && (hOriginal > hFinal))
			return;
	}
	else
		if ((hOriginal < hInicial) || (hOriginal > hFinal))
			return;

	setMatiz(pixel, desplazamiento, hOriginal, rojo, verde, azul, croma, minRGBRef);
}



void MatizColor::setMatiz(Uint32* pixel, double desplazamiento, double hOriginal, Uint8 rojo, Uint8 verde, Uint8 azul, double croma, int *minRGBRef)
{
	double hDec = (restoReal(hOriginal + desplazamiento, 360)) / 60.0f;
	

	int intermedio = (int)round(croma * (1 - abs(restoReal(hDec, 2) - 1)));
	int cromaInt = (int)round(croma);

	Uint8 color1 = Uint8(cromaInt + *minRGBRef);	
	Uint8 color2 = Uint8(intermedio + *minRGBRef);
	Uint8 color3 = Uint8(*minRGBRef);

	
	if ((hOriginal == H_INDEF) || (hDec < 0)) return;
	
	if (hDec < 1)
	{
		*pixel = SDL_MapRGB(superficie->format, color1, color2, color3);
		return;
	}
	if (hDec < 2)
	{
		*pixel = SDL_MapRGB(superficie->format, color2, color1, color3);
		return;
	}
	if (hDec < 3)
	{
		*pixel = SDL_MapRGB(superficie->format, color3, color1, color2);
		return;
	}
	if (hDec < 4)
	{
		*pixel = SDL_MapRGB(superficie->format, color3, color2, color1);
		return;
	}
	if (hDec < 5)
	{
		*pixel = SDL_MapRGB(superficie->format, color2, color3, color1);
		return;
	}
	if (hDec < 6)
	{
		*pixel = SDL_MapRGB(superficie->format, color1, color3, color2);
		return;
	}

}

SDL_Surface* MatizColor::mostrarCongelado(SDL_Rect* cuadro)
{
	cuadroModificado = cuadro;
	int xFin = cuadroModificado->x + cuadroModificado->w;
	int yFin = cuadroModificado->y + cuadroModificado->h;
	Uint8 rojo, verde, azul, alfa;
	double croma = 0;
	int minRGBRef;
	Uint8* punteroPixel;
	modificado = true;

	if (SDL_MUSTLOCK(superficie))
		SDL_LockSurface(superficie);

	for (int x = cuadroModificado->x; x < xFin; x++)
		for (int y = cuadroModificado->y; y < yFin; y++)
		{

			Uint32 pixelXY = obtenerPixel(x, y, &punteroPixel);
			// encola el valor de pixel original
			pixelsOrig.push(pixelXY);

			// tomo los valores rojo, verde, azul, alfa
			SDL_GetRGBA(pixelXY, superficie->format, &rojo, &verde, &azul, &alfa);

			if (alfa > ALFA_MIN) {
				int maxRGB = maximo(rojo, verde, azul);
				int minRGB = minimo(rojo, verde, azul);
				croma = (double)(maxRGB - minRGB);
				minRGBRef = minRGB;
				// cambiar el matiz
				setMatizCongelado(&pixelXY, rojo, verde, azul, croma, &minRGBRef);
				*((Uint32*)punteroPixel) = pixelXY;
			}


		}

	if (SDL_MUSTLOCK(superficie))
		SDL_UnlockSurface(superficie);

	return superficie;
}

void MatizColor::setMatizCongelado(Uint32* pixel, Uint8 rojo, Uint8 verde, Uint8 azul, double croma, int *minRGBRef)
{
	double hDec = (restoReal(MATIZ_CONG, 360)) / 60.0f;


	int intermedio = (int)round(croma * (1 - abs(restoReal(hDec, 2) - 1)));
	int cromaInt = (int)round(croma);

	Uint8 color1 = Uint8(cromaInt + *minRGBRef);
	Uint8 color2 = Uint8(intermedio + *minRGBRef);
	Uint8 color3 = Uint8(*minRGBRef);

	if ((color1 < OSCUR_MIN) && (color2 < OSCUR_MIN) && (color3 < OSCUR_MIN))
	{
		*pixel = SDL_MapRGB(superficie->format, rojo, verde + DESP_VERDE_CONG, azul + DESP_AZUL_CONG);
		return;
	}

	if (hDec < 0) return;

	if (hDec < 1)
	{
		*pixel = SDL_MapRGB(superficie->format, color1, color2, color3);
		return;
	}
	if (hDec < 2)
	{
		*pixel = SDL_MapRGB(superficie->format, color2, color1, color3);
		return;
	}
	if (hDec < 3)
	{
		*pixel = SDL_MapRGB(superficie->format, color3, color1, color2);
		return;
	}
	if (hDec < 4)
	{
		*pixel = SDL_MapRGB(superficie->format, color3, color2, color1);
		return;
	}
	if (hDec < 5)
	{
		*pixel = SDL_MapRGB(superficie->format, color2, color3, color1);
		return;
	}
	if (hDec < 6)
	{
		*pixel = SDL_MapRGB(superficie->format, color1, color3, color2);
		return;
	}

}

SDL_Surface* MatizColor::mostrarOriginal()
{
	if (!modificado)
		return superficie;

	// restauro el cuadro de la superficie modificado
	modificado = false;
	int xFin = cuadroModificado->x + cuadroModificado->w;
	int yFin = cuadroModificado->y + cuadroModificado->h;	
	Uint8* punteroPixel;


	if (SDL_MUSTLOCK(superficie))
		SDL_LockSurface(superficie);

	for (int x = cuadroModificado->x; x < xFin; x++)
		for (int y = cuadroModificado->y; y < yFin; y++)
		{

			Uint32 pixelXY = obtenerPixel(x, y, &punteroPixel);

			*((Uint32*)punteroPixel) = pixelsOrig.front();
			pixelsOrig.pop();
		}

	if (SDL_MUSTLOCK(superficie))
		SDL_UnlockSurface(superficie);

	return superficie;
}

MatizColor::~MatizColor()
{
	delete unCuadro;
	//if (superficie != nullptr) SDL_FreeSurface(superficie);
	if (cuadroModificado != nullptr) delete cuadroModificado;
}