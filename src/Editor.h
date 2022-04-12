//
// Created by reschivon on 4/11/22.
//

#ifndef MINIMA_EDITOR_H
#define MINIMA_EDITOR_H

#include "Document.h"
#include "Commands.h"

#include <fstream>
#include <iostream>
#include <sstream>

class Editor {
private:
    Document document;
    Command command;
    bool open = true;

    uint scroll = 0;

    enum editMode {EDIT=0, COMMAND=1};
    editMode mode = EDIT;
public:
    explicit Editor(const std::string& filename) : command(document){

        std::ifstream infile(filename.c_str());
        if(infile.is_open()) {
            // read the file to document
            while (!infile.eof()) {
                std::string line;
                getline(infile, line);
                document.addLine(line);
            }
        } else {
            open = false;
            println("File can't be opened");
        }
        infile.close();
    }

    void printStatusLine() {
        std::string statusMessage;
        if(mode == COMMAND) {
            statusMessage += "Command: ";
            statusMessage += command.getCommandChain();
        }

        attron(A_REVERSE);
        if(mode == COMMAND)
            attron(COLOR_PAIR(1));
        mvprintw(LINES-1, 0, statusMessage.c_str());
        clrtoeol();
        if(mode == COMMAND)
            attroff(COLOR_PAIR(1));
        //attron(COLOR_PAIR(0));
        attroff(A_REVERSE);
    }

    void printView() {
        uint screenHeight = getmaxy(stdscr) - 1; // save a line for status bar
        auto &lines = document.getLines();

        int screenLine = 0, documentLine = int(scroll);
        for(; documentLine < lines.size() && screenLine < screenHeight;
              documentLine++, screenLine++) {
            mvprintw(screenLine, 0, lines.at(documentLine).c_str());
            clrtoeol();
        }

        for(; screenLine < screenHeight; screenLine++) {
            move(screenLine, 0);
            clrtoeol();
        }

        auto caretPos = document.getCaretPos();

        // try to scroll
        int MARGIN = 3;
        int caretScreenLine = caretPos.line - scroll;
        if (caretScreenLine < MARGIN) {
            int toScroll = MARGIN - caretScreenLine;
            toScroll = std::min(int(scroll), toScroll);
            scroll -= toScroll;
        }
        if(caretScreenLine > screenHeight - MARGIN) {
            int toScroll = caretScreenLine - (screenHeight - MARGIN);
            int scrollLimit = document.getLines().size() - screenHeight;
            toScroll = std::min(scrollLimit - scroll, toScroll);
            scroll += toScroll;
        }
        move(int(caretPos.line) - scroll, int(caretPos.chara));
    }

    void eatInput(int key) {
        if(key == 27) { //ESC
            mode = static_cast<enum editMode>((mode + 1) % 2);
            command.clearCommands();
            return;
        }
        if(mode == EDIT) {
            command.editModeCommand(key);
        }else if(mode == COMMAND) {
            command.commandModeCommand(key);
        }
    }
    bool isOpen() const {
        return open;
    }
};

#endif //MINIMA_EDITOR_H
