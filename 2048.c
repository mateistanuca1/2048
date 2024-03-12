#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 4
WINDOW *tile_windows[SIZE][SIZE];
WINDOW *score_board;
int board[SIZE][SIZE];
int n_choices = 3;

// functie care salveaza tabla si scorul intr-un fisier binar
void save_game(const char *filename, int board[SIZE][SIZE], int score) {
    FILE *file = fopen((filename), "wb");

    fwrite(board, sizeof(int), SIZE * SIZE, file);

    fwrite(&score, sizeof(int), 1, file);

    fclose(file);
}

// functie care preia datele din fisierul binar
void read_from_file(const char *filename, int *value) {
    FILE *file = fopen(filename, "rb");

    fread(board, sizeof(int), SIZE * SIZE, file);

    fread(value, sizeof(int), 1, file);

    fclose(file);
}

// functie care printeaza meniul
void print_menu(WINDOW *menu_win, int highlight, int maxy, int maxx) {
    char *choices[] = {
        "Resume",
        "New Game",
        "Exit",
    };
    init_pair(20, COLOR_WHITE, COLOR_BLACK);
    wbkgd(menu_win, COLOR_BLACK);
    wrefresh(menu_win);
    int x, y, i;
    x = maxx / 2 - 5;
    y = maxy / 5;
    mvwprintw(menu_win, y, x, "2048");
    y *= 2;
    wattron(menu_win, COLOR_PAIR(20));
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            wattron(menu_win, A_BOLD);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_BOLD);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        y += maxy / 5;
    }
    wrefresh(menu_win);
}

// functie care sterge o fereastra
void destroy_win(WINDOW *local_win) {
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}

// declararea tablei de joc
void init_board() {
    int row, col, i, j;
    getmaxyx(stdscr, row, col);
    col = col / 2;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            tile_windows[i][j] = newwin(row / SIZE, col / SIZE,
                                        i * (row / SIZE), j * (col / SIZE));
            box(tile_windows[i][j], 0, 0);
            board[i][j] = 0;
        }
    }
}

// declararea tablei de scor
void init_score_board() {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    int row, col;
    getmaxyx(stdscr, row, col);
    score_board = newwin(row, col / 2, 0, col / 2);
    wbkgd(score_board, COLOR_PAIR(1));
    box(score_board, 0, 0);
    wrefresh(score_board);
}

// functie pentru afisarea timpului(se updateaza constant in while ul din main)
void update_time_date() {
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    mvwprintw(score_board, 1, getmaxx(score_board) / 3,
              "TIME: %02d:%02d:%02d  DATE: %02d/%02d/%04d", time_info->tm_hour,
              time_info->tm_min, time_info->tm_sec, time_info->tm_mday,
              time_info->tm_mon + 1, time_info->tm_year + 1900);
    wrefresh(score_board);
}

// afisarea tablei de scor
void print_score_board(int score) {
    int maxy, maxx;
    getmaxyx(score_board, maxy, maxx);
    wclear(score_board);
    box(score_board, 0, 0);
    wattron(score_board, A_BOLD);
    mvwprintw(score_board, maxy / 5, maxx / 2, "SCORE:%d", score);
    mvwprintw(score_board, maxy / 5 * 2, maxx / 2, "LEGEND:");
    wattroff(score_board, A_BOLD);
    mvwprintw(score_board, maxy / 5 * 2 + 1, maxx / 2 - 3, "KEY UP : MOVE UP");
    mvwprintw(score_board, maxy / 5 * 2 + 2, maxx / 2 - 3,
              "KEY DOWN : MOVE DOWN");
    mvwprintw(score_board, maxy / 5 * 2 + 3, maxx / 2 - 3,
              "KEY LEFT : MOVE LEFT");
    mvwprintw(score_board, maxy / 5 * 2 + 4, maxx / 2 - 3,
              "KEY RIGHT : MOVE RIGHT");
    mvwprintw(score_board, maxy / 5 * 2 + 5, maxx / 2 - 3, "Q : OPEN MENU");
    wrefresh(score_board);
}

// functie logaritmica(pentru culori)
int putere(int valoare, int rez) {
    if (valoare == 1) {
        return rez;
    }
    return putere(valoare / 2, ++rez);
}

// colorarea si afisarea fiecarei celule
void print_tile(int row, int col, int value) {
    WINDOW *tile_win = tile_windows[row][col];

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(4, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    init_pair(6, COLOR_BLACK, COLOR_GREEN);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);
    init_pair(8, COLOR_BLACK, COLOR_YELLOW);
    init_pair(9, COLOR_BLACK, COLOR_RED);
    init_pair(10, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(11, COLOR_BLACK, COLOR_CYAN);
    wclear(tile_win);
    box(tile_win, 0, 0);
    int maxy, maxx;
    getmaxyx(tile_win, maxy, maxx);
    if (value != 0) {
        attron(COLOR_PAIR(putere(value, 0)));
        wbkgd(tile_win, COLOR_PAIR(putere(value, 0)));
        wattron(tile_win, A_BOLD);
        mvwprintw(tile_win, maxy / 2, maxx / 2, "%d", value);
        wattroff(tile_win, A_BOLD);
        attroff(COLOR_PAIR(putere(value, 0)));
    } else {
        wbkgd(tile_win, COLOR_BLACK);
    }
    wrefresh(tile_win);
}

// afisarea tablei
void print_board() {
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            print_tile(i, j, board[i][j]);
        }
    }
}

// genereaza o celula oarecare cu valoarea 2/4
void generate_random_tile() {
    int empty_tiles[SIZE * SIZE][2];
    int count = 0, i, j;

    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            if (board[i][j] == 0) {
                empty_tiles[count][0] = i;
                empty_tiles[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        int index = rand() % count;
        int value = (rand() % 2 + 1) * 2;
        board[empty_tiles[index][0]][empty_tiles[index][1]] = value;
    }
}

// unirea a doua celule
void merge_tiles(int i, int j, int k, int l) {
    board[i][j] *= 2;
    board[k][l] = 0;
}

// functiile de miscare(mai intai unesc si dupa le aduc la locul lor)
int move_left(int board[SIZE][SIZE], int *score) {
    int moved = 0, i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board[i][j] != 0) {
                int k = j + 1;
                while (!board[i][k] && k < SIZE) {
                    k++;
                }
                if (board[i][k] == board[i][j] && k != SIZE) {
                    merge_tiles(i, j, i, k);
                    (*score) += board[i][j];
                    moved = 1;
                }
            }
        }
    }
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (!board[i][j]) {
                int k;
                for (k = j; k < SIZE; k++) {
                    if (board[i][k]) {
                        board[i][j] = board[i][k];
                        board[i][k] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }
    }

    return moved;
}

int move_right(int board[SIZE][SIZE], int *score) {
    int moved = 0, i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = SIZE - 1; j >= 0; j--) {
            if (board[i][j] != 0) {
                int k = j - 1;
                while (!board[i][k] && k >= 0) {
                    k--;
                }
                if (board[i][k] == board[i][j] && k >= 0) {
                    merge_tiles(i, j, i, k);
                    (*score) += board[i][j];
                    moved = 1;
                }
            }
        }
    }
    for (i = 0; i < SIZE; i++) {
        for (j = SIZE - 1; j >= 0; j--) {
            if (!board[i][j]) {
                int k;
                for (k = j; k >= 0; k--) {
                    if (board[i][k]) {
                        board[i][j] = board[i][k];
                        board[i][k] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

int move_up(int board[SIZE][SIZE], int *score) {
    int moved = 0, i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board[j][i] != 0) {
                int k = j + 1;
                while (!board[k][i] && k < SIZE) {
                    k++;
                }
                if (board[k][i] == board[j][i] && k != SIZE) {
                    merge_tiles(j, i, k, i);
                    (*score) += board[j][i];
                    moved = 1;
                }
            }
        }
    }
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (!board[j][i]) {
                int k;
                for (k = j; k < SIZE; k++) {
                    if (board[k][i]) {
                        board[j][i] = board[k][i];
                        board[k][i] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

int move_down(int board[SIZE][SIZE], int *score) {
    int moved = 0, i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = SIZE - 1; j >= 0; j--) {
            if (board[j][i] != 0) {
                int k = j - 1;
                while (!board[k][i] && k >= 0) {
                    k--;
                }
                if (board[k][i] == board[j][i] && k >= 0) {
                    merge_tiles(j, i, k, i);
                    (*score) += board[j][i];
                    moved = 1;
                }
            }
        }
    }
    for (i = 0; i < SIZE; i++) {
        for (j = SIZE - 1; j >= 0; j--) {
            if (!board[j][i]) {
                int k;
                for (k = j; k >= 0; k--) {
                    if (board[k][i]) {
                        board[j][i] = board[k][i];
                        board[k][i] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }
    }

    return moved;
}

// verific daca jocul s-a terminat
int is_game_over() {
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            if (board[i][j] == 0 ||
                (j + 1 < SIZE && board[i][j] == board[i][j + 1]) ||
                (i + 1 < SIZE && board[i][j] == board[i + 1][j])) {
                return 0;
            }
        }
    }
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board[i][j] == 2048) {
                return 1;
            }
        }
    }
    return 2;
}

// sterg celulele si inchid salvarea
void cleanup(int score) {
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            delwin(tile_windows[i][j]);
        }
    }
    save_game("saved_game.bin", board, score);
}

// updatarea celulelor
void update_tile(int row, int col) {
    print_tile(row, col, board[row][col]);
    wrefresh(tile_windows[row][col]);
}

void update_all_tiles() {
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            update_tile(i, j);
        }
    }
}

// numar cate celule libere sunt in tabla de joc
int count_empty_cells(int temp_board[SIZE][SIZE]) {
    int count = 0, i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            if (temp_board[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

// simulez miscarile pe o tabla auxiliara
// pentru a gasi cea mai buna miscare
int simulate_move(int direction, int temp_board[SIZE][SIZE], int *score) {
    int moved = 0;

    switch (direction) {
        case 0:
            moved = move_left(temp_board, score);
            break;
        case 1:
            moved = move_right(temp_board, score);
            break;
        case 2:
            moved = move_up(temp_board, score);
            break;
        case 3:
            moved = move_down(temp_board, score);
            break;
    }

    return moved;
}

int find_best_move(int *score) {
    int best_move = -1;
    int max_empty_cells = 0;
    int temp_board[SIZE][SIZE];
    int i, j, direction;

    for (direction = 0; direction < 4; ++direction) {
        for (i = 0; i < SIZE; ++i) {
            for (j = 0; j < SIZE; ++j) {
                temp_board[i][j] = board[i][j];
            }
        }

        int moved = simulate_move(direction, temp_board, score);

        if (moved) {
            int empty_cells = count_empty_cells(temp_board);
            if (empty_cells > max_empty_cells) {
                max_empty_cells = empty_cells;
                best_move = direction;
            }
        }
    }

    return best_move;
}

int main() {
    WINDOW *menu_win;
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(0);
    int row, col, highlight = 1, choice = 0, c, maxx, maxy,
                  resume_menu_opened = 1, score = 0, cont = 0, idle_time = 0;
    getmaxyx(stdscr, row, col);
    getmaxyx(stdscr, maxy, maxx);
    clear();
    noecho();
    curs_set(0);
    cbreak();
    menu_win = newwin(maxy, maxx, 0, 0);
    keypad(menu_win, TRUE);
    refresh();
    init_board();
    init_score_board();
    generate_random_tile();
    if ("saved_game.bin" != NULL)
        read_from_file(
            "saved_game.bin",
            &score);  // recreez tabla si scorul in functie de salvare
    update_all_tiles();
    print_score_board(score);
    timeout(0);
    nodelay(stdscr, TRUE);
    refresh();
    // incep cu meniul deschis
    while (1) {
        if (!resume_menu_opened) {
            int ch = getch();
            if (ch != ERR) {
                idle_time = 0;
                int moved = 0;
                switch (ch) {
                    case KEY_LEFT:
                        moved = move_left(board, &score);
                        break;
                    case KEY_RIGHT:
                        moved = move_right(board, &score);
                        break;
                    case KEY_UP:
                        moved = move_up(board, &score);
                        break;
                    case KEY_DOWN:
                        moved = move_down(board, &score);
                        break;
                    case 'q':
                        resume_menu_opened = 1;
                        break;
                }

                if (moved) {
                    generate_random_tile();
                    update_all_tiles();
                    print_score_board(score);
                    refresh();
                }
            } else {
                // ma bazez pe complexitatea programului pt a face o miscare
                // singur undeva la 10 sec
                idle_time++;
                if (idle_time >= 1500000) {
                    int best_move = find_best_move(&score);

                    if (best_move != -1) {
                        int moved = simulate_move(best_move, board, &score);
                        if (moved) {
                            generate_random_tile();
                            update_all_tiles();
                            print_score_board(score);
                            refresh();
                        }
                    }
                    idle_time = 0;
                }
            }

            switch (is_game_over()) {
                case 0:
                    break;
                case 1:
                    clear();
                    attron(A_BOLD);
                    mvprintw(row / 2, col / 2, "YOU WON!");
                    attroff(A_BOLD);
                    mvprintw(row / 2 + 1, col / 2, "Press Enter to continue");
                    refresh();
                    cont = getch();
                    while (cont != 10) {
                        cont = getch();
                    }
                    resume_menu_opened = 1;
                    break;
                case 2:
                    clear();
                    attron(A_BOLD);
                    mvprintw(row / 2, col / 2, "YOU LOST!");
                    attroff(A_BOLD);
                    mvprintw(row / 2 + 1, col / 2, "Press Enter to contine");
                    refresh();
                    cont = getch();
                    while (cont != 10) {
                        cont = getch();
                    }
                    resume_menu_opened = 1;
                    break;
            }
        } else {
            menu_win = newwin(maxy, maxx, 0, 0);
            keypad(menu_win, TRUE);
            refresh();
            print_menu(menu_win, highlight, maxy, maxx);
            while (1) {
                c = wgetch(menu_win);
                switch (c) {
                    case KEY_UP:
                        if (highlight == 1)
                            highlight = n_choices;
                        else
                            --highlight;
                        break;
                    case KEY_DOWN:
                        if (highlight == n_choices)
                            highlight = 1;
                        else
                            ++highlight;
                        break;
                    case 10:
                        choice = highlight;
                        break;
                }
                print_menu(menu_win, highlight, maxy, maxx);
                if (choice != 0) {
                    break;
                }
            }
            if (choice != 0) {
                resume_menu_opened = 0;
                destroy_win(menu_win);
                if (choice == 1) {
                    clear();
                    update_all_tiles();
                    print_score_board(score);
                }
                if (choice == 2) {
                    clear();
                    score = 0;
                    init_board();
                    generate_random_tile();
                    update_all_tiles();
                    init_score_board();
                    print_score_board(score);
                } else if (choice == 3) {
                    cleanup(score);
                    endwin();
                    break;
                }
            }
            choice = 0;
        }
        update_time_date();
    }
    return 0;
}
