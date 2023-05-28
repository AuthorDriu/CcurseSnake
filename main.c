/*
    Игра в змейку на curses

    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
*/

#include <ncurses.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SCR_WIDTH       20
#define SCR_HEIGHT      20

#define WALL_CHAR       '#'
#define WALL_STR        "#"

#define SNAKE_BODY      "$"  

#define APPLE           "O"


/*
    ЗМЕЙКА
*/

typedef struct Coords
{
    int y;
    int x;
} Coords;

// Элемент змейки()
typedef struct SnakeChain
{
    Coords coords;
    struct SnakeChain* next;
} SnakeChain;

typedef struct Apple
{
    Coords coords;
} Apple;

// Создает новый элемент цепи без основы
SnakeChain* createNewChain(int x, int y)
{
    SnakeChain* snake = malloc(sizeof(SnakeChain));
    
    snake->coords.x = x;
    snake->coords.y = y;
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
            x = snake->coords.x;
            y = snake->coords.y - 1;
        }
        else if (direcrion == 1)
        {
            x = snake->coords.x;
            y = snake->coords.y + 1;
        }
        else if (direcrion == 2)
        {
            x = snake->coords.x - 1;
            y = snake->coords.y;
        }
        else if (direcrion == 3)
        {
            x = snake->coords.x + 1;
            y = snake->coords.y;
        }

        SnakeChain* new_snake = createNewChain(x, y);
        snake->next = new_snake;
    }
    else
    {
        createChain(snake->next, direcrion);
    }

    return 0;
}

// Удлиняет змейку в нужном направлении
int snakeGrow(SnakeChain* snake, short direction)
{
    if (snake->next == NULL)
    {
        SnakeChain* new_snake = malloc(sizeof(SnakeChain));

        if (direction == 0)
        {
            new_snake->coords.y = snake->coords.y - 1;
            new_snake->coords.x = snake->coords.x;
        }
        else if (direction == 1)
        {
            new_snake->coords.y = snake->coords.y + 1;
            new_snake->coords.x = snake->coords.x;
        }
        else if (direction == 2)
        {
            new_snake->coords.y = snake->coords.y;
            new_snake->coords.x = snake->coords.x - 1;
        }
        else if (direction == 3)
        {
            new_snake->coords.y = snake->coords.y;
            new_snake->coords.x = snake->coords.x + 1;
        }
        
        snake->next = new_snake;

        return 0;
    }
    else
    {
        return snakeGrow(snake->next, direction);
    }
}

// Двигает змейку
int snakeMove(SnakeChain* snake, short direction)
{
    // Если элемент последний, мы продвигаем его по направлению
    if (snake->next == NULL)
    {
        if (direction == 0)
        {
            snake->coords.y -= 1;
        }
        else if (direction == 1)
        {
            snake->coords.y += 1;
        }
        else if (direction == 2)
        {
            snake->coords.x -= 1;
        }
        else if (direction == 3)
        {
            snake->coords.x += 1;
        }

        return 0;
    }
    else
    {
        snake->coords.x = snake->next->coords.x;
        snake->coords.y = snake->next->coords.y;

        return snakeMove(snake->next, direction);
    }
}

// Возвращает 1 если по указанным координатам существует звено змейки, в противном случае 0
int inSnake(SnakeChain* snake, int x, int y)
{
    if ((snake->coords.x == x) && (snake->coords.y == y))
    {
        return 1;
    }
    else
    {
        if (snake->next == NULL)
        {
            return 0;
        }

        return inSnake(snake->next, x, y);
    }
}

/*
    ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ ЯБЛОКИ
*/

// Кидает яблоко на случайную точку на карте, избегая змейки
int throwApple(Apple* apple, SnakeChain* snake)
{
    int x, y;
    do
    {
        x = (int) (rand() % (SCR_WIDTH - 2) + 1);
        y = (int) (rand() % (SCR_HEIGHT - 2) + 1);
    } while (inSnake(snake, x, y));
    
    apple->coords.x = x;
    apple->coords.y = y;

    return 0;

}

/*
    ОТРИСОВКА
*/

// Вырисовывает игровое поле
int drawField()
{
    for (int y = 0; y < SCR_HEIGHT; ++y)
    {
        if ((y == 0) || (y == SCR_HEIGHT - 1))
        {
            char wall[SCR_WIDTH + 1];
            wall[SCR_WIDTH] = '\0';

            for (int x = 0; x < SCR_WIDTH; ++x)
                wall[x] = WALL_CHAR;
            move(y, 0);
            printw(wall);
        }
        else
        {
            move(y, 0);
            printw(WALL_STR);
            move(y, SCR_WIDTH - 1);
            printw(WALL_STR);
        }
    }
    return 0;
}

int drawSnake(SnakeChain* snake)
{
    move(snake->coords.y, snake->coords.x);
    printw(SNAKE_BODY);

    if (snake->next == NULL)
    {
        return 0;
    }
    else
    {
        return drawSnake(snake->next);
    }
}

int drawApple(Apple* apple)
{
    move(apple->coords.y, apple->coords.x);
    printw(APPLE);

    return 0;
}

int draw(SnakeChain* snake, Apple* apple)
{
    drawField();
    drawApple(apple);
    drawSnake(snake);

    move(SCR_HEIGHT, SCR_WIDTH);

    return 0;
}

/*
    СЧИТЫВАНИЕ НАЖАТИЙ
    w (W) -> вверх      код движения: 0     код ASCII: l-119, U-87
    d (D) -> вправо     код движения: 3     код ASCII: l-100, U-68
    s (S) -> вниз       код движения: 1     код ASCII: l-115, U-83
    a (A) -> влево      код движения: 2     код ASCII: l-97, U-65
*/


int main(void)
{
    srand(time(NULL));
    initscr();
    
    SnakeChain* snake = createNewChain(SCR_WIDTH / 2, SCR_HEIGHT / 2);
    snakeGrow(snake, 1);
    snakeGrow(snake, 2);
    snakeGrow(snake, 2);
    snakeGrow(snake, 2);

    Apple* apple = malloc(sizeof(Apple));
    throwApple(apple, snake);

    draw(snake, apple);
    refresh();

    getch();
    endwin();
    return 0;
}