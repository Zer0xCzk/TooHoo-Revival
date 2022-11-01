#include <SDL.h>
#include "object.h"

/*
Don't know if this requires a .cpp file, but what I'm thinking is that the Object.type relates to what object it's supposed to be, and that dictates the behavior
type 0 = player NOTE: Could be multiple characters that you switch between
type 1 = pbullet
type 2 = ebullet
type 3 = "roamer" - Walks diagonally down bouncing from the sides. Doesn't shoot, but moves faster than the player
type 4 = "shooter" - Roamer, but it shoots bellow itself
type 5 = "slugger" - Most destructive. Shoots 4 shots in a / | | \ pattern, but can't move from it's initial possition
*/