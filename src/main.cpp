#include "engine.h"
#include "sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "object.h"

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);

#define WW 1200
#define WH 900

Object LBorder{0, 0, WW / 3, WH, 0, Left, Default, false, false };
Object RBorder{WW - WW/3, 0, WW / 3, WH, 0, Right, Default, false, false  };
Object player{WW / 2, 3*WH / 4, 48, 48, 500, Right, Default, true, true };
Object enemy[15] {0, 0, 48, 48, 500, Left, Default, false, true};
Object pbullet[10] {0, 0, 48, 16, 1000, Right, Default, false, true};
Object ebullet[40] {0, 0, 48, 16, 1000, Left, Default, false, true};;
float lastspawn = 80;
float lastshot = 15;
float elastshot = 60;
bool initdir;
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

	srand((unsigned int)time(NULL));

	StartLoop(Update, RenderFrame);

	CleanUp();


	return 0;
}

//=============================================================================

void EnemySpawn(float dt)
{
	int initial = rand() % (WW/3 - 64) + WW / 3;
	int etype = rand() % 2 + 1;
	for (long unsigned int i = 0; i < sizeof(enemy) / sizeof(Object); i++)
	{
		if (!enemy[i].live && lastspawn >= (30 * dt))
		{
			enemy[i].box.w = 48;
			enemy[i].box.h = 48;
			enemy[i].live = true;
			enemy[i].box.x = initial;
			enemy[i].box.y = 0 - enemy[i].box.h;
			enemy[i].type = (Type)etype;
			switch (rand() % 1)
			{
			case 0:
				enemy[i].direction = Left;
				break;
			case 1:
				enemy[i].direction = Right;
				break;
			}
			enemy[i].speed = 400;
			lastspawn = 0;
			break;
		}
	}
	lastspawn += dt;
}

void PlayerShoot(float dt)
{
	for (long unsigned int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		if (!pbullet[i + 1].live && lastshot >= (12 * dt))
		{
			pbullet[i + 1].box.w = 16;
			pbullet[i + 1].box.h = 48;
			pbullet[i + 1].live = true;
			pbullet[i + 1].box.x = player.box.x + player.box.w / 2 - pbullet[i + 1].box.w / 2;
			pbullet[i + 1].box.y = player.box.y - pbullet[i + 1].box.h;
			pbullet[i + 1].speed = 1000;
			lastshot = 0;
			break;
		}
	}
	lastshot += dt;
}

void EnemyShoot(float dt)
{
	// This solution is very temporary, every enemy gets it's own bullet, the Slugger needs four, how I'll do that remains to be seen
	for (long unsigned int i = 0; i < sizeof(ebullet) / sizeof(Object); i++)
	{
		switch (enemy[i].type)
		{
			case Default:
			//fallthrough
			case Slugger:
			//fallthrough
			case Roamer:
				break;
			case Shooter:
				// I am SO sorry for this fix, but bullet 0 just doesn't work, I HAVE to offset it
				if (!ebullet[i + 1].live && elastshot >= 60 * dt && enemy[i].live)
				{
					ebullet[i + 1].box.w = 16;
					ebullet[i + 1].box.h = 48;
					ebullet[i + 1].live = true;
					ebullet[i + 1].box.x = enemy[i].box.x + enemy[i].box.w / 2 - ebullet[i + 1].box.w / 2;
					ebullet[i + 1].box.y = enemy[i].box.y + ebullet[i + 1].box.h;
					ebullet[i + 1].speed = 1000;
					break;
				}
		}
	}
	elastshot += dt;
	if (elastshot > (60 + 1)* dt)
	{
		elastshot = 0;
	}
}

void ColUpdate()
{
	// Point of pbullet in Enemy
	for (long unsigned int i = 0; i < sizeof(pbullet) / sizeof(Object); i++)
	{
		for (long unsigned int j = 0; j < sizeof(enemy) / sizeof(Object); j++)
		{
			SDL_Point left_top = { pbullet[i].box.x, pbullet[i].box.y };
			SDL_Point right_top = { pbullet[i].box.x + pbullet[i].box.w, pbullet[i].box.y };
			if (SDL_PointInRect(&left_top, &enemy[j].box) || SDL_PointInRect(&right_top, &enemy[j].box))
			{
				enemy[j].live = false;
				pbullet[i].live = false;
				enemy[j].box.y = 0;
				enemy[j].box.x = 0;
 				pbullet[i].box.x = 0;
				pbullet[i].box.y = 0;
			}
		}
	}
	// Point of ebullet in Player
	for (long unsigned int i = 0; i < sizeof(ebullet) / sizeof(Object); i++)
	{
		SDL_Point left_bottom = { ebullet[i].box.x, ebullet[i].box.y + ebullet[i].box.h};
		SDL_Point right_bottom = { ebullet[i].box.x + ebullet[i].box.w, ebullet[i].box.y + ebullet[i].box.h};
		if (SDL_PointInRect(&left_bottom, &player.box) || SDL_PointInRect(&right_bottom, &player.box))
		{
			ExitGame();
		}
	}
	// Point of Enemy in Player
	for (long unsigned int i = 0; i < sizeof(enemy); i++)
	{
		SDL_Point left_bottom = { enemy[i].box.x, enemy[i].box.y + enemy[i].box.h };
		SDL_Point right_bottom = { enemy[i].box.x + enemy[i].box.w, enemy[i].box.y + enemy[i].box.h};
		if ((SDL_PointInRect(&left_bottom, &player.box) || SDL_PointInRect(&right_bottom, &player.box)) && (enemy[i].type == Roamer ))
		{
			ExitGame();
		}
	}
}

void PosUpdate(float dt)
{
	for (long unsigned int i = 0; i <= 40; i++)
	{
		if (sizeof(enemy) / sizeof(Object) >= i)
		{
			if (enemy[i].live)
			{
				switch (enemy[i].type)
				{
					case Roamer:
						enemy[i].box.y += (int)(enemy[i].speed / 2 * dt + 0.5f);
						//fallthrough
					case Slugger:
						if (enemy[i].direction == Right)
						{
							enemy[i].box.x += (int)(enemy[i].speed * dt + 0.5f);
						}
						else
						{
							enemy[i].box.x -= (int)(enemy[i].speed * dt + 0.5f);
						}
					case Default:
						break;
					//Note: Could be done better with fallthrough somewhere
					case Shooter:
						if (enemy[i].box.y < 25)
						{
							enemy[i].box.y += (int)(enemy[i].speed / 2 * dt + 0.5f);
						}
						else
						{
							if (enemy[i].direction == Right)
							{
								enemy[i].box.x += (int)(enemy[i].speed * dt + 0.5f);
							}
							else
							{
								enemy[i].box.x -= (int)(enemy[i].speed * dt + 0.5f);
							}
						}
				}
				if (enemy[i].box.x + enemy[i].box.w > 2*WW/3)
				{
					enemy[i].direction = Left;
				}
				if (enemy[i].box.x < WW/3)
				{
					enemy[i].direction = Right;
				}
				if (enemy[i].box.y > WH)
				{
					enemy[i].live = false;
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
	if (IsKeyDown(SDL_SCANCODE_DOWN) && (player.box.y + 48) <= WH)
	{
		player.box.y += (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_LEFT) && player.box.x > WW/3)
	{
		player.box.x -= (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_RIGHT) && player.box.x + player.box.w < 2*WW/3)
	{
		player.box.x += (int)(player.speed * dt + 0.5f);
	}
	if (IsKeyDown(SDL_SCANCODE_Z))
	{
		PlayerShoot(dt);
	}
	if (IsKeyUp(SDL_SCANCODE_Z))
	{
		lastshot = 15;
	}
	EnemyShoot(dt);
	EnemySpawn(dt);
	ColUpdate();
	PosUpdate(dt);
}

void RenderFrame(float interpolation)
{
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderFillRect(gRenderer, &LBorder.box);
	SDL_RenderFillRect(gRenderer, &RBorder.box);
	SDL_SetRenderDrawColor(gRenderer, 65, 225, 225, 255);
	SDL_RenderFillRect(gRenderer, &player.box);
	SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
	for (long unsigned int i = 0; i <= sizeof(enemy) / sizeof(Object); i++)
	{
		if (enemy[i].live)
		{
			SDL_RenderFillRect(gRenderer, &enemy[i].box);
		}
	}
	for (long unsigned int i = 0; i <= sizeof(pbullet) / sizeof(Object); i++)
	{
		if (pbullet[i].live)
		{
			SDL_RenderFillRect(gRenderer, &pbullet[i].box);
		}
	}
	for (long unsigned int i = 0; i <= sizeof(ebullet) / sizeof(Object); i++)
	{
		if (ebullet[i].live)
		{
			SDL_RenderFillRect(gRenderer, &ebullet[i].box);
		}
	}
}
