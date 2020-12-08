#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


int string_length(const wchar_t* str)
{
    const wchar_t* str_temp = str;
    int len = 0;

    while (*str_temp++ != L'\0')
    {
        len++;
    }
    return len;
}


void wstring_copy(wchar_t* source, const wchar_t* str)
{
    for (int i = 0; i < string_length(str) + 1; i++)
    {
        source[i] = str[i];
    }
}

struct coord
{
    int x, y;
};


struct vector
{
    size_t size;
    size_t total_size;
    struct coord* arr;
};

struct LCGrand
{
    unsigned long long seed;

    unsigned long long a;
    unsigned long long m;

    unsigned long long q;
    unsigned long long r;

    unsigned long long t;
};

struct LCGrand get_seed(long long time)
{
    struct LCGrand rand;

    rand.seed = 2147483645;
    rand.a = 16807;
    rand.m = 2147483647;
    rand.q = 127773;
    rand.r = 2836;
    rand.t = time % 1000;

    return rand;
}

unsigned long get_random(struct LCGrand* rand, int max)
{
    unsigned long long k = 0;

    k = rand->seed / rand->q;
    rand->seed = rand->a * (rand->seed - k * rand->q) - rand->r * k;

    if (rand->seed < 0)
    {
        rand->seed += rand->m;
    }

    return (rand->seed / (rand->t + 1)) % max;
}

void swap(struct coord* a, struct coord* b)
{
    struct coord temp;
    temp.x = b->x;
    temp.y = b->y;
    b->x = a->x;
    b->y = a->y;
    a->x = temp.x;
    a->y = temp.y;
}

void vector_initialize(struct vector* vect)
{
    vect->total_size = 10;
    vect->arr = (struct coord*) malloc(vect->total_size * 8);
    vect->size = 0;
}

void vector_push(struct vector* vect, struct coord point)
{
    struct coord* arr_temp;
    if (vect->size >= vect->total_size - 1)
    {
        vect->total_size *= 2;
        arr_temp = (struct coord*) realloc(vect->arr, vect->total_size * 8);
        if (arr_temp)
        {
            vect->arr = arr_temp;
        }
        else
        {
            free(vect->arr);
        }
    }

    vect->arr[vect->size].x = point.x;
    vect->arr[vect->size].y = point.y;
    vect->size++;
}

void vector_reserve(struct vector* vect, size_t size)
{
    struct coord* arr_temp = (struct coord*) realloc(vect->arr, size * 8);

    if (arr_temp)
    {
        vect->arr = arr_temp;
        vect->total_size = size;
    }
    else
    {
        free(vect->arr);
    }
}

struct coord* vector_front(struct vector* vect)
{
    return &vect->arr[0];
}

enum Directions
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

int main()
{
        HANDLE console = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(console);
        FILE* file_handler;
        file_handler = fopen("./resources/highscore.txt", "r");
        SetConsoleTitle(TEXT("Snake Game"));
        CONSOLE_FONT_INFOEX cfi = {sizeof(cfi)};
        cfi.dwFontSize.X = 12;
        cfi.dwFontSize.Y = 14;
        //wcscpy_s();
        wstring_copy(cfi.FaceName, L"Terminal");
        SetCurrentConsoleFontEx(console, FALSE, &cfi);
        // SetConsoleDisplayMode(console, CONSOLE_FULLSCREEN_MODE, 0);
        ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
        HANDLE handle_input;
        DWORD prev_mode;
        handle_input = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(handle_input, &prev_mode);
        SetConsoleMode(handle_input, prev_mode & ENABLE_EXTENDED_FLAGS);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(console, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(console, &cursorInfo);
        DWORD dwBytesWritten = 0;
        bool esc_key = false;
        int high_score;
        COORD window_size = GetLargestConsoleWindowSize(console);
        SHORT WIDTH = window_size.X;
        SHORT HEIGHT = window_size.Y;
        char* screen = (char*) calloc(HEIGHT * WIDTH, sizeof(char));
        fscanf(file_handler, "%d", &high_score);
        fclose(file_handler);
        file_handler = fopen("./resources/highscore.txt", "w");

        for (int i = 1; i < HEIGHT - 1; i++)
        {
            for (int j = 1; j < WIDTH - 1; j++)
            {
                screen[i * WIDTH + j] = ' ';
            }
        }

        for (int i = 0; i < WIDTH; i++)
        {
            screen[i] = '#';
            screen[(HEIGHT - 1) * WIDTH + i] = '#';
        }

        for (int i = 1; i < HEIGHT - 1; i++)
        {
            screen[i * WIDTH] = '#';
            screen[i * WIDTH + (WIDTH - 1)] = '#';
        }

        while (!(0x8000 & GetKeyState(0x20)))
        {
            esc_key = (0x8000 & GetKeyState(0x1B));

            if (esc_key)
            {
                sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 44) / 2], "                                           ");
                sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 8) / 2], "GOODBYE");
                sprintf(&screen[(HEIGHT / 2 + 2) * WIDTH + (WIDTH - 30) / 2], "                             ");
                WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);
                SetConsoleMode(handle_input, prev_mode);
                fprintf(file_handler, "%d", high_score);
                fclose(file_handler);
                free(screen);
                clock_t start_time = clock();
                while (clock() < start_time + 1000);
                exit(0);
            }

            sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 44) / 2], "WELCOME TO SNAKE GAME. MADE BY VASILE IGNAT");
            sprintf(&screen[(HEIGHT / 2 + 2) * WIDTH + (WIDTH - 30) / 2], "PRESS SPACE TO START THE GAME");
            WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);
        }

        while (true)
        {
            for (int i = 1; i < HEIGHT - 1; i++)
            {
                for (int j = 1; j < WIDTH - 1; j++)
                {
                    screen[i * WIDTH + j] = ' ';
                }
            }

            int score = 0;
            int missed_food = 0;
            int time_left = 1, time_count = 1;
            bool isDead = false, isHungry = false;
            int snake_direction = RIGHT;
            int previous_direction = RIGHT;
            int temp_direction = RIGHT;
            bool right_key = false, left_key = false, up_key = false, down_key = false, num1_key = false, num2_key = false, num3_key = false,
                 num4_key = false, pause_key = false;
            int vertical_speed = 60, horizontal_speed = 60;
            struct vector snake;
            struct LCGrand random = get_seed(time(NULL));
            struct coord food = { 0, 0 };
            while (screen[food.y * WIDTH + food.x] != ' ')
            {
                food.x = get_random(&random, WIDTH - 2) + 1;
                food.y = get_random(&random, HEIGHT - 2) + 1;
            }
            vector_initialize(&snake);
            vector_push(&snake, { 60, 15 });

            while (!isDead)
            {
                num1_key = (0x8000 & GetKeyState(0x61));
                num2_key = (0x8000 & GetKeyState(0x62));
                num3_key = (0x8000 & GetKeyState(0x63));
                num4_key = (0x8000 & GetKeyState(0x64));
                right_key = (0x8000 & GetKeyState(0x44));
                left_key = (0x8000 & GetKeyState(0x41));
                up_key = (0x8000 & GetKeyState(0x57));
                down_key = (0x8000 & GetKeyState(0x53));
                esc_key = (0x8000 & GetKeyState(0x1B));
                pause_key = (1 & GetKeyState(0x50));

                if (esc_key)
                {
                    sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 8) / 2], "GOODBYE");
                    WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);
                    fprintf(file_handler, "%d", high_score);
                    SetConsoleMode(handle_input, prev_mode);
                    fclose(file_handler);
                    free(screen);
                    free(snake.arr);
                    clock_t start_time = clock();
                    while (clock() < start_time + 1000);
                    exit(0);
                }

                if (pause_key)
                {
                    while (!(0x8000 & GetKeyState(0x50)))
                    {
                        esc_key = (0x8000 & GetKeyState(0x1B));

                        if (esc_key)
                        {
                            sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 8) / 2], "GOODBYE");
                            WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);
                            fprintf(file_handler, "%d", high_score);
                            SetConsoleMode(handle_input, prev_mode);
                            fclose(file_handler);
                            free(screen);
                            free(snake.arr);
                            clock_t start_time = clock();
                            while (clock() < start_time + 1000);
                            exit(0);
                        }
                    }
                }

                if (num1_key)
                {
                    vertical_speed = 90;
                    horizontal_speed = 90;
                }
                else if (num2_key)
                {
                    vertical_speed = 60;
                    horizontal_speed = 60;
                }
                else if (num3_key)
                {
                    vertical_speed = 30;
                    horizontal_speed = 30;
                }
                else if (num4_key)
                {
                    vertical_speed = 10;
                    horizontal_speed = 10;
                }

                if (up_key)
                {
                    if (previous_direction != DOWN)
                    {
                        snake_direction = UP;
                        previous_direction = UP;
                    }
                }
                else if (right_key)
                {
                    if (previous_direction != LEFT)
                    {
                        snake_direction = RIGHT;
                        previous_direction = RIGHT;
                    }
                }
                else if (down_key)
                {
                    if (previous_direction != UP)
                    {
                        snake_direction = DOWN;
                        previous_direction = DOWN;
                    }
                }
                else if (left_key)
                {
                    if (previous_direction != RIGHT)
                    {
                        snake_direction = LEFT;
                        previous_direction = LEFT;
                    }
                }

                struct coord* snake_segment = vector_front(&snake);
                struct coord temp_snake;

                switch (snake_direction)
                {
                    case UP:
                        temp_snake.x = snake_segment->x;
                        temp_snake.y = snake_segment->y;

                        for (int i = 1; i < snake.size; i++)
                        {
                            swap(&temp_snake, &snake.arr[i]);
                        }

                        snake_segment->y -= 1;

                        if (snake_segment->y == 0)
                        {
                            snake_segment->y = HEIGHT - 2;
                        }
                        break;
                    case RIGHT:
                        temp_snake.x = snake_segment->x;
                        temp_snake.y = snake_segment->y;

                        for (int i = 1; i < snake.size; i++)
                        {
                            swap(&temp_snake, &snake.arr[i]);
                        }

                        snake_segment->x += 1;

                        if (snake_segment->x == WIDTH - 1)
                        {
                            snake_segment->x = 1;
                        }
                        break;
                    case DOWN:
                        temp_snake.x = snake_segment->x;
                        temp_snake.y = snake_segment->y;

                        for (int i = 1; i < snake.size; i++)
                        {
                            swap(&temp_snake, &snake.arr[i]);
                        }

                        snake_segment->y += 1;

                        if (snake_segment->y == HEIGHT - 1)
                        {
                            snake_segment->y = 1;
                        }
                        break;
                    case LEFT:
                        temp_snake.x = snake_segment->x;
                        temp_snake.y = snake_segment->y;

                        for (int i = 1; i < snake.size; i++)
                        {
                            swap(&temp_snake, &snake.arr[i]);
                        }

                        snake_segment->x -= 1;

                        if (snake_segment->x == 0)
                        {
                            snake_segment->x = WIDTH - 2;
                        }
                }

                screen[food.y * WIDTH + food.x] = '@';

                screen[temp_snake.y * WIDTH + temp_snake.x] = ' ';

                for (int i = 1; i < snake.size; i++)
                {
                    if ((snake_segment->x == snake.arr[i].x) && (snake_segment->y == snake.arr[i].y))
                    {
                        isDead = true;
                        // PlaySound(TEXT("./resources/dead.wav"), NULL, SND_ASYNC);

                        if (score > high_score)
                        {
                            high_score = score;
                        }
                        break;
                    }
                }

                if (((food.x - snake_segment->x >= -3) && (food.x - snake_segment->x <= 3)) && (food.y - snake_segment->y == 0)
                    && ((snake_direction == UP) || (snake_direction == DOWN)))
                {
                        missed_food++;
                        if (missed_food == 10)
                        {
                            time_left = (food.x > snake_segment->x) ? food.x - snake_segment->x : snake_segment->x - food.x;
                            temp_direction = snake_direction;
                        }
                }
                else if (((food.y - snake_segment->y >= -3) && (food.y - snake_segment->y <= 3)) && (food.x - snake_segment->x == 0)
                    && ((snake_direction == LEFT) || (snake_direction == RIGHT)))
                {
                        missed_food++;
                        if (missed_food == 10)
                        {
                            time_left = (food.y > snake_segment->y) ? food.y - snake_segment->y : snake_segment->y - food.y;
                            temp_direction = snake_direction;
                        }
                }
                else if (((food.x - snake_segment->x >= -1) && (food.x - snake_segment->x <= 1)) && (food.y - snake_segment->y == 0))
                {
                    missed_food++;
                }
                else if (((food.x - snake_segment->x >= -1) && (food.x - snake_segment->x <= 1)) && (food.y - snake_segment->y == 0))
                {
                    missed_food++;
                }

                if ((snake_segment->x == food.x) && (snake_segment->y == food.y))
                {
                    // PlaySound(TEXT("./resources/ra.wav"), NULL, SND_ASYNC);
                    int x, y;

                    if (score != 0)
                    {
                        x = snake.arr[snake.size - 1].x - snake.arr[snake.size - 2].x;
                        y = snake.arr[snake.size - 1].y - snake.arr[snake.size - 2].y;
                    }
                    else
                    {
                        switch (snake_direction)
                        {
                        case UP:
                            x = 0;
                            y = -1;
                            break;
                        case RIGHT:
                            x = 1;
                            y = 0;
                            break;
                        case DOWN:
                            x = 0;
                            y = 1;
                            break;
                        case LEFT:
                            x = -1;
                            y = 0;
                        }
                    }
                    struct coord new_segment = { snake.arr[snake.size - 1].x - x, snake.arr[snake.size - 1].y - y };
                    vector_push(&snake, new_segment);
                    snake_segment = vector_front(&snake);
                    while (screen[food.y * WIDTH + food.x] != L' ')
                    {
                        food.x = get_random(&random, WIDTH - 2) + 1;
                        food.y = get_random(&random, HEIGHT - 2) + 1;
                    }
                    screen[food.y * WIDTH + food.x] = L' ';
                    time_count = 1;
                    time_left = 1;
                    missed_food = 0;
                    isHungry = false;
                    score++;
                }

                if (isHungry)
                {
                    if ((time_count == time_left) || (temp_direction == snake_direction))
                    {
                        isDead = true;
                        // PlaySound(TEXT("./resources/dead.wav"), NULL, SND_ASYNC);
                    }

                    time_count++;

                    if (score > high_score)
                    {
                        high_score = score;
                    }
                }

                if (missed_food == 10)
                {
                    isHungry = true;
                }

                for (int i = 1; i < snake.size; i++)
                {
                    screen[snake.arr[i].y * WIDTH + snake.arr[i].x] = isDead ? 'x' : '0';
                }

                screen[snake_segment->y * WIDTH + snake_segment->x] = isDead ? 'X' : 'O';

                WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);

                clock_t start_time = clock();
                while (clock() < start_time + ((snake_direction % 2 == 0) ? vertical_speed : horizontal_speed));
            }

            sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 40) / 2], "YOUR SCORE IS %d. YOUR HIGH SCORE IS %d", score, high_score);
            sprintf(&screen[(HEIGHT / 2 + 2) * WIDTH + (WIDTH - 26) / 2], "PRESS SPACE TO PLAY AGAIN");
            WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);

            while (!(0x8000 & GetKeyState(0x20))) {
                esc_key = (0x8000 & GetKeyState(0x1B));

                if (esc_key)
                {
                    sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 40) / 2], "                                       ");
                    sprintf(&screen[(HEIGHT / 2 + 2) * WIDTH + (WIDTH - 26) / 2], "                         ");
                    sprintf(&screen[(HEIGHT / 2) * WIDTH + (WIDTH - 8) / 2], "GOODBYE");
                    WriteConsoleOutputCharacter(console, screen, WIDTH * HEIGHT, { 0, 0 }, &dwBytesWritten);
                    fprintf(file_handler, "%d", high_score);
                    SetConsoleMode(handle_input, prev_mode);
                    fclose(file_handler);
                    free(screen);
                    free(snake.arr);
                    clock_t start_time = clock();
                    while (clock() < start_time + 1000);
                    exit(0);
                }
            }
            // PlaySound(NULL, 0, 0);
            // PlaySound(TEXT("./resources/again.wav"), NULL, SND_ASYNC);
        }
	return 0;
}
