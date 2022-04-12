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
#include <iomanip>
#include <cmath>
#include <bitset>

class Editor {
private:
    Document document;
    Command command;
    bool open = true;

    int scroll = 0;
    int gutterSize = 0;

    enum editMode {EDIT=0, COMMAND=1};
    editMode mode = COMMAND;
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
        int screenHeight = getmaxy(stdscr);
        std::string statusMessage;
        if(mode == COMMAND) {
            statusMessage += "Command: ";
            statusMessage += command.getCommandChain();
        }

        // command bar
        if(mode == COMMAND) attron(COLOR_PAIR(1));
        mvprintw(screenHeight - 1, 0, (statusMessage + getStatus()).c_str());
        clrtoeol();
        if(mode == COMMAND) attroff(COLOR_PAIR(1));

        // line stats
        auto[line, chara] = document.caretPos();
        std::string lineStats = std::to_string(line) + ":" + std::to_string(chara);
        int screenWidth = getmaxx(stdscr);
        mvprintw(screenHeight - 1, screenWidth - (int)lineStats.size() - 3, lineStats.c_str());
    }

    static std::string padLeft(std::string s, int width) {
        return s.insert(0, width - s.length(), ' ');
    }

    void printView() {
        int screenHeight = getmaxy(stdscr) - 1; // save a line for status bar
        auto &lines = document.getLines();

        int screenLine = 0, documentLine = int(scroll);
        for(; documentLine < lines.size() && screenLine < screenHeight;
              documentLine++, screenLine++) {

            attron(COLOR_PAIR(2));
            std::string row = padLeft(std::to_string(documentLine), (int)ceil(std::log10(lines.size())));
            row += " ";
            mvprintw(screenLine, 0, row.c_str());
            attroff(COLOR_PAIR(2));

            gutterSize = (int)row.size();
            mvprintw(screenLine, gutterSize, lines.at(documentLine).c_str());

            clrtoeol();
        }

        for(; screenLine < screenHeight; screenLine++) {
            move(screenLine, 0);
            clrtoeol();
        }

    }

    void setScroll() {
        auto caretPos = document.caretPos();
        int screenHeight = getmaxy(stdscr) - 1; // save a line for status bar

        // try to scroll
        int MARGIN = 3;
        int smallerMargin = MARGIN;
        int largerMargin = screenHeight - smallerMargin;

        int caretScreenLine = caretPos.line - scroll;
        if (caretScreenLine <= smallerMargin)
            scrollBy(caretScreenLine - smallerMargin);

        if (caretScreenLine > largerMargin)
            scrollBy(caretScreenLine - largerMargin);
    }
    void scrollBy(int delta) {
        int screenHeight = getmaxy(stdscr) - 1; // save a line for status bar

        scroll += delta;

        int minScroll = 0;
        int maxScroll = int(document.getLines().size()) - screenHeight;
        if(maxScroll < 0) maxScroll = 0;

        if (scroll < minScroll) scroll = minScroll;

        if (scroll > maxScroll) scroll = maxScroll;
    }

    void setCaret() {
        auto caretPos = document.caretPos();
        move(caretPos.line - scroll, gutterSize + caretPos.chara);

    }
    void eatInput(int key) {
        if(key == 27) { //ESC
            mode = static_cast<enum editMode>((mode + 1) % 2);
            command.clearCommands();
            return;
        }

        MEVENT event;
        if(key == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(event.bstate & BUTTON5_PRESSED) {
                    document.moveCaretDown();
                }if(event.bstate & BUTTON4_PRESSED) {
                    document.moveCaretUp();
                }
            }
        }

        if(mode == EDIT) {
            command.editModeCommand(key);
        }else if(mode == COMMAND) {
            if(key == 'q' || key == 'Q') {
                open = false;
                return;
            }
            if(command.commandModeCommand(key))
                mode = EDIT;
        }
    }

    [[nodiscard]] bool isOpen() const {
        return open;
    }
};

#endif //MINIMA_EDITOR_H
