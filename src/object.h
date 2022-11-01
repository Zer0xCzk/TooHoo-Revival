#pragma once

enum Type { Player, EBullet, PBullet, Roamer, Shooter, Slugger};
enum Heading { Left, Right };

struct Object
{
	SDL_Rect box;
	double speed;
	Heading direction;
	Type type;
	bool live;
	bool free;
};