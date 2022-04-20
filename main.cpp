#include <iostream>
#include <ncurses.h>
#include "Print.h"
#include "Editor.h"

Editor *editor;

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
    //BUTTON2_PRESSED | BUTTON3_PRESSED
    mousemask(ALL_MOUSE_EVENTS, nullptr);

    init_pair(1, COLOR_WHITE, COLOR_BLUE); // command bar colors

    init_color(COLOR_YELLOW, 580, 500, 450);
    init_pair(2, COLOR_YELLOW, -1); // line number colors

    init_color(COLOR_RED, 1000, 700, 0);
    init_color(COLOR_MAGENTA, 300, 210, 0);
    init_pair(3, COLOR_RED, COLOR_MAGENTA); // line number colors
}

int main(int argc, char* argv[]) {
    std::string filename;
    if(argc != 2) {
        return 1;
    } else {
        filename = std::string(argv[1]);
    }

    editor = new Editor(filename);
    if(!editor->isOpen()) {
        println("File can't be opened\n");
        return 1;
    }

    curses_init();

    editor->printStatusLine();
    editor->printView();
    editor->setCaret();
    
    while(editor->isOpen()) {
        editor->eatInput(getch());
        editor->updateSelection();
        editor->setScroll();
        editor->printStatusLine();
        editor->printView();
        editor->setCaret();
    }

    free(editor);

    refresh();
    endwin();

    return 0;
}
