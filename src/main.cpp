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

Object player{ WW / 2, WH / 2, 64, 64, 500 };
Object enemy[10]{};
Object pbullet[10]{};
Object ebullet[40]{};
int lastspawn = 200;
int lastshot = 0;
int elastshot = 120;
int curbullet = 0;
int curenemy = 0;

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
	// Not actually random, idk, idc
	int initial = rand() % WW;
	for (int i = 0; i < sizeof(enemy) / sizeof(Object); i++)
	{
		if (!enemy[i].live && lastspawn >= 100)
		{
			enemy[i].box.w = 64;
			enemy[i].box.h = 64;
			enemy[i].live = true;
			enemy[i].box.x = initial;
			enemy[i].box.y = 100 - enemy[i].box.h;
			enemy[i].type = Shooter;
			enemy[i].direction = Right;
			enemy[i].speed = 400;
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

void EnemyShoot()
{
	for (int i = 0; i < sizeof(ebullet) / sizeof(Object); i++)
	{
		switch (enemy[i].type)
			case Shooter:
				if (!ebullet[i].live && elastshot >= 120)
				{
					ebullet[i].box.w = 32;
					ebullet[i].box.h = 64;
					ebullet[i].live = true;
					ebullet[i].box.x = enemy[i].box.x + enemy[i].box.w / 2 - ebullet[i].box.w / 2;
					ebullet[i].box.y = enemy[i].box.y + ebullet[i].box.h;
					ebullet[i].speed = 1000;
					break;
				}
				elastshot++;
	}
	if (elastshot > 120)
	{
		elastshot = 0;
	}
}

void ColUpdate()
{
	for (int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		for (int j = 0; j < sizeof(enemy) / sizeof(Object); j++)
		{
			SDL_Point left_top = { pbullet[i].box.x, pbullet[i].box.y };
			SDL_Point right_top = { pbullet[i].box.x + pbullet[i].box.w, pbullet[i].box.y };
			if (SDL_PointInRect(&left_top, &enemy[j].box) || SDL_PointInRect(&right_top, &enemy[j].box))
			{
				enemy[j].live = false;
				pbullet[i].live = false;
				enemy[j].box.y = -WH;
				enemy[j].box.x = -WW;
 				pbullet[i].box.x = 2 * WW;
				pbullet[i].box.y = 2 * WH;
			}
		}
	}
}

void PosUpdate(float dt)
{
	for (int i = 0; i <= 40; i++)
	{
		int tmp1 = sizeof(enemy) / sizeof(Object);
		if (sizeof(enemy) / sizeof(Object) >= i)
		{
			if (enemy[i].live)
			{
				switch (enemy[i].type)
				{
				case Shooter:
					if (enemy[i].box.y <= player.box.y)
					{
						EnemyShoot();
					}
				case Roamer:
					enemy[i].box.y += (int)(enemy[i].speed / 4 * dt + 0.5f);
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
		}
		if (sizeof(pbullet) / sizeof(Object) >= i)
		{
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
		if (sizeof(ebullet) / sizeof(Object) >= i)
		{
			if (ebullet[i].live)
			{
				ebullet[i].box.y += (int)(ebullet[i].speed * dt + 0.5f);
				if (ebullet[i].box.y >= WH)
				{
					ebullet[i].live = false;
					ebullet[i].box.x = WW + WW;
					ebullet[i].box.y = WH + WH;
				}
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
	if (IsKeyDown(SDL_SCANCODE_LEFT) && player.box.x >= 0)
	{
		player.box.x -= (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_RIGHT) && player.box.x + player.box.w <= WW)
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
	for (int i = 0; i <= sizeof(ebullet) / sizeof(Object); i++)
	{
		if (ebullet[i].live)
		{
			SDL_RenderFillRect(gRenderer, &ebullet[i].box);
		}
	}
}
