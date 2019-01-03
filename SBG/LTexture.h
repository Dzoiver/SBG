#pragma once

class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	bool loadFromRenderText(std::string textureText, SDL_Color textColor);

	//Deallocates texture
	void free();

	void setBlendMode(SDL_BlendMode blending);

	void setAlpha(Uint8 alpha);

	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions(
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};