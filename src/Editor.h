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
    std::string filename;

    Document document;
    Command command;
    bool open = true;

    int scroll = 0;
    int gutterSize = 0;

    enum editMode {EDIT=0, COMMAND=1};
    editMode mode = COMMAND;
public:
    explicit Editor(const std::string& filename) : command(document), filename(filename){

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
        mvprintw(screenHeight - 1, 0, (statusMessage + getStatus() + " ").c_str());
        clrtoeol();
        if(mode == COMMAND) attroff(COLOR_PAIR(1));

        // line stats
        auto[line, chara] = document.caretPos();
        std::string lineStats;
        lineStats += (document.isSelection() ? "select    " : "");
        lineStats += std::to_string(line) + ":" + std::to_string(chara);
        int screenWidth = getmaxx(stdscr);
        mvprintw(screenHeight - 1, screenWidth - (int)lineStats.size() - 3, lineStats.c_str());
    }

    static std::string padLeft(std::string s, int width) {
        return s.insert(0, width - s.length(), ' ');
    }

    void updateSelection() {
        document.updateSelection();
        auto sel = document.getSelection();
    }

    std::string sub(const std::string &in, int start, int end) {
        return in.substr(start, end - start);
    }

    void printView() {
        Range selection = document.getSelection();

        int screenHeight = getmaxy(stdscr) - 1; // save a line for status bar
        auto &lines = document.getLines();

        int screenLine = 0, documentLine = int(scroll);
        for(; documentLine < lines.size() && screenLine < screenHeight;
              documentLine++, screenLine++) {

            // line number
            attron(COLOR_PAIR(2));
            std::string row = padLeft(std::to_string(documentLine), (int)ceil(std::log10(lines.size())));
            row += " ";
            mvprintw(screenLine, 0, row.c_str());
            attroff(COLOR_PAIR(2));

            // actual text
            gutterSize = (int)row.size();
            std::string fulltext = lines.at(documentLine);

            // at selection start or end
            if(selection.start.line == selection.end.line) {
                if (documentLine == selection.start.line) {
                    std::string first = sub(fulltext, 0, selection.start.chara);
                    std::string second = sub(fulltext, selection.start.chara, selection.end.chara);
                    std::string third = sub(fulltext, selection.end.chara, fulltext.size());
                    mvprintw(screenLine, gutterSize, first.c_str());
                    attron(A_REVERSE);
                    mvprintw(screenLine, gutterSize + first.size(), second.c_str());
                    attroff(A_REVERSE);
                    mvprintw(screenLine, gutterSize + first.size() + second.size(), third.c_str());
                } else {
                    mvprintw(screenLine, gutterSize, fulltext.c_str());
                }
            }
            // highlight for second part
            else if (documentLine == selection.start.line) {
                std::string first = sub(fulltext, 0, selection.start.chara);
                std::string second = sub(fulltext, selection.start.chara, fulltext.size());
                mvprintw(screenLine, gutterSize, first.c_str());
                attron(A_REVERSE);
                mvprintw(screenLine, gutterSize + first.size(), second.c_str());
                attroff(A_REVERSE);
            }
            // highlight for first part
            else if(documentLine == selection.end.line) {
                std::string first = sub(fulltext, 0, selection.end.chara);
                std::string second = sub(fulltext, selection.end.chara, fulltext.size());
                attron(A_REVERSE);
                mvprintw(screenLine, gutterSize, first.c_str());
                attroff(A_REVERSE);
                mvprintw(screenLine, gutterSize + first.size(), second.c_str());
            }
            // highlight whole thing
            else if(selection.start.line < documentLine && documentLine < selection.end.line) {
                attron(A_REVERSE);
                mvprintw(screenLine, gutterSize, fulltext.c_str());
                attroff(A_REVERSE);
            }
            // highlight nothing
            else {
                mvprintw(screenLine, gutterSize, fulltext.c_str());
            }

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
        // toggle mode
        if(key == 27) { //ESC
            mode = static_cast<enum editMode>((mode + 1) % 2);
            command.clearCommands();
            return;
        }

        static bool wasJustScrolling = false;
        MEVENT event;
        if(key == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(event.bstate & BUTTON5_PRESSED) {
                    document.moveCaretDown();
                    wasJustScrolling = true;
                }else if(event.bstate & BUTTON4_PRESSED) {
                    document.moveCaretUp();
                    wasJustScrolling = true;
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
            command.commandModeCommand(key);
//            if()
//                mode = EDIT;
        }

    }

    [[nodiscard]] bool isOpen() const {
        return open;
    }

    void save() {
        // get text with newlines
        auto &lines = document.getLines();
        std::string fullText;
        for(const auto& line : lines) {
            fullText += line;
            fullText += '\n';
        }
        // remove last newline
        fullText.pop_back();

        std::ofstream out;
        out.open(filename);
        out << fullText;
        out.close();
    }

};

#endif //MINIMA_EDITOR_H
