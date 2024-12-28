#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define COLS 80
#define ROWS 25

void creating_field(char arr[ROWS][COLS]);
void print_field(char arr[ROWS][COLS], long* current_speed);
void initialize_array_with_spaces(char arr[ROWS][COLS]);
void print_top_border();
void print_bottom_border();
int get_X(int i);
int get_Y(int i);
void copy_state(char arr[ROWS][COLS], char prevArr[ROWS][COLS]);
void new_step_generation(char arr[ROWS][COLS]);
void game_show(char arr[ROWS][COLS]);
void random_generated_field(char arr[ROWS][COLS]);
int interface();
int run_app(char arr[ROWS][COLS]);
void speed_change_field(long* current_speed);
void speed(long* current_speed);

int main() {
    char arr[ROWS][COLS];
    if (run_app(arr) != 0) {
        printw("Error");
    }
    // interface();

    endwin();
    return 0;
}

int run_app(char arr[ROWS][COLS]) {
    initialize_array_with_spaces(arr);
    int err = 0;
    int choice = interface();

    if (choice == 2) {
        printf("\nTo end the enter tap \"q\"\n");
        // for(;;) {
        creating_field(arr);
        // }
        //смена с ввода файла в терминальный ввод
        if (!freopen("/dev/tty", "r", stdin)) {
            perror("/dev/tty");
            return 1;
        }

        initscr();
        noecho();
        timeout(0);
        game_show(arr);
        // print_field(arr);
        refresh();
        getch();
    } else if (choice == 1) {
        initscr();
        noecho();
        timeout(0);
        random_generated_field(arr);
        game_show(arr);
        refresh();
        getch();
    }
    return err;
}

int interface() {
    int choice;
    system("clear");
    printf("Choose generation: \n");
    printf("1 - random field\n");
    printf("2 - enter x y from keyboard\n");
    printf("Your choice: ");
    scanf("%d", &choice);
    for (;;) {
        if (choice == 1 || choice == 2) {
            break;
        } else {
            printf("\nNot available input, choose from 1, 2: ");
            scanf("%d", &choice);
        }
    }
    return choice;
}

void game_show(char arr[ROWS][COLS]) {
    long current_speed = 500000;

    for (;;) {
        char key;
        print_field(arr, &current_speed);
        new_step_generation(arr);
        key = getch();
        if (key == 'z') {
            current_speed -= 100000;
        }
        if (key == 'x') {
            current_speed += 100000;
        }
        if (key == 'q') {
            break;
        }
        usleep(current_speed);
        clear();
    }
}

//создание поля
void creating_field(char arr[ROWS][COLS]) {
    int row, col;
    while (scanf("%d %d", &row, &col) == 2) {
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            arr[row][col] = '*';
        }
    }
}
//вывод поля
void print_field(char arr[ROWS][COLS], long* current_speed) {
    print_top_border();
    for (int i = 0; i < ROWS; i++) {
        printw("|");  // левая граница
        for (int j = 0; j < COLS; j++) {
            printw("%c", arr[i][j]);
        }
        printw("|\n");  // правая граница
    }
    print_bottom_border();
    printw("\n");
    speed_change_field(current_speed);
}

void speed_change_field(long* current_speed) {
    printw("Current speed: %ld | \"z\" - increase | \"x\" - decrease | \"q\" to exit\n", *current_speed);
}
//инициализация пробелами
void initialize_array_with_spaces(char arr[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            arr[i][j] = ' ';
        }
    }
}
//вывод границы
void print_top_border() {
    printw("+");  // левый верхний угол
    for (int i = 0; i < COLS; i++) {
        printw("-");  // верхняя граница
    }
    printw("+\n");  // правый верхний угол
}

void print_bottom_border() {
    printw("+");  // левый нижний угол
    for (int i = 0; i < COLS; i++) {
        printw("-");  // нижняя  граница
    }
    printw("+\n");  // правый нижний угол
}

void copy_state(char arr[ROWS][COLS], char prevArr[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            prevArr[i][j] = arr[i][j];
        }
    }
}

int get_X(int i) { return (ROWS + i) % ROWS; }

int get_Y(int j) { return (COLS + j) % COLS; }

void new_step_generation(char arr[ROWS][COLS]) {
    //запоминаем состояние предыдущее
    char prevArr[ROWS][COLS];
    initialize_array_with_spaces(prevArr);
    copy_state(arr, prevArr);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int count_neighbours = 0;
            //поиск соседей
            if (prevArr[get_X(i - 1)][get_Y(j - 1)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i - 1)][get_Y(j)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i - 1)][get_Y(j + 1)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i)][get_Y(j - 1)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i)][get_Y(j + 1)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i + 1)][get_Y(j - 1)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i + 1)][get_Y(j)] == '*') {
                count_neighbours++;
            }
            if (prevArr[get_X(i + 1)][get_Y(j + 1)] == '*') {
                count_neighbours++;
            }

            //условия игры
            //если клетка мертвая и у нее было три соседа, то клетка оживает
            if ((prevArr[i][j] == ' ') && (count_neighbours == 3)) {
                arr[i][j] = '*';
                // если клетка живая и у нее 2 или 3 соседа, то она умирает
            } else if ((count_neighbours < 2) || (count_neighbours > 3)) {
                arr[i][j] = ' ';
            }
        }
    }
}

void random_generated_field(char arr[ROWS][COLS]) {
    srand(time(NULL));
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (rand() % 10 == 0) {
                arr[i][j] = '*';
            }
        }
    }
}
