#pragma once
#include <SDL.h>

class LButton
{
public:
	//Initializes internal variables
	LButton();

	//Sets top left position
	void setPosition(int x, int y);

	void handleEvent(SDL_Event* e);

	//Shows button sprite
	void render();

private:
	//Top left position
	SDL_Point mPosition;

	//Currently used global sprite
	LButtonSprite mCurrentSprite;
};