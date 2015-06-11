#include "stdafx.h"
#include "EventoDeMouse.h"


EventoDeMouse::EventoDeMouse()
{
	x = 0;
	y = 0;
	click = false;
}


void EventoDeMouse::setX(int unX){
	x = unX;
}


void EventoDeMouse::setY(int unY){
	y = unY;
}


void EventoDeMouse::setClick(bool unClick){
	click = unClick;
}


int EventoDeMouse::getX(){
	return x;
}


int EventoDeMouse::getY(){
	return y;
}


bool EventoDeMouse::getClick(){
	return click;
}


EventoDeMouse::~EventoDeMouse()
{
}
