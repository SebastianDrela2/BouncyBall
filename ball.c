#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "include/SDL.h"
#include "circle.h"
#include "stack.h"

const size_t whiteColor = 0xFFFFFFFF;
const size_t backgroundColor = 0xFF202020;
const size_t windowWidth = 1000;
const size_t windowHeight = 600;

const double gravity = 0.5;
const double ballStartingVelocityX = 30;
const double ballStartingVelocityY = -10;
const double bounceDeepening = 0.97;

typedef struct
{
    Circle circle;
    StackCircle trailStack;
    Circle trail[20];
} Game;

size_t GenerateRandomHexColor() {
    srand(time(NULL));
    
    unsigned int a = 255;
    unsigned int r = rand() % 256;
    unsigned int g = rand() % 256;
    unsigned int b = rand() % 256;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void SDL_FillCircle(SDL_Surface* surface, Circle* circle, int r)
{
    circle->color = GenerateRandomHexColor();

    for (int x = -r; x <= r; x = x + 1)
    {
        for (int y = -r; y <= r; y = y + 1)
        {
            double distance = sqrt((x*x) + (y*y));

            if (distance <= r)
            {
                SDL_Rect rectangle;
                rectangle.w = 1;
                rectangle.h = 1;
                rectangle.x = x + circle->x;
                rectangle.y = y + circle->y;

                SDL_FillRect(surface, &rectangle, circle->color);
            }
        }
    }
}

void SDL_DrawTrailCircle(SDL_Surface* surface, Game* game)
{
    int trailCricleRadius = game->circle.radius;

    for (int i = game->trailStack.head - 1; i >= 0; --i)
    {
        SDL_FillCircle(surface, &game->trail[i], trailCricleRadius);
        trailCricleRadius *= 0.9;
    }

    if (game->trailStack.isFull != 0)
    {        
        for (int i = game->trailStack.size - 1; i >= game->trailStack.head; --i)
        {
            SDL_FillCircle(surface, &game->trail[i], trailCricleRadius);
            trailCricleRadius *= 0.9;
        }
    }
}

void SDL_AccelerateCircle(Circle* circle)
{
    circle->velocityY += gravity;

    circle->x = circle->x + circle->velocityX;
    circle->y = circle->y + circle->velocityY;
}

void SDL_UpdateBallVelocity(Circle* circle, int w, int h)
{
    if ((circle->y - circle->radius) <= 0)
    {
        circle->velocityY = -circle->velocityY;
        circle->velocityY *= bounceDeepening;
        circle->y = circle->radius;
    }
    else if ((circle->y + circle->radius) >= h)
    {
        circle->velocityY = -circle->velocityY;
        circle->velocityY *= bounceDeepening;
        circle->y = h - circle->radius;
    }
    if ((circle->x + circle->radius) >= w)
    {
        circle->velocityX = -circle->velocityX;
        circle->velocityX *= bounceDeepening;
        circle->x = w - circle->radius;
    }
    else if ((circle->x - circle->radius) <= 0)
    {
        circle->velocityX = -circle->velocityX;
        circle->velocityX *= bounceDeepening;
        circle->x = circle->radius;
    }
}

void SDL_SimulateGame(SDL_Window* window, SDL_Surface* surface, SDL_Rect* surfaceRectangle, Game* game)
{
    SDL_DrawTrailCircle(surface, game);
    SDL_FillCircle(surface, &game->circle, game->circle.radius);

    SDL_AccelerateCircle(&game->circle);
    SDL_UpdateBallVelocity(&game->circle, surface->w, surface->h);
    StackAdd(&game->trailStack, &game->circle);
    SDL_UpdateWindowSurface(window);

    SDL_Delay(16);
    SDL_FillRect(surface, surfaceRectangle, backgroundColor);
    SDL_UpdateWindowSurface(window);
}

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Ball", 100, 100, windowWidth, windowHeight, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window); 
    SDL_Event windowEvent;

    SDL_Rect surfaceRectangle;
    surfaceRectangle.x = 0;
    surfaceRectangle.y = 0;
    surfaceRectangle.w = windowWidth;
    surfaceRectangle.h = windowHeight;

    SDL_Rect rectangle;
    rectangle.w = 100;
    rectangle.h = 100;
    rectangle.x = (surface->w/2) - (rectangle.w/2);
    rectangle.y = (surface->h/2) - (rectangle.h/2);

    Circle circle;
    circle.radius = 50;
    circle.color = whiteColor;
    circle.x = (surface->w/2);
    circle.y = (surface->h/2);
    circle.velocityX = ballStartingVelocityX;
    circle.velocityY = ballStartingVelocityY;

    Game game;
    game.circle = circle;

    StackCircle trailStack;
    memset(&trailStack, 0, sizeof(StackCircle));
    trailStack.size = 20;
    trailStack.items = game.trail;

    game.trailStack = trailStack;

    while(1)
    {
        if(SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
            {
                break;
            }
        }
        
        SDL_SimulateGame(window, surface, &surfaceRectangle, &game);
    }
}