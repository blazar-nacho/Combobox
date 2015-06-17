#pragma once
class Input
{
public:
	Input();
	void setInput(std::string unInput);
	void setColor(SDL_Color unColor);
	std::string getInput();
	SDL_Color getColor();
	~Input();

private:
	std::string input;
	SDL_Color color;
};

