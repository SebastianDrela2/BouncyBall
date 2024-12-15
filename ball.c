#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "include/SDL.h"
#include "circle.h"
#include "stack.h"

#define trailAmount 20

const size_t whiteColor = 0xFFFFFF;
const size_t orangeColor = 0xFFFFA500;
const size_t purplePinkColor = 0xFFFF00FF;
const size_t backgroundColor = 0xFF202020;
const size_t windowWidth = 1200;
const size_t windowHeight = 700;

const double gravity = 0.5;
const double ballStartingVelocityX = 50;
const double ballStartingVelocityY = -20;
const double bounceDeepening = 0.99;
const double moveMultiplier = 0.3;
const double timeMultiplier = 60;
const double trailFrequency = 1;

typedef struct
{
    Circle circle;
    StackCircle trailStack;
    double pendingTrail;
    Position trail[trailAmount];
} Game;

size_t GenerateRandomHexColor() {
    srand(time(NULL));

    unsigned int a = 255;
    unsigned int r = rand() % 256;
    unsigned int g = rand() % 256;
    unsigned int b = rand() % 256;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

size_t SDL_MultipleTrailColor(size_t color, double multiplier)
{
    size_t a = (color & 0xFF000000) >> 24;
    size_t r = (color & 0xFF0000) >> 16;
    size_t g = (color & 0xFF00) >> 8;
    size_t b = color & 0xFF;

    r = (size_t)(r * multiplier);
    g = (size_t)(g * multiplier);
    b = (size_t)(b * multiplier);

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

double lerp(double minA, double maxB, double t)
{
    return minA + (t * (maxB - minA));
}

void SDL_DrawHorizontalLine(SDL_Surface* surface, int x1, int x2, int y, size_t color) 
{
    if (y < 0 || y >= surface->h || x1 >= surface->w || x2 < 0)
    {
        return;
    }

    if (x1 < 0) 
    {
        x1 = 0;
    }

    if (x2 >= surface->w) 
    {
        x2 = surface->w - 1;
    }

    Uint32 *pixels = (Uint32*)surface->pixels;
    for (int x = x1; x <= x2; x++) {
        pixels[y * (surface->pitch / 4) + x] = color;
    }
}

void SDL_FillCircle(SDL_Surface* surface, Position position, int r, Uint32 color) {
    int cx = position.x;
    int cy = position.y;
    int x = 0;
    int y = r;
    int p = 1 - r;

    while (x <= y) {
        SDL_DrawHorizontalLine(surface, cx - x, cx + x, cy - y, color);
        SDL_DrawHorizontalLine(surface, cx - y, cx + y, cy - x, color);
        SDL_DrawHorizontalLine(surface, cx - x, cx + x, cy + y, color);
        SDL_DrawHorizontalLine(surface, cx - y, cx + y, cy + x, color);

        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

void SDL_FadeTrailCircle(SDL_Surface* surface, Position trail, double trailCircleRadius, double trailMultiplier, size_t color)
{
    size_t fadedColor = SDL_MultipleTrailColor(color, trailMultiplier);
    SDL_FillCircle(surface, trail, trailCircleRadius, fadedColor);
}

void SDL_DrawSingleTrail(SDL_Surface* surface, Position trail, double* drawnTrailCounter, double circleRadius)
{
    double trailMultiplier = *drawnTrailCounter / trailAmount;
    SDL_FadeTrailCircle(surface, trail, circleRadius * trailMultiplier, trailMultiplier, purplePinkColor);

    --*drawnTrailCounter;
}

void SDL_DrawTrailCircle(SDL_Surface* surface, Game* game)
{
    double drawnTrailCounter = trailAmount;
    double trailMultiplier = 1;

    for (int i = game->trailStack.head - 1; i >= 0; --i)
    {
        SDL_DrawSingleTrail(surface, game->trail[i], &drawnTrailCounter, game->circle.radius);
    }

    if (game->trailStack.isFull != 0)
    {        
        for (int i = game->trailStack.size - 1; i >= game->trailStack.head; --i)
        {
            SDL_DrawSingleTrail(surface, game->trail[i], &drawnTrailCounter, game->circle.radius);
        }
    }
}

void SDL_AccelerateCircle(Circle* circle, double deltaTime)
{
    if (deltaTime == 0)
    {
        return;
    }

    circle->velocityY += gravity * deltaTime * moveMultiplier;

    circle->position.x += (circle->velocityX * deltaTime) * moveMultiplier;
    circle->position.y += (circle->velocityY * deltaTime) * moveMultiplier;
}

void SDL_CheckBoundsBounce(Circle* circle, int w, int h)
{
    if ((circle->position.y - circle->radius) < 0) // TOP
    {
        circle->velocityY *= -bounceDeepening;
        circle->position.y = 0 + circle->radius;
    }
    else if ((circle->position.y + circle->radius) > h) // BOTTOM
    {
        circle->velocityY *= -bounceDeepening;
        circle->position.y = h - circle->radius;
    }

    if ((circle->position.x + circle->radius) > w) // RIGHT
    {
        circle->velocityX *= -bounceDeepening;
        circle->position.x = w - circle->radius;
    }
    else if ((circle->position.x - circle->radius) < 0) // LEFT
    {
        circle->velocityX *= -bounceDeepening;
        circle->position.x = 0 + circle->radius;
    }
}

void SDL_SimulateGame(SDL_Window* window, SDL_Surface* surface, SDL_Rect* surfaceRectangle, Game* game, double deltaTime)
{
    SDL_FillRect(surface, surfaceRectangle, backgroundColor);

    SDL_DrawTrailCircle(surface, game);
    SDL_FillCircle(surface, game->circle.position, game->circle.radius, whiteColor);

    SDL_AccelerateCircle(&game->circle, deltaTime);
    SDL_CheckBoundsBounce(&game->circle, surface->w, surface->h);

    game->pendingTrail += deltaTime * trailFrequency;

    while (game->pendingTrail > 1)
    {
        game->pendingTrail--;
        StackAdd(&game->trailStack, game->circle.position);
    }
    
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
    surfaceRectangle.w = surface->w;
    surfaceRectangle.h = surface->h;

    SDL_Rect rectangle;
    rectangle.w = 100;
    rectangle.h = 100;
    rectangle.x = (surface->w/2) - (rectangle.w/2);
    rectangle.y = (surface->h/2) - (rectangle.h/2);

    Circle circle;
    circle.radius = 60;
    circle.color = whiteColor;
    circle.position.x = (surface->w/2);
    circle.position.y = (surface->h/2);
    circle.velocityX = ballStartingVelocityX;
    circle.velocityY = ballStartingVelocityY;

    Game game;
    game.pendingTrail = 0;
    game.circle = circle;

    StackCircle trailStack;
    memset(&trailStack, 0, sizeof(StackCircle));
    trailStack.size = trailAmount;
    trailStack.items = game.trail;

    game.trailStack = trailStack;

    Uint64 previousTicks = SDL_GetPerformanceCounter();
    const Uint64 frequency = SDL_GetPerformanceFrequency();

    double deltaTime = 0;

    while(1)
    {
        if(SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
            {
                break;
            }
        }
        
        SDL_SimulateGame(window, surface, &surfaceRectangle, &game, deltaTime);

        Uint64 currentTicks = SDL_GetPerformanceCounter();
        deltaTime = (double)(currentTicks - previousTicks) / frequency;
        deltaTime *= timeMultiplier;

        previousTicks = currentTicks;  
    }
}