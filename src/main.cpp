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
Object player { WW / 2, WH / 2, 64, 64, 500 };
Object enemy[5]{};
Object pbullet[20]{ WW / 2, WH / 2, 64, 64};
Object ebullet[]{ WW / 2, WH / 2, 64, 64};
int LastSpawn = 200;
int LastShot = 100;
//rand() % WW
int initial = 600;

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
	for (int i = 0; i < sizeof(enemy) / sizeof(Object); i++)
	{
		if (!enemy[i].live && LastSpawn >= 250)
		{
			enemy[i].box.w = 64;
			enemy[i].box.h = 64;
			enemy[i].live = true;
			enemy[i].box.x = initial;
			enemy[i].box.y = 0 - enemy[i].box.h;
			enemy[i].type = Roamer;
			enemy[i].direction = Right;
			enemy[i].speed = 500;
			LastSpawn = 0;
			break;
		}
	}
	LastSpawn++;
}

void PosUpdate(float dt)
{
	for (int i = 0; i <= 100; i++)
	{
		if (enemy[i].live)
		{
			switch (enemy[i].type)
			{
				case Roamer:
					enemy[i].box.y += (int)(enemy[i].speed / 10 * dt + 0.5f);
					if (enemy[i].direction == Right)
					{
						enemy[i].box.x += (int)(enemy[i].speed * dt + 0.5f);
					}
					else
					{
						enemy[i].box.x -= (int)(enemy[i].speed * dt + 0.5f);
					}
					if (enemy[i].box.x + enemy[i].box.w >= WW)
					{
						enemy[i].direction = Left;
					}
					if (enemy[i].box.x <= 0)
					{
						enemy[i].direction = Right;
					}
					if (enemy[i].box.y>= WH)
					{
						enemy[i].live = false;
					}
					break;
			}
		}
	}
}

void PlayerShoot ()
{
	for (int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		if (!pbullet[i].live && LastShot >= 60)
		{
			pbullet[i].box.w = 32;
			pbullet[i].box.h = 64;
			pbullet[i].live = true;
			pbullet[i].box.x = player.box.x + player.box.w / 2 - pbullet[i].box.w / 2;
			pbullet[i].box.y = player.box.y - pbullet[i].box.h;
			break;
		}
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
		player.box.y -= (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_DOWN))
	{
		player.box.y += (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_LEFT))
	{
		player.box.x -= (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		player.box.x += (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyPressed(SDL_SCANCODE_Z))
	{
		PlayerShoot();
	}
	EnemySpawn();
	PosUpdate(dt);
}

void RenderFrame(float interpolation)
{
	SDL_SetRenderDrawColor(gRenderer, 65, 105, 225, 255);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 65, 225, 225, 255);
	SDL_RenderFillRect(gRenderer, &player.box);
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
	for (int i = 0; i <= sizeof(enemy) / sizeof(Object); i++)
	{
		if (enemy[i].live)
		{
			SDL_RenderFillRect(gRenderer, &enemy[i].box);
		}
	}
	for (int i = 0; i <= sizeof(pbullet) / sizeof(Object); i++)
	{
		if (pbullet[i].live)
		{
			SDL_RenderFillRect(gRenderer, &pbullet[i].box);
		}
	}
}
