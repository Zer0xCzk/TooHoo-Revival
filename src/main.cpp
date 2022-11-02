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
Object enemy[10]{};
Object pbullet[10]{};
Object ebullet[]{ WW / 2, WH / 2, 64, 64};
int lastspawn = 200;
int lastshot = 10;
int curbullet = 0;
int curenemy = 0;
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
		if (!enemy[i].live && lastspawn >= 100)
		{
			enemy[i].box.w = 64;
			enemy[i].box.h = 64;
			enemy[i].live = true;
			enemy[i].box.x = initial;
			enemy[i].box.y = 0 - enemy[i].box.h;
			enemy[i].type = Roamer;
			enemy[i].direction = Right;
			enemy[i].speed = 300;
			lastspawn = 0;
			break;
		}
	}
	lastspawn++;
}

void PlayerShoot()
{
	for (int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		if (!pbullet[i].live && lastshot >= 10)
		{
			pbullet[i].box.w = 32;
			pbullet[i].box.h = 64;
			pbullet[i].live = true;
			pbullet[i].box.x = player.box.x + player.box.w / 2 - pbullet[i].box.w / 2;
			pbullet[i].box.y = player.box.y - pbullet[i].box.h;
			pbullet[i].speed = 1000;
			lastshot = 0;
			break;
		}
	}
	lastshot++;
}

void ColUpdate()
{
	for (int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		for (int j = 0; j < sizeof(enemy) / sizeof(Object); j++)
		{
			// HasIntersection doesn't work here, apologies for this mess, I tried to make it better with comments
			// left top of bullet below top of enemy    left top of bullet above bottom of enemy              left top of bullet right of the enemy    left top of bullet left of the enemy
			if ((pbullet[i].box.y >= enemy[j].box.y && pbullet[i].box.y <= enemy[j].box.y + enemy[j].box.h) && (pbullet[i].box.x >= enemy[j].box.x && pbullet[i].box.x <= enemy[j].box.x + enemy[j].box.w))
			{
				enemy[j].live = false;
				pbullet[i].live = false;
				enemy[j].box.y = 0 - WH;
				enemy[j].box.x = 0 - WW;
 				pbullet[i].box.x = WW + WW;
				pbullet[i].box.y = WH + WH;
			}
			//                right top of bullet below top of enemy                  right top of bullet above bottom of enemy                                     right top of bullet right of the enemy                        right top of bullet left of the enemy
			else if ((pbullet[i].box.y + pbullet[i].box.h >= enemy[j].box.y && pbullet[i].box.y + pbullet[i].box.h <= enemy[j].box.y + enemy[j].box.h) && (pbullet[i].box.y + pbullet[i].box.w >= enemy[j].box.x && pbullet[i].box.y + pbullet[i].box.w <= enemy[j].box.x + enemy[j].box.w))
			{
				enemy[j].live = false;
				pbullet[i].live = false;
				enemy[j].box.y = 0 - WH;
				enemy[j].box.x = 0 - WW;
				pbullet[i].box.x = WW + WW;
				pbullet[i].box.y = WH + WH;
			}
		}
	}
}

void PosUpdate(float dt)
{
	for (int i = 0; i <= 20; i++)
	{
		if (enemy[i].live)
		{
			switch (enemy[i].type)
			{
			case Roamer:
				enemy[i].box.y += (int)(enemy[i].speed / 5 * dt + 0.5f);
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
				if (enemy[i].box.y >= WH)
				{
					enemy[i].live = false;
				}
				break;
			}
		}
		if (pbullet[i].live)
		{	
			pbullet[i].box.y -= (int)(pbullet[i].speed * dt + 0.5f);
			if (pbullet[i].box.y + pbullet[i].box.h <= 0)
			{
				pbullet[i].live = false;
				pbullet[i].box.x = WW + WW;
				pbullet[i].box.y = WH + WH;
			}
		}
	}
}

void Update(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		ExitGame();
	}
	if (IsKeyDown(SDL_SCANCODE_UP) && player.box.y >= 0)
	{
		player.box.y -= (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_DOWN) && player.box.y + player.box.h <= WH)
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
	if (IsKeyDown(SDL_SCANCODE_Z))
	{
		PlayerShoot();
	}
	EnemySpawn();
	ColUpdate();
	PosUpdate(dt);
}

void RenderFrame(float interpolation)
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
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
