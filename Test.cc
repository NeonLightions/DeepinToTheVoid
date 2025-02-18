#include <curses.h>
#include <thread>
#include <chrono>

void fadeText(WINDOW* win, int y, int x, const char* text) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_BLACK); // "Mờ dần" bằng cách trùng với nền

    int delay = 300; // Độ trễ giữa các bước

    for (int i = 1; i <= 4; ++i) {
        wattron(win, COLOR_PAIR(i));
        mvwprintw(win, y, x, text);
        wattroff(win, COLOR_PAIR(i));
        wrefresh(win);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main() {
    initscr();
    noecho();
    curs_set(0);

    fadeText(stdscr, 5, 10, "Fading Text Effect!");

    getch();
    endwin();
    return 0;
}
