#include <iostream>
#include <ncurses.h>
#include <Print.h>
#include <Editor.h>


// Initializes the curses.h
void curses_init()
{
    initscr();
    raw();
    nonl();
    set_escdelay(50);
    keypad(stdscr, true);

    if(has_colors()) {
        start_color();
        use_default_colors();
    }

//    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLUE, COLOR_WHITE);
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        println("Nothing to open");
        return 0;
    }

    std::string filename = std::string(argv[1]);
    Editor editor(filename);

    curses_init();

    while(editor.isOpen()) {
        editor.printStatusLine();
        editor.printView();
        editor.eatInput(getch());
    }

    refresh();
    endwin();

    return 0;
}
