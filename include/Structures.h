#ifndef STRUCTURES_H
#define STRUCTURES_H

#pragma once
#include "SDL.h"
#include "Constants.h"

struct Beer {
    int x, y, speed;
    Uint32 spawnTime;
};

struct X2Icon {
    int x, y;
    bool active;
    Uint32 spawnTime;
    static const int WIDTH = 40;
    static const int HEIGHT = 40;
    static const int FALL_SPEED = 8;
};

#endif // STRUCTURES_H
