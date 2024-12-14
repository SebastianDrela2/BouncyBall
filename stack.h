#pragma once

#include "circle.h"

typedef struct
{
    int head;
    int tail;
    int size;
    int isFull;
    Position* items;
} StackCircle;

void StackAdd(StackCircle* stack, Position position);

// TEMPLATE

// void StackLoop(StackCircle* stack)
// {
//     if (stack->head > stack->tail)
//     {
//         for(int i = stack->head - 1; i >= 0; --i)
//         {

//         }

//         for (int i = stack->size - 1; i >= stack->tail; i--)
//         {

//         }
//     }
//     else
//     {
//         for (int i = stack->head - 1; i >= stack->tail; i--)
//         {

//         }
//     }
// }

