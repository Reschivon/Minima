//
// Created by reschivon on 4/11/22.
//

#ifndef MINIMA_DOCUMENT_H
#define MINIMA_DOCUMENT_H

#include <utility>
#include <optional>

#include "Structure.h"

class Document {
private:
    std::vector<std::string> lines;
    int caretChar = 0, caretLine = 0;

    Point selectBegin = {0, 0};
    Point selectEnd = {0, 0};
    bool selecting = false;

    /* Steppers */

    [[nodiscard]]
    std::pair<Point,bool> stepLeft(Point curr) const {
        bool success = true;
        if(curr.chara == 0) {
            // move up a line if possible
            if(curr.line > 0) {
                curr.line--;
                curr.chara = (int) lines.at(curr.line).length();
            } else {
                success = false;
            }
        } else {
            // move back a character
            curr.chara--;
        }

        return {curr, success};
    }

    [[nodiscard]]
    std::pair<Point, bool> stepRight(Point curr) const {
        bool success = true;
        if(curr.chara == lines.at(curr.line).length()) {
            // move down a line if possible
            if(curr.line < lines.size() - 1) {
                curr.line++;
                curr.chara = 0;
            } else {
                success = false;
            }
        } else {
            // move forward a character
            curr.chara++;
        }

        return {curr, success};
    }

    [[nodiscard]]
    std::pair<Point, bool> stepChar(Point curr, int direction) const {
        if(direction < 0)
            return stepLeft(curr);
        if(direction > 0)
            return stepRight(curr);
        return {curr, true};
    }

    /**
    * Insert any char, including new line
    */
    void insertChar(char insert, Point start) {
        if(insert == '\n') {
            auto &currLine = lines.at(start.line);
            auto rightOfCaret = currLine.substr(start.chara, std::string::npos);
            currLine.erase(start.chara, std::string::npos);
            lines.insert(lines.begin() + start.line + 1, rightOfCaret);
            caretLine += 1;
            caretChar = 0;
            return;
        } else {
            lines.at(start.line).insert(start.chara, std::string(1, insert));
            caretChar++;
        }
    }

    void insertInLine(const std::string& insert, Point start) {
        auto &line = lines.at(start.line);
        line.insert(start.chara, insert);
        caretChar += insert.size();
    }

public:

    std::function<void(Action)> updateHistory{};

    /* Related to Highlighting */

    void toggleSelection() {
        if (selecting) {
            selecting = false;
        } else {
            selecting = true;
            selectBegin = caret();
            selectEnd = caret();
        }
    }

    Range getSelection() {
        Range sel(selectBegin, selectEnd);
        validifyRange(sel);
        return sel;
    }

    [[nodiscard]] bool isSelecting() const {
        return selecting;
    }

    void updateSelection() {
        if(selecting)
            selectEnd = caret();
    }

    void validifyRange(Range &check) {
        if(check.start.line >= lines.size())
            check.start.line = (int) lines.size() - 1;
        if(check.end.line >= lines.size())
            check.end.line = (int)lines.size() - 1;
        if(check.start.chara > lines.at(check.start.line).size())
            check.start.chara = (int)lines.at(check.start.line).size();
        if(check.end.chara > lines.at(check.end.line).size())
            check.end.chara = (int) lines.at(check.end.line).size();
    }

    /* Loose utils */

    const std::vector<std::string> &getLines() {
        return lines;
    }

    void setLines(std::vector<std::string> newLines) {
        lines = std::move(newLines);
    }

    [[nodiscard]] inline
    Point caret() const {
        return {caretLine, caretChar};
    }
    [[nodiscard]]
    int line() const {
        return caretLine;
    }
    [[nodiscard]]
    int chara() const {
        return caretChar;
    }

    [[nodiscard]] static
    Point lineStart(Point start) {
        return {start.line, 0};
    }
    [[nodiscard]]
    Point lineEnd(Point start) const {
        return {start.line, (int)lines.at(start.line).size()};
    }

    [[nodiscard]]
    char charAt(Point p) const {
        if(lines.at(p.line).length() == p.chara) {
            return ' ';
        }

        return lines.at(p.line).at(p.chara);
    }

    void setCaret(Point p) {
        p.line = std::clamp(p.line,
                            0,
                            (int) lines.size() - 1);
        p.chara = std::clamp(p.chara,
                             0,
                             (int) lines.at(p.line).size());

        caretLine = p.line;
        caretChar = p.chara;
    }


    /* Static steppers */
    Point charOffset(Point start, int offsetChars) const {
        int sign = signum(offsetChars);
        offsetChars = abs(offsetChars);

        std::pair<Point, bool> curr = {start, true};
        while(offsetChars --> 0) {
            curr = stepChar(curr.first, sign);
            if(!curr.second) break;
        }

        return curr.first;
    }

    [[nodiscard]]
    Point nextCharChange(Point currChar, int num) const {
        std::pair<Point, bool> runningChar = {currChar, true};
        bool startState = std::isspace(charAt(currChar));

        // move until exit or enter word
        int transitions = 0;
        do {
            runningChar = stepChar(runningChar.first, num);
            bool currSpace = isspace(charAt(runningChar.first));
            if(currSpace != startState) {
                transitions++;
                startState = currSpace;
            }
            if(!runningChar.second) return runningChar.first;
        } while(transitions < abs(num));

        return runningChar.first;
    }

    [[nodiscard]]
    Range wordOffset(Point start, int num) const {
        if(num == 0)
            return {start, start};

        // each word counts for two changes
        if(num < 0) num = num * 2 + 1;
        else        num = num * 2 - 1;
        if(num < 0 ) start = charOffset(start, -1);

        Point end = nextCharChange(start, num);

        // if move left and not analized, bip a lil right
        if(num < 0 && end != Point::origin) end = stepChar(end, 1).first;

        return Range(start, end);
    }

    [[nodiscard]]
    Range lineOffset(Point start, int num) const {
        if(num == 0) return {caret(), caret()};

        start = {start.line, 0};

        int endLine = start.line + num;
        endLine = std::clamp(endLine,
                             0,
                             (int) lines.size() - 1);

        return Range(start, {endLine, 0});
    }

    [[nodiscard]]
    Range paraOffset(Point start, int num) {
        if(num == 0) return {caret(), caret()};

        auto validLine = [this](int line){return line >= 0 && line < lines.size();};
        auto isParagraphHead = [this](int line){
            if(line <= 0 || line >= lines.size()) return true;
            auto currLine = lines.at(line), prevLine = lines.at(line-1);
            return currLine.find(tab) == 0  // start with tab
                   || std::all_of(prevLine.begin(), prevLine.end(), isspace) // blank line
                   || prevLine.empty();}; // blank line

        // find start of current paragraph
        while(start.line > 0 && !isParagraphHead(start.line))
            start.line--;

        // loop for num of iterations
        int i = 0, line = start.line;
        while(validLine(line) && i < num * signum(num)) {
            line += signum(num);
            if (isParagraphHead(line))
                i++;
        }

        line = std::clamp(line, 0, (int)lines.size()-1);

        Point end = {line, 0};

        return Range(start, end);
    }

    static std::string substring(const std::string &in, size_t start, size_t end) {
        if(start - end == 0)
            return "";
        return in.substr(start, end - start);
    }
    static void eraseString(std::string &in, size_t start, size_t end) {
        in.erase(start, end - start);
    }

    /* Bad boy general insert and delete */
    void deleteRange(Range toDelete) {
        if(toDelete.start.line == toDelete.end.line) {
            auto &startLine = lines.at(toDelete.start.line);
            eraseString(startLine, toDelete.start.chara, toDelete.end.chara);
            setCaret(toDelete.start);

            updateHistory({
                  Action::DELETE,
                  selectionToString(toDelete),
                  toDelete
                  });

            return;
        }

        std::string stringToDelete = selectionToString(toDelete);

        // Merge lines whose linebreak has been deleted
        auto &startLine = lines.at(toDelete.start.line);
        eraseString(startLine, toDelete.start.chara, std::string::npos);

        auto endLine = lines.at(toDelete.end.line);
        auto endLineRight = endLine.substr(toDelete.end.chara, endLine.size() - toDelete.end.chara);

        startLine += endLineRight;

        // delete the complete lines
        int numToDel = toDelete.end.line - toDelete.start.line;
        if (numToDel > 0)
            lines.erase(lines.begin() + toDelete.start.line + 1,
                        lines.begin() + toDelete.end.line + 1);

        updateHistory({Action::DELETE, stringToDelete, toDelete
        });

        setCaret(toDelete.start);
    }


    void insertString(const std::string& insert) {
        Point initialCaret = caret();

        size_t pos = 0;
        while (true) {
            size_t nextNewline = (int) insert.find('\n', pos);
            insertInLine(substring(insert, pos, nextNewline), caret());

            if(nextNewline == std::string::npos)
                break;
            pos = nextNewline + 1;
            insertChar('\n', caret());
        }

        updateHistory({
            Action::ADD,
            insert,
            {initialCaret, caret()}
        });
    }


    std::string selectionToString(Range selection){
        std::string text;

        validifyRange(selection);

        Point start = selection.start;
        Point end   = selection.end;

        if(start.line == end.line) {
            text += substring(lines.at(start.line), start.chara, end.chara);
            return text;
        }
        // begin
        text += substring(lines.at(start.line), start.chara, std::string::npos);
        text += '\n';

        // mid
        for(auto it = lines.begin() + start.line + 1;
            it < lines.begin() + end.line; it++) {
            text += (*it);
            text += '\n';
        }

        // end
        text += substring(lines.at(end.line), 0, end.chara);

        return text;
    }
};

#endif //MINIMA_DOCUMENT_H
