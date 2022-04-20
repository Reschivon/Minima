//
// Created by reschivon on 4/11/22.
//

#ifndef MINIMA_EDITOR_H
#define MINIMA_EDITOR_H

#include "Document.h"
#include "Commands.h"
#include "History.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <bitset>
#include <utility>

class Editor {
private:
    std::string filename;

    Document document;
    Command command;
    History history;

    bool open = true;

    int scroll = 0;
    int gutterSize = 0;

    EditMode mode = COMMAND;
public:
    explicit Editor(const std::string& filename)
                : filename(filename), history(document), command(document, history){

        std::ifstream infile(filename.c_str());
        if(infile.is_open()) {
            // read the file to document
            std::vector<std::string> allLines{};
            while (!infile.eof()) {
                std::string line;
                getline(infile, line);
                allLines.push_back(line);
            }
            document.setLines(std::move(allLines));
        } else {
            open = false;
        }
        infile.close();

        document.updateHistory = [this](Action action){history.addAction(std::move(action));};
    }

    void printStatusLine() {
        int screenHeight = getmaxy(stdscr);
        std::string statusMessage;
        if(mode == COMMAND) {
            statusMessage += " Command: ";
            statusMessage += command.getCommandChain();
        }

        // command bar
        if(mode == COMMAND) attron(COLOR_PAIR(1));
        mvprintw(screenHeight - 1, 0, (statusMessage + getStatus() + " ").c_str());
        clrtoeol();
        if(mode == COMMAND) attroff(COLOR_PAIR(1));

        // line stats
        auto[line, chara] = document.caret();
        std::string lineStats;
        lineStats += (document.isSelecting() ? "select    " : "");
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
            if(document.line() == documentLine) attron(COLOR_PAIR(3) | A_BOLD);
            else                                attron(COLOR_PAIR(2));

            std::string row = padLeft(std::to_string(documentLine), (int)ceil(std::log10(lines.size())));
            row += " ";
            mvprintw(screenLine, 0, row.c_str());
            attroff(COLOR_PAIR(2));

            if(document.line() == documentLine) attroff(COLOR_PAIR(3) | A_BOLD);
            else                                attroff(COLOR_PAIR(2));

            row += " ";

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

//            int screenWidth = getmaxx(stdscr);
//            int currX = gutterSize + fulltext.size();
//            mvprintw(screenLine, currX, std::string(screenWidth - currX, ' ').c_str());
            clrtoeol();
        }

        for(; screenLine < screenHeight; screenLine++) {
            move(screenLine, 0);
            clrtoeol();
        }

    }

    void setScroll() {
        auto caretPos = document.caret();
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
        auto caretPos = document.caret();
        move(caretPos.line - scroll, gutterSize + caretPos.chara);

    }
    void eatInput(int key) {
        setStatus("");

        REQUESTED_ACTION req = command.eatKey(key, mode);
        if(req == TOCMD)
            mode = COMMAND;
        if(req == TOEDIT)
            mode = EDIT;
        if(req == SAVE) {
            save();
            open = false;
        }
    }

    [[nodiscard]]
    bool isOpen() const {
        return open;
    }
    void close() {
        open = false;
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
