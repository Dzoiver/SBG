//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int BUTTON_WIDTH = 250;
const int BUTTON_HEIGHT = 250;
const int TOTAL_BUTTONS = 5;
const int BUTTON_HIDE_SHOP_WIDTH = 100;
const int BUTTON_HIDE_SHOP_HEIGHT = 30;

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_TOTAL = 3
};

#include "ltexture.h"

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

bool is_hide_shop = true;

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font *gFont = NULL;

SDL_Rect gSpriteClips[4];

const int SPAWN_ANIMATION_FRAMES = 10;
SDL_Rect gSpawnClips[10];
LTexture gTextTexture;
LTexture gSoldierSheetTexture;
LTexture gSpawnSheetTexture;
LTexture gModulatedTexture;
LTexture gArchvileTexture;

LTexture gFooTexture;
LTexture gBackgroundTexture;

#include "LButton.h"

//Buttons

SDL_Rect Sheet_Shop_buttons[BUTTON_SPRITE_TOTAL];

LButton gShopButton;

SDL_Rect gCard_buttons[BUTTON_SPRITE_TOTAL];

LTexture gCardSheetTexture;

LButton gButtons[TOTAL_BUTTONS];



LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;
	
	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if (x < mPosition.x)
		{
			inside = false;
		}
		//Mouse is right of the button
		else if (x > mPosition.x + BUTTON_WIDTH)
		{
			inside = false;
		}
		//Mouse about the button
		else if (y < mPosition.y)
		{
			inside = false;
		}
		//Mouse below the button
		else if (y > mPosition.y + BUTTON_HEIGHT)
		{
			inside = false;
		}
		//Mouse is outside button
		if (!inside)
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		//Mouse is inside button
		else {
			//Set mouse over sprite
			switch (e->type)
			{ 
			case SDL_MOUSEMOTION:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					break;

				case SDL_MOUSEBUTTONDOWN:
						mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
						break;

					case SDL_MOUSEBUTTONUP:
						mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
						break;

			}
		}
	}
}

void LButton::render()
{
	//Show current button sprite
	gCardSheetTexture.render(mPosition.x, mPosition.y, &gCard_buttons[mCurrentSprite]);
}

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}
#ifdef _SDL_TTF_H
bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
#endif

bool LTexture::loadFromRenderText(std::string textureText, SDL_Color textColor)
{
	free();

	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else {
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface(textSurface);
	}

	return mTexture != NULL;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}
void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf coould not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	gFont = TTF_OpenFont("res/arial.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load arial font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 0, 0, 0 };
		if (!gTextTexture.loadFromRenderText("QWE ASD for changing colors. JK to rotate. ZX to change alpha. Arrows to change direction", textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}

	gArchvileTexture.loadFromFile("res/imp.png");
	if (!gSoldierSheetTexture.loadFromFile("res/player_sheet4.png"))
	{
		printf("Failed to load Soldier texture!\n");
		success = false;
	}
	else
	{
		//Set the sprite
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 99;
		gSpriteClips[0].h = 98;

		gSpriteClips[1].x = 98;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = 99;
		gSpriteClips[1].h = 98;

		gSpriteClips[2].x = 198;
		gSpriteClips[2].y = 0;
		gSpriteClips[2].w = 99;
		gSpriteClips[2].h = 98;

		gSpriteClips[3].x = 297;
		gSpriteClips[3].y = 0;
		gSpriteClips[3].w = 99;
		gSpriteClips[3].h = 98;
		////////////////////////////////////////////////
		/*
		//left
		gSpriteClips[7].x = 297;
		gSpriteClips[7].y = 98;
		gSpriteClips[7].w = 99;
		gSpriteClips[7].h = 98;

		gSpriteClips[11].x = 297;
		gSpriteClips[11].y = 196;
		gSpriteClips[11].w = 99;
		gSpriteClips[11].h = 98;
		
		//right
		gSpriteClips[5].x = 99;
		gSpriteClips[5].y = 98;
		gSpriteClips[5].w = 99;
		gSpriteClips[5].h = 98;

		gSpriteClips[9].x = 99;
		gSpriteClips[9].y = 196;
		gSpriteClips[9].w = 99;
		gSpriteClips[9].h = 98;
		
		//////////////////////////////////////////////////
		//up
		gSpriteClips[4].x = 0;
		gSpriteClips[4].y = 98;
		gSpriteClips[4].w = 99;
		gSpriteClips[4].h = 98;

		gSpriteClips[8].x = 0;
		gSpriteClips[8].y = 196;
		gSpriteClips[8].w = 99;
		gSpriteClips[8].h = 98;
		
		//down
		gSpriteClips[6].x = 196;
		gSpriteClips[6].y = 98;
		gSpriteClips[6].w = 99;
		gSpriteClips[6].h = 98;

		gSpriteClips[10].x = 198;
		gSpriteClips[10].y = 196;
		gSpriteClips[10].w = 99;
		gSpriteClips[10].h = 98;*/
	}
	if (!gSpawnSheetTexture.loadFromFile("res/spawn.png"))
	{
		printf("Failed to load spawn.png\n");
		success = false;
	}
	else
	{
		// 42 56
		//offset 21 28
		gSpawnClips[0].x = 0;
		gSpawnClips[0].y = 0;
		gSpawnClips[0].w = 42;
		gSpawnClips[0].h = 56;

		gSpawnClips[1].x = 42;
		gSpawnClips[1].y = 0;
		gSpawnClips[1].w = 42;
		gSpawnClips[1].h = 45;

		gSpawnClips[2].x = 86;
		gSpawnClips[2].y = 0;
		gSpawnClips[2].w = 40;
		gSpawnClips[2].h = 37;

		gSpawnClips[3].x = 127;
		gSpawnClips[3].y = 0;
		gSpawnClips[3].w = 30;
		gSpawnClips[3].h = 34;
		//offset 21 28
		gSpawnClips[4].x = 160;
		gSpawnClips[4].y = 0;
		gSpawnClips[4].w = 17;
		gSpawnClips[4].h = 16;

		gSpawnClips[5].x = 181;
		gSpawnClips[5].y = 0;
		gSpawnClips[5].w = 9;
		gSpawnClips[5].h = 8;

		gSpawnClips[6].x = 195;
		gSpawnClips[6].y = 0;
		gSpawnClips[6].w = 3;
		gSpawnClips[6].h = 3;

		gSpawnClips[7].x = 195;
		gSpawnClips[7].y = 49;
		gSpawnClips[7].w = 7;
		gSpawnClips[7].h = 7;

		gSpawnClips[8].x = 181;
		gSpawnClips[8].y = 9;
		gSpawnClips[8].w = 13;
		gSpawnClips[8].h = 13;
		//offset 21 28
		gSpawnClips[9].x = 195;
		gSpawnClips[9].y = 9;
		gSpawnClips[9].w = 17;
		gSpawnClips[9].h = 17;
	}
	if (!gModulatedTexture.loadFromFile("res/colors.png"))
	{
		printf("Failed to load Colors texture!\n");
		success = false;
	}
	//Load background texture
	if (!gBackgroundTexture.loadFromFile("res/SBG_logo.png"))
	{
		printf("Failed to load background texture image\n");
		success = false;
	}
	else
	{
		gBackgroundTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}
	//Load card texture sheet
	if (!gCardSheetTexture.loadFromFile("res/card_sheet.png"))
	{
		printf("Failed to load card sheet texture!\n");
		success = false;
	}
	else
	{
		//Set sprites
		for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
		{
			gCard_buttons[i].x = 0;
			gCard_buttons[i].y = i * 250;
			gCard_buttons[i].w = BUTTON_WIDTH;
			gCard_buttons[i].h = BUTTON_HEIGHT;
		}

		int space_between_shop_cards = 100;
		int card_x_Position = 150;
		for (int i = 0; i < 5; i++)
		{
			gButtons[i].setPosition(card_x_Position, 150);
			card_x_Position+=350;
		}



		Sheet_Shop_buttons[0].x = 250;
		Sheet_Shop_buttons[0].y = 0;
		Sheet_Shop_buttons[0].w = BUTTON_HIDE_SHOP_WIDTH;
		Sheet_Shop_buttons[0].h = BUTTON_HIDE_SHOP_HEIGHT;

		Sheet_Shop_buttons[1].x = 250;
		Sheet_Shop_buttons[1].y = 31;
		Sheet_Shop_buttons[1].w = BUTTON_HIDE_SHOP_WIDTH;
		Sheet_Shop_buttons[1].h = BUTTON_HIDE_SHOP_HEIGHT;

		Sheet_Shop_buttons[2].x = 250;
		Sheet_Shop_buttons[2].y = 31;
		Sheet_Shop_buttons[2].w = BUTTON_HIDE_SHOP_WIDTH;
		Sheet_Shop_buttons[2].h = BUTTON_HIDE_SHOP_HEIGHT;

		Sheet_Shop_buttons[3].x = 250;
		Sheet_Shop_buttons[3].y = 31;
		Sheet_Shop_buttons[3].w = BUTTON_HIDE_SHOP_WIDTH;
		Sheet_Shop_buttons[3].h = BUTTON_HIDE_SHOP_HEIGHT;

		gShopButton.setPosition(60, 800);

	}


	return success;
}

void close()
{
	//Free loaded image
	gBackgroundTexture.free();
	gSoldierSheetTexture.free();
	gArchvileTexture.free();
	gTextTexture.free();
	gCardSheetTexture.free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	//IMG_QUIT();
	TTF_Quit();
		IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;


			//Event handler
			SDL_Event e;

			int frame = 0;

			Uint8 r = 255;
			Uint8 g = 255;
			Uint8 b = 255;

			double degrees = 0;

			SDL_RendererFlip flipType = SDL_FLIP_NONE;

			Uint8 a = 255;

			int i = 0;
			bool walking = false;
			//While application is running
			printf("initialization completed");
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_UP:
							i = 0;
							break;
						case SDLK_RIGHT:
							i = 1;
							break;
						case SDLK_DOWN:
							i = 2;
							break;
						case SDLK_LEFT:
							i = 3;
							break;
							//Increase red
						case SDLK_q:
							r += 32;
							break;

							//Increase green
						case SDLK_w:
							g += 32;
							break;

							//Increase blue
						case SDLK_e:
							b += 32;
							break;

							//Decrease red
						case SDLK_a:
							r -= 32;
							break;

							
						case SDLK_j:
							degrees -= 60;
							break;

						case SDLK_k:
							degrees += 60;
							break;

						case SDLK_u:
							flipType = SDL_FLIP_HORIZONTAL;
							break;

						case SDLK_i:
							flipType = SDL_FLIP_NONE;
							break;

						case SDLK_o:
							flipType = SDL_FLIP_VERTICAL;
							break;

						case SDLK_z:
							if(a+32 > 255)
							{
								a = 255;
							}
							else
							{
								a += 32;
							}
							break;

						case SDLK_x:
							if (a - 32 < 0)
							{
								a = 0;
							}
							else
							{
								a -= 32;
							}
							break;

							//Decrease green
						case SDLK_s:
							g -= 32;
							break;

							//Decrease blue
						case SDLK_d:
							b -= 32;
							break;
						}
					}
					//Handle button events
					for (int i = 0; i < TOTAL_BUTTONS; ++i)
					{
						gButtons[i].handleEvent(&e);
					}
						gShopButton.handleEvent(&e);
				}
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				gTextTexture.render(600, 700);
				SDL_Rect* currentClip = &gSpawnClips[frame / 20];
				gSpawnSheetTexture.render(800, 500, currentClip);
				++frame;
				if (frame / 20 >= SPAWN_ANIMATION_FRAMES)
				{
					frame = 0;
				}

				gModulatedTexture.setColor(r, g, b);
				//Render background texture to screen
				gModulatedTexture.render(1500,0);

				gArchvileTexture.render(1300, 500, NULL, degrees, NULL, flipType);
				gSoldierSheetTexture.render(200, 600, &gSpriteClips[i]);
				//Update screen
				gBackgroundTexture.setAlpha(a);
				gBackgroundTexture.render(0, 0);

				//render buttons
				if (is_hide_shop)
				{
					for (int i = 0; i < TOTAL_BUTTONS; ++i)
					{
						gButtons[i].render();
					}
				}

				gShopButton.render();

				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}