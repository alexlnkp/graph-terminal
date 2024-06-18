#include <stdlib.h>

#define NCURSES_WIDECHAR 1
#include <ncurses.h>
#include <time.h>
#include <math.h>

#include <locale.h>

#define GRAPH_CHAR L"⬣"

#define EMPTY_CHAR L"⸱"

void sleep_ms(int milliseconds) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

void draw_grid(size_t rows, size_t cols) {
    for (size_t x = 0; x < cols; ++x)
        for (size_t y = 0; y < rows; ++y)
            mvaddwstr(y, x, EMPTY_CHAR);
}

void draw_elem(
    size_t x, size_t y,
    size_t* trail_x, size_t* trail_y,
    size_t* trail_index, size_t trail_length
) {
    attron(COLOR_PAIR(2)); mvaddwstr(trail_y[*trail_index], trail_x[*trail_index], EMPTY_CHAR); attroff(COLOR_PAIR(2));

    trail_x[*trail_index] = x; trail_y[*trail_index] = y;
    attron(COLOR_PAIR(1)); mvaddwstr(y, x, GRAPH_CHAR); attroff(COLOR_PAIR(1));

    *trail_index = (*trail_index + 1) % trail_length;
}

void draw_sine(
    size_t cols,
    double freq, double ampl,
    size_t trail_length,
    size_t* trail_index,
    size_t *ctr,
    size_t* trail_x, size_t* trail_y,
    double slow_down_factor
) {
    for (size_t x = cols; x > 0; --x) {
        size_t new_y = sin(x * freq + *ctr / slow_down_factor) * ampl + ampl;
        draw_elem(x, new_y, trail_x, trail_y, trail_index, trail_length);
    }
}

void draw_bounce(
    size_t cols,
    double freq, double ampl,
    size_t trail_length,
    size_t* trail_index,
    size_t *ctr,
    size_t* trail_x, size_t* trail_y,
    double slow_down_factor
) {
    for (size_t x = cols; x > 0; --x) {
        size_t new_y = sin(pow(x, cos(x)) * freq + *ctr / slow_down_factor) * ampl + ampl;
        draw_elem(x, new_y, trail_x, trail_y, trail_index, trail_length);
    }
}



int main(void) {
    setlocale(LC_ALL, "");

    size_t rows = 0;
    size_t cols = 0;

    initscr(); start_color(); use_default_colors();
    init_pair(1, COLOR_CYAN, -1); init_pair(2, COLOR_WHITE, -1);
    getmaxyx(stdscr,rows,cols);

    const double slow_down_factor = 16.0;

    double freq = 1.0 / (cols >> 3);
    double ampl = rows >> 1;

    curs_set(0); clear();
    size_t ctr = 0;
    size_t trail_index = 0;

    const size_t trail_length = cols - 2;
    size_t* trail_x = calloc(trail_length, sizeof(size_t));
    size_t* trail_y = calloc(trail_length, sizeof(size_t));

    draw_grid(rows, cols);
    while (1) {
        draw_sine(cols, freq, ampl, trail_length, &trail_index, &ctr, trail_x, trail_y, slow_down_factor);
        ctr++;
        refresh(); sleep_ms(24);
    }

    free(trail_x); free(trail_y);
    getch(); endwin(); curs_set(1);
    return 0;
}
