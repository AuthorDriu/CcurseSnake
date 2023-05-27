/*
    Игра в змейку на curses
*/

#include <curses.h>
#include <stdlib.h>

#define SCR_WIDTH       12
#define SCR_HEIGHT      12


// Элемент змейки
typedef struct SnakeChain
{
    int x;
    int y;
    SnakeChain* next;
} SnakeChain;


// Создает новый элемент цепи без основы
SnakeChain* createChain(int x, int y)
{
    SnakeChain* snake;
    
    snake->x = x;
    snake->y = y;
    snake->next = NULL;
    
    return snake;
}

// Создает новый элемент цепи с основой
int createChain(SnakeChain* snake, short direcrion)
{
    if (snake->next == NULL)
    {
        int x, y;

        if (direcrion == 0)
        {
            x = snake->x;
            y = snake->y - 1;
        }
        else if (direcrion == 1)
        {
            x = snake->x;
            y = snake->y + 1;
        }
        else if (direcrion == 2)
        {
            x = snake->x - 1;
            y = snake->y;
        }
        else if (direcrion == 3)
        {
            x = snake->x + 1;
            y = snake->y;
        }

        SnakeChain* new_snake = createChain(x, y);
        snake->next = new_snake;
    }
    else
    {
        createChain(snake->next, direcrion);
    }

    return 0;
}

// Двигает змейку
int snakeMove(SnakeChain* snake, short direction)
{
    // Если элемент последний, мы продвигаем его по направлению
    if (snake->next == NULL)
    {
        if (direction == 0)
        {
            snake->y -= 1;
        }
        else if (direction == 1)
        {
            snake->y += 1;
        }
        else if (direction == 2)
        {
            snake->x -= 1;
        }
        else if (direction == 3)
        {
            snake->x += 1;
        }
        
        return 0;
    }
    else
    {
        snake->x = snake->next->x;
        snake->y = snake->next->y;

        return snakeMove(snake->next, direction);
    }
}

