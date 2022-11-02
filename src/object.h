#pragma once

enum Type { Player, Roamer, Shooter, Slugger};
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