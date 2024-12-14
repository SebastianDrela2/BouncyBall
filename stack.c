#include "stack.h"

void StackAdd(StackCircle* stack, Circle* circle)
{
    stack->items[stack->head] = *circle;
    stack->head = stack->head + 1;

    if (stack->head >= stack->size)
    {
        stack->head = 0;
        stack->isFull = 1;
    }
    
    if (stack->isFull != 0)
    {
        stack->tail = stack->head;
    }
}