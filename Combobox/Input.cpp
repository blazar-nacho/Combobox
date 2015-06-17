#include "stdafx.h"
#include "Input.h"


Input::Input()
{
}


void Input::setColor(SDL_Color unColor){
	color = unColor;
}


void Input::setInput(std::string unInput){
	input = unInput;
}


std::string Input::getInput(){
	return input;
}


SDL_Color Input::getColor(){
	return color;
}


Input::~Input()
{
	input.clear();
}
