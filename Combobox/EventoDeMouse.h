#pragma once
class EventoDeMouse
{
public:
	EventoDeMouse();
	void setX(int unX);
	void setY(int unY);
	void setClick(bool unClick);
	int getX();
	int getY();
	bool getClick();
	~EventoDeMouse();

private:
	int x;
	int y;
	bool click;
};

