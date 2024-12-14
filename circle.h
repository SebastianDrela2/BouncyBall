#pragma once

#include <stdlib.h>

typedef struct 
{
    int x;
    int y;
} Position;

typedef struct
{
    Position position;
    int radius;
    size_t color;
    double velocityX;
    double velocityY;
} Circle;

