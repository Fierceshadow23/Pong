#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
using namespace std;

const int SCREENW = 640;
const int SCREENH = 480;

void RenderTexture(SDL_Texture *img, SDL_Renderer *ren, int x, int y);

class Ball
{
public:
	Ball(SDL_Renderer *ren, SDL_Texture *tex);
	~Ball();
	void RefreshCent();
	void Render();
	void StartPos();
	void Move(int lPaddleY, int rPaddleY, int paddleH, int paddleW);
	int checkScore();

private:
	SDL_Renderer *pRen;
	SDL_Texture *pTex;
	int ballW;
	int rad;
	int posX, posY;
	int centX, centY;
	int momentX = -5;
	int momentY = 0;
	int speed = 5;
};

Ball::Ball(SDL_Renderer *ren, SDL_Texture *tex) :
	pRen(ren),
	pTex(tex)
{
	SDL_QueryTexture(tex, NULL, NULL, &ballW, NULL);
	rad = ballW / 2;
	posX = SCREENW / 2 - rad;
	posY = SCREENH / 2 - rad;
	RefreshCent();
	RenderTexture(tex, pRen, posX, posY);
}


Ball::~Ball()
{
	SDL_DestroyTexture(pTex);
	SDL_DestroyRenderer(pRen);
}


//refresh center points
void Ball::RefreshCent()
{
	centX = posX + rad;
	centY = posY + rad;
}

void Ball::Render()
{
	RenderTexture(pTex, pRen, posX, posY);
}

void Ball::StartPos()
{
	posX = SCREENW / 2 - rad;
	posY = SCREENH / 2 - rad;
	momentX = -5;
	momentY = 0;
	speed = 5;
	RefreshCent();
}

void Ball::Move(int lPaddleY, int rPaddleY, int paddleH, int paddleW)
//takes Y positions of left and right paddle and paddle Height and Width
{
	int tempMomentX, tempMomentY;
	bool tempUsedX = false;
	bool tempUsedY = false;

	//top or bot
	if (centY + momentY < 0 + rad)
	{
		tempMomentY = (0 + rad) - centY;
		tempUsedY = true;
	}
	else if (centY + momentY > SCREENH - rad)
	{
		tempMomentY = (SCREENH - rad) - centY;
		tempUsedY = true;
	}
	
	//alligned Y's?
	bool against_lPaddle = centY >= lPaddleY && centY <= lPaddleY + paddleH;
	bool against_rPaddle = centY >= rPaddleY && centY <= rPaddleY + paddleH;
	//check if going to miss paddle
	if (centX + momentX < 0 + paddleW + rad && against_lPaddle)
	{
		tempMomentX = (0 + paddleW + rad) - centX;
		tempUsedX = true;
	}
	else if (centX + momentX > SCREENW - paddleW - rad && against_rPaddle)
	{
		tempMomentX = (SCREENW - paddleW - rad) - centX;
		tempUsedX = true;
	}

	//check if hit paddle
	if (centX == 0 + paddleW + rad && against_lPaddle)
	{
		int lThird = lPaddleY + paddleH / 3;
		//where on paddle did it hit?
		if (momentY == 0)
		{
			if (centY <= lThird)
			{
				momentY -= speed;
			}
			else if (centY >= lThird + paddleH / 3)
			{
				momentY += speed;
			}
			else
			{
				momentY = 0;
			}
		}
		momentX += speed;
	}
	else if (centX == SCREENW - paddleW - rad && against_rPaddle)
	{
		int rThird = rPaddleY + paddleH / 3;
		//where on paddle did it hit?
		if (momentY == 0)
		{
			if (centY <= rThird)
			{
				momentY -= speed;
			}
			else if (centY >= rThird + paddleH / 3)
			{
				momentY += speed;
			}
			else
			{
				momentY = 0;
			}
		}
		momentX -= speed;
	}

	//check if hit top or bot
	if (centY == 0 + rad)
	{
		momentY += speed;
	}
	else if (centY == SCREENH - rad)
	{
		momentY -= speed;
	}

	//move
	if (!tempUsedX && !tempUsedY) {
		posX += momentX;
		posY += momentY;
	}
	else if (tempUsedX && !tempUsedY)
	{
		posX += tempMomentX;
		posY += momentY;
	}
	else if (!tempUsedX && tempUsedY)
	{
		posX += momentX;
		posY += tempMomentY;
	}
	else
	{
		posX += tempMomentX;
		posY += tempMomentY;
	}

	RefreshCent();
	Render();
}

int Ball::checkScore()
{
	if (centX <= 0 + rad)
	{
		return 1;
	}
	else if (centX >= SCREENW - rad)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char **argv)
{
	//Initalize objects
	SDL_Window *win = NULL;
	SDL_Renderer *ren = NULL;
	SDL_Texture *tex = NULL;
	SDL_Texture *background = NULL;
	SDL_Texture *lPaddle = NULL;
	SDL_Texture *rPaddle = NULL;
	SDL_Texture *lScore = NULL;
	SDL_Texture *rScore = NULL;
	TTF_Font *font = NULL;
	SDL_Surface *temp = NULL;
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

	//Initialize TTF
	if (TTF_Init() < 0)
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
	lPaddle = IMG_LoadTexture(ren, "paddle.png");
	rPaddle = IMG_LoadTexture(ren, "paddle.png");

	//drawing everything
	SDL_RenderClear(ren);

	RenderTexture(background, ren, 0, 0);
	
	tex = IMG_LoadTexture(ren, "ball.png");
	Ball ball(ren, tex);
	
	int paddleW, paddleH;
	SDL_QueryTexture(lPaddle, NULL, NULL, &paddleW, &paddleH);
	int rPaddleY = SCREENH / 2 - paddleH / 2;
	RenderTexture(lPaddle, ren, 0, rPaddleY);
	int lPaddleY = SCREENH / 2 - paddleH / 2;
	RenderTexture(rPaddle, ren, SCREENW - paddleW, lPaddleY);
	
	//start TTF
	string nums[11] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
	SDL_Color color = { 0, 0, 0, 0 };
	font = TTF_OpenFont("makushka.ttf", 64);
	temp = TTF_RenderText_Blended(font, nums[0].c_str(), color);
	lScore = SDL_CreateTextureFromSurface(ren, temp);
	rScore = SDL_CreateTextureFromSurface(ren, temp);
	SDL_FreeSurface(temp);
	RenderTexture(lScore, ren, SCREENW / 2 - 64, 30);
	RenderTexture(rScore, ren, SCREENW / 2 + 64, 30);

	SDL_RenderPresent(ren);

	//main loop
	bool wait = false;
	int score;
	int lPoints = 0, rPoints = 0;
	int move = 30;
	bool end = false;
	while (!end && lPoints <= 10 && rPoints < 10 && lPoints < 10)
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

			//if left Paddle is at top or bot
			if (lPaddleY > SCREENH - paddleH)
			{
				lPaddleY = SCREENH - paddleH;
			}
			else if (lPaddleY < 0)
			{
				lPaddleY = 0;
			}
			//right paddle
			if (rPaddleY > SCREENH - paddleH)
			{
				rPaddleY = SCREENH - paddleH;
			}
			else if (rPaddleY < 0)
			{
				rPaddleY = 0;
			}

		}
		//clear screen
		SDL_RenderClear(ren);

		//render objects
		RenderTexture(background, ren, 0, 0);
		//check for point and move ball
		score = ball.checkScore();
		if (score == 1)
		{
			rPoints += 1;
			ball.StartPos();
			ball.Render();
			wait = true;
			lPaddleY = SCREENH / 2 - paddleH / 2;
			rPaddleY = SCREENH / 2 - paddleH / 2;
		}
		else if (score == 2)
		{
			lPoints += 1;
			ball.StartPos();
			ball.Render();
			wait = true;
			lPaddleY = SCREENH / 2 - paddleH / 2;
			rPaddleY = SCREENH / 2 - paddleH / 2;
		}
		else
		{
			ball.Move(lPaddleY, rPaddleY, paddleH, paddleW);
		}
		RenderTexture(lPaddle, ren, 0, lPaddleY);
		RenderTexture(rPaddle, ren, SCREENW - paddleW, rPaddleY);
		//score
		temp = TTF_RenderText_Blended(font, nums[lPoints].c_str(), color);
		lScore = SDL_CreateTextureFromSurface(ren, temp);
		SDL_FreeSurface(temp);
		temp = TTF_RenderText_Blended(font, nums[rPoints].c_str(), color);
		rScore = SDL_CreateTextureFromSurface(ren, temp);
		SDL_FreeSurface(temp);
		RenderTexture(lScore, ren, SCREENW / 2 - 64, 30);
		RenderTexture(rScore, ren, SCREENW / 2 + 64, 30);

		//update screen
		if (!wait)
		{
			SDL_RenderPresent(ren);
		}
		else
		{
			SDL_RenderPresent(ren);
			wait = false;
			SDL_Delay(1000);
		}

	}
	
	//Clean up
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(tex);
	SDL_DestroyTexture(lPaddle);
	SDL_DestroyTexture(rPaddle);
	SDL_DestroyTexture(lScore);
	SDL_DestroyTexture(rScore);

	TTF_CloseFont(font);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}

void RenderTexture(SDL_Texture *img, SDL_Renderer *ren, int x, int y)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;


	SDL_QueryTexture(img, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, img, NULL, &dst);

}
