#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>
#include <SDL_mixer.h>

// Screen dimensions
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 800;
const int GAME_AREA_WIDTH = 600;
const int INFO_AREA_WIDTH = 300;

// Player and beer dimensions
const int PLAYER_WIDTH = 150;
const int PLAYER_HEIGHT = 80;
const int BEER_WIDTH = 40;
const int BEER_HEIGHT = 120;

// Game mechanics
const int PLAYER_SPEED = 10;
const float INITIAL_BEER_SPEED = 3.7;
const int BEER_DELAY = 300;
const int BUTTON_SIZE = 40;
const int BUTTON_MARGIN = 10;

// X2 effect constants
const int X2_SPAWN_INTERVAL = 50000;
const int X2_EFFECT_DURATION = 8000;
const int X2_MIN_BEERS = 35;

#endif // CONSTANTS_H
