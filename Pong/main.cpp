#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
using namespace std;

const int SCREENW = 640;
const int SCREENH = 480;

void RenderTexture(SDL_Texture *img, SDL_Renderer *ren, int x, int y);

int main(int argc, char **argv)
{
	SDL_Window *win = NULL;
	SDL_Renderer *ren = NULL;
	SDL_Texture *background = NULL;
	SDL_Texture *ball = NULL;
	SDL_Texture *lPaddle = NULL;
	SDL_Texture *rPaddle = NULL;
	SDL_Event event;

	//Initialize SDL video subsystems
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Quit();
		return -1;
	}

	
	//Initialize IMG 
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Quit();
		return -1;
	}
	

	//start display
	win = SDL_CreateWindow("Pong", 100, 100, SCREENW, SCREENH, SDL_WINDOW_SHOWN);
	if (win == NULL)
	{
		SDL_Quit();
		return -1;
	}
	
	//create renderer
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL)
	{
		SDL_DestroyWindow(win);
		SDL_Quit();
	}
	
	//setting up window
	background = IMG_LoadTexture(ren, "background.png");
	ball = IMG_LoadTexture(ren, "ball.png");
	lPaddle = IMG_LoadTexture(ren, "paddle.png");
	rPaddle = IMG_LoadTexture(ren, "paddle.png");

	//drawing everything
	SDL_RenderClear(ren);

	RenderTexture(background, ren, 0, 0);
	
	int ballW;
	SDL_QueryTexture(ball, NULL, NULL, &ballW, NULL);
	RenderTexture(ball, ren, SCREENW / 2 - 25, SCREENH / 2 - ballW / 2);
	
	int paddleW, paddleH;
	SDL_QueryTexture(lPaddle, NULL, NULL, &paddleW, &paddleH);
	int rPaddleY = SCREENH / 2 - paddleH / 2;
	RenderTexture(lPaddle, ren, 0, rPaddleY);
	int lPaddleY = SCREENH / 2 - paddleH / 2;
	RenderTexture(rPaddle, ren, SCREENW - paddleW, lPaddleY);
	
	SDL_RenderPresent(ren);
	
	int move = 20;
	bool end = false;
	while (!end)
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				SDL_Quit();
				end = true;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					end = true;
					break;
				case SDLK_UP:
					rPaddleY -= move;
					break;
				case SDLK_DOWN:
					rPaddleY += move;
					break;
				case SDLK_w:
					lPaddleY -= move;
					break;
				case SDLK_s:
					lPaddleY += move;
					break;
				}
				break;
			}
		}
		//clear screen
		SDL_RenderClear(ren);

		//render objects
		RenderTexture(background, ren, 0, 0);
		RenderTexture(lPaddle, ren, 0, lPaddleY);
		RenderTexture(rPaddle, ren, SCREENW - paddleW, rPaddleY);
		RenderTexture(ball, ren, SCREENW / 2 - 25, SCREENH / 2 - ballW / 2);

		//update screen
		SDL_RenderPresent(ren);
	}
	
	//Clean up
	SDL_DestroyTexture(ball);
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(lPaddle);
	SDL_DestroyTexture(rPaddle);

	IMG_Quit();
	SDL_Quit();

	return 0;
}

void RenderTexture(SDL_Texture *img, SDL_Renderer *ren, int x, int y)
{
	SDL_Rect temp;
	temp.x = x;
	temp.y = y;

	SDL_QueryTexture(img, NULL, NULL, &temp.w, &temp.h);
	SDL_RenderCopy(ren, img, NULL, &temp);
}
