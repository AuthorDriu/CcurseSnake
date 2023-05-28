/*
    Игра в змейку на curses

    НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ НЕ ИСПОЛЬЗУЙ РЕКУРСИЮ
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
#include <unistd.h>

#define SCR_WIDTH       20
#define SCR_HEIGHT      20

#define UPDATE_DELEY    1

#define WALL_CHAR       '#'
#define WALL_STR        "#"

#define SNAKE_BODY      "$"  

#define APPLE           "O"


#define PRINT_HELP 1

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

// Устанавливает значение coords равные coords головы змейки
int getHead(SnakeChain* snake, Coords* coords)
{
    if (snake->next == NULL)
    {
        coords->x = snake->coords.x;
        coords->y = snake->coords.y;
    }
    else
    {
        getHead(snake->next, coords);
    }

    return 0;
}

// Освобождает память, которая была выделена для змейки
int snakeFree(SnakeChain* snake)
{
    if (snake->next == NULL)
    {
        free(snake);
        
        return 0;
    }
    else
    {
        snakeFree(snake->next);
        free(snake);

        return 0;
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

int ate(Apple* apple, SnakeChain* snake)
{
    if (snake->next == NULL)
    {
        if ((snake->coords.x == apple->coords.x) && (snake->coords.y == apple->coords.y))
        {
            return 1;
        }
        return 0;
    }
    else
    {
        return ate(apple, snake->next);
    }
}

/*
    ОТРИСОВКА
*/

// Вырисовывает игровое поле
int drawField()
{
    char wall[SCR_WIDTH + 1];       
    wall[SCR_WIDTH] = '\0';
    for (int x = 0; x < SCR_WIDTH; ++x)
    {
        wall[x] = WALL_CHAR;
    }

    for (int y = 0; y < SCR_HEIGHT + 1; ++y)
    {
        if ((y == 0) || (y == SCR_HEIGHT))
        {
            printw(wall);
        }
        else
        {
            printw(WALL_STR);
            move(y, SCR_WIDTH);
            printw(WALL_STR);
        }
        move(y, 0);
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

#ifdef PRINT_HELP
    move(1, SCR_WIDTH + 5);
    printw("HELP:");
    move(2, SCR_WIDTH + 10);
    printw("[ W ] - up");
    move(3, SCR_WIDTH + 10);
    printw("[ D ] - right");
    move(4, SCR_WIDTH + 10);
    printw("[ S ] - down");
    move(5, SCR_WIDTH + 10);
    printw("[ A ] - left");
    move(7, SCR_WIDTH + 10);
    printw("[ E ] - exit");
#endif

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


short* TO_PLAY;
char* kbPressedKey;
short* movementDirection;

void* inputHandle(void* threadData)
{
    nodelay(stdscr, 1);
    while (*TO_PLAY)
    {
        *kbPressedKey = getch();

        if ((*kbPressedKey == 'w') || (*kbPressedKey == 'W'))
        {
            *movementDirection = 0;
        }
        else if ((*kbPressedKey == 's') || (*kbPressedKey == 'S'))
        {
            *movementDirection = 1;
        }
        else if ((*kbPressedKey == 'a') || (*kbPressedKey == 'A'))
        {
            *movementDirection = 2;
        }
        else if ((*kbPressedKey == 'd') || (*kbPressedKey == 'D'))
        {
            *movementDirection = 3;
        }
        else if ((*kbPressedKey == 'e') || (*kbPressedKey == 'E'))
        {
            *TO_PLAY = 0;
        }
    }
    pthread_exit(0);
}

int main(void)
{
    srand(time(NULL));
    initscr();
    noecho();
    
    SnakeChain* snake = createNewChain(SCR_WIDTH / 2, SCR_HEIGHT / 2);
    Apple* apple = malloc(sizeof(Apple));

    throwApple(apple, snake);

    movementDirection = malloc(2);
    *movementDirection = -1;
    TO_PLAY = malloc(2);
    kbPressedKey = malloc(1);

    *TO_PLAY = 1;

    void* thread_data = NULL;
	pthread_t thread;
 
	pthread_create(&thread, NULL, inputHandle, thread_data);

    while (*TO_PLAY)
    {
        if (ate(apple, snake))
        {
            snakeGrow(snake, *movementDirection);
            throwApple(apple, snake);
        }
        else
        {
            snakeMove(snake, *movementDirection);
        }

        clear();
        refresh();
        usleep(200);
        draw(snake, apple);
        usleep(200000);        
    }

    endwin();

    pthread_join(thread, NULL);

    snakeFree(snake);
    free(movementDirection);
    free(apple);
    free(TO_PLAY);
    free(kbPressedKey);

    return 0;
}