#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "include/SDL.h"

const size_t whiteColor = 0xFFFFFFFF;
const size_t blackColor = 0xFF000000;
const size_t windowWidth = 1000;
const size_t windowHeight = 600;
const double gravity = 0.5;

struct Circle
{
    int x;
    int y;
    int radius;
    size_t color;
    double velocityX;
    double velocityY;
};

size_t GenerateRandomHexColor() {
    srand(time(NULL));

    unsigned int a = 255;
    unsigned int r = rand() % 256;
    unsigned int g = rand() % 256;
    unsigned int b = rand() % 256;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void SDL_FillCircle(SDL_Surface* surface, struct Circle* circle)
{
    int r = circle->radius;

    for (int x = -r; x <= r; x = x + 1)
    {
        for (int y = -r; y <= r; y = y + 1)
        {
            double distance = sqrt((x*x) + (y*y));

            if (distance <= circle->radius)
            {
                SDL_Rect rectangle;
                rectangle.w = 1;
                rectangle.h = 1;
                rectangle.x = x + circle->x;
                rectangle.y = y + circle->y;
                circle->color = GenerateRandomHexColor();
                SDL_FillRect(surface, &rectangle, circle->color);
            }
        }
    }
}

void SDL_AccelerateCircle(struct Circle* circle)
{
    circle->velocityY += gravity;

    circle->x = circle->x + circle->velocityX;
    circle->y = circle->y + circle->velocityY;
}

void SDL_UpdateBallVelocity(struct Circle* circle, int w, int h)
{
    int bounced = 0;

    if ((circle->y - circle->radius) <= 0)
    {
        circle->velocityY = -circle->velocityY;
        circle->velocityY *= 0.95;
        circle->y = circle->radius;
        bounced = 1;
    }
    else if ((circle->y + circle->radius) >= h)
    {
        circle->velocityY = -circle->velocityY;
        circle->velocityY *= 0.95;
        circle->y = h - circle->radius;
        bounced = 1;
    }
    if ((circle->x + circle->radius) >= w)
    {
        circle->velocityX = -circle->velocityX;
        circle->velocityX *= 0.95;
        circle->x = w - circle->radius;
        bounced = 1;
    }
    else if ((circle->x - circle->radius) <= 0)
    {
        circle->velocityX = -circle->velocityX;
        circle->velocityX *= 0.95;
        circle->x = circle->radius;
        bounced = 1;
    }

    // if (bounced == 1)
    // {
    //     circle->color = GenerateRandomHexColor();
    // }
}

void SDL_SimulateCircle(SDL_Window* window, SDL_Surface* surface, SDL_Rect* surfaceRectangle, struct Circle* circle)
{
    SDL_FillCircle(surface, circle);
    SDL_AccelerateCircle(circle);
    SDL_UpdateBallVelocity(circle, surface->w, surface->h);
    SDL_UpdateWindowSurface(window);

    SDL_Delay(16);
    SDL_FillRect(surface, surfaceRectangle, blackColor);
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

    struct Circle circle;
    circle.radius = 50;
    circle.color = whiteColor;
    circle.x = (surface->w/2);
    circle.y = (surface->h/2);
    circle.velocityX = 50;
    circle.velocityY = -20;

    while(1)
    {
        if(SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
            {
                break;
            }
        }
        
        SDL_SimulateCircle(window, surface, &surfaceRectangle, &circle);
    }
}