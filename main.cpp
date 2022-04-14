#include <iostream>
#include <ncurses.h>
#include <Print.h>
#include <Editor.h>

#include <signal.h>

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

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_color(COLOR_YELLOW, 900, 680, 80);
    init_pair(2, COLOR_YELLOW, -1);
}

int main(int argc, char* argv[]) {
    std::string filename;
    if(argc != 2) {
        filename = "test";
    } else {
        filename = std::string(argv[1]);
    }

    editor = new Editor(filename);

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

    editor->save();

    free(editor);

    refresh();
    endwin();

    return 0;
}
