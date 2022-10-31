#include "engine.h"
#include "sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "object.h"

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);

#define WW 1920
#define WH 1080

//NOTE: FIGURE OUT HOW TO  SPRITES
Object Player { WW / 2, WH / 2, 64, 64, 500 };
Object Enemy[]{ WW / 2, WH / 2, 64, 64};
Object PBullet[]{ WW / 2, WH / 2, 64, 64};
Object EBullet[]{ WW / 2, WH / 2, 64, 64};
int LastSpawn = 0;

//=============================================================================
int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("TooHoo: Revival", WW, WH))
	{
		return 1;
	}

	StartLoop(Update, RenderFrame);

	CleanUp();


	return 0;
}

//=============================================================================

void EnemySpawn()
{
	//rand() % WW
	int initial = 600;
	if (LastSpawn > 2)
	{
		for (int i = 0; i <= sizeof(Enemy); i++)
		{
			if (Enemy[i].alive == 0)
			{
				Enemy[i].alive = true;
				Enemy[i].box.x = initial;
				Enemy[i].box.y = 0;
			}
		}
		LastSpawn = 0;
	}
	else
	{
		LastSpawn++;
	}
}

void Update(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		ExitGame();
	}
	if (IsKeyDown(SDL_SCANCODE_UP))
	{
		Player.box.y -= (int)(Player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_DOWN))
	{
		Player.box.y += (int)(Player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_LEFT))
	{
		Player.box.x -= (int)(Player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		Player.box.x += (int)(Player.speed * dt + 0.5f);
	}
	EnemySpawn();
}

void RenderFrame(float interpolation)
{
	SDL_SetRenderDrawColor(gRenderer, 65, 105, 225, 255);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 65, 225, 225, 255);
	SDL_RenderFillRect(gRenderer, &Player.box);
	for (int i = 0; i <= sizeof(Enemy); i++)
	{
		if (Enemy[i].alive == 1)
		{
			SDL_SetRenderDrawColor(gRenderer, 65, 0, 255, 0);
			SDL_RenderFillRect(gRenderer, &Enemy[i].box);
		}
	}
}
