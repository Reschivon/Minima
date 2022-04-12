//
// Created by reschivon on 4/11/22.
//

#ifndef MINIMA_DOCUMENT_H
#define MINIMA_DOCUMENT_H

struct Point {
    uint line, chara;
};

class Range {

    static bool goesForward(Point start, Point end) {
        if (end.line < start.line)
            return false;
        if (end.chara < start.chara)
            return false;
        return true;
    }

public:
    Point start{}, end{};

    Range(Point start_, Point end_) {
        if(goesForward(start_, end_)) {
            start = start_;
            end = end_;
        } else {
            start = end_;
            end = start_;
        }
    }
};


class Document {
private:
    std::vector<std::string> lines;
    uint caretChar = 0, caretLine = 0;

    std::pair<Point, bool> moveLeft(Point curr) {
        bool success = true;
        if(curr.chara == 0) {
            // move up a line if possible
            if(curr.line > 0) {
                curr.line--;
                curr.chara = lines.at(curr.line).length();
            } else {
                success = false;
            }
        } else {
            // move back a character
            curr.chara--;
        }
        return std::make_pair(curr, success);

    }

    std::pair<Point, bool> moveRight(Point curr) {
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
        return std::make_pair(curr, success);
    }

    Point charOffset(Point start, int offsetChars) {
        Point curr = start;

        if(offsetChars < 0) {
            offsetChars = abs(offsetChars);
            while(offsetChars --> 0) {
                bool success;
                std::tie(curr, success) = moveLeft(curr);
                if(!success) break;
            }
        } else {
            while (offsetChars --> 0) {
                bool success;
                std::tie(curr, success) = moveRight(curr);
                if(!success) break;
            }
        }

        return curr;
    }

    void deleteRange(Range toDelete) {
        if(toDelete.start.line == toDelete.end.line) {
            auto &startLine = lines.at(toDelete.start.line);
            startLine.erase(toDelete.start.chara, toDelete.end.chara - toDelete.start.chara);
            return;
        }

        // Merge lines whose linebreak has been deleted
        auto endLine = lines.at(toDelete.end.line);
        auto endLineRight = endLine.substr(toDelete.end.chara, endLine.size() - toDelete.end.chara);

        auto &startLine = lines.at(toDelete.start.line);
        startLine.erase(startLine.begin() + toDelete.start.chara,
                        startLine.end());
        startLine += endLineRight;

        // delete the complete lines
        int numToDel = toDelete.end.line - toDelete.start.line;
        if (numToDel > 0)
            lines.erase(lines.begin() + toDelete.start.line + 1,
                        lines.begin() + toDelete.end.line + 1);
    }

public:
    void addLine(const std::string& line) {
        lines.push_back(line);
    }

    void newLine() {
        auto &currLine = lines.at(caretLine);
        auto rightOfCaret = currLine.substr(caretChar, currLine.size() - caretChar);
        currLine.erase(caretChar, currLine.size() - caretChar);
        lines.insert(lines.begin() + caretLine + 1, rightOfCaret);
        caretLine++;
        caretChar = 0;
    }

    void insertWithinLine(const std::string& insert, bool moveCaret = true) {
        lines.at(caretLine).insert(caretChar, insert);
        if(moveCaret) {
            moveCaretChars(insert.length());
        }
    }

    void deleteChars(int direction, bool moveCaret = true) {
        Point toDelete = charOffset({caretLine, caretChar}, direction);
        deleteRange(Range({caretLine, caretChar}, toDelete));
        if(moveCaret) {
            caretLine = toDelete.line;
            caretChar = toDelete.chara;
        }
    }

    void moveCaretChars(int num) {
        Point newPos = charOffset({caretLine, caretChar}, num);
        caretLine = newPos.line;
        caretChar = newPos.chara;
    }

    void moveCaret(Point p) {
        caretLine = p.line;
        caretChar = p.chara;
    }

    void moveCaretLeft() {
        auto new_pos = moveLeft({caretLine, caretChar});
        caretLine = new_pos.first.line;
        caretChar = new_pos.first.chara;
    }

    void moveCaretRight() {
        auto new_pos = moveRight({caretLine, caretChar});
        caretLine = new_pos.first.line;
        caretChar = new_pos.first.chara;
    }

    void moveCaretUp() {
        if(int(caretLine) - 1 < 0)
            return;
        caretLine--;
        caretChar = std::clamp((ulong) caretChar,
                               (ulong) 0,
                               lines.at(caretLine).length());
    }

    void moveCaretDown() {
        if(caretLine + 1 >= lines.size())
            return;
        caretLine++;
        caretChar = std::clamp((ulong) caretChar,
                               (ulong) 0,
                               lines.at(caretLine).length());
    }

    std::vector<std::string> &getLines() {
        return lines;
    }

    Point getCaretPos() {
        return {caretLine, caretChar};
    }

    char charAt(Point p) {
        if(lines.at(p.line).length() == p.chara) {
            return ' ';
        }

        return lines.at(p.line).at(p.chara);
    }

    Point nextWord() {
        Point currChar{caretLine, caretChar};
        bool success;

        std::tie(currChar, success) = searchWord(currChar, [this](Point p){return moveRight(p);});
        return currChar;
    }

    Point prevWord() {
        auto [currChar, success] = moveLeft({caretLine, caretChar});
        if(!success)
            return {caretLine, caretChar};

        std::tie(currChar, success) = searchWord(currChar, [this](Point p){return moveLeft(p);});

        if (!success)
            return currChar;
        // now we move one char back right
        return moveRight(currChar).first;
    }

    Point lineStart() {
        return {caretLine, 0};
    }
    Point lineEnd() {
        return {caretLine, (uint)lines.at(caretLine).size()};
    }

    /*
     * OOf, too clever
     */
    std::pair<Point, bool> searchWord(Point currChar, const std::function<std::tuple<Point, bool>(Point)>& advance) {

        std::function<bool(Point)> continuation;
        if(charAt(currChar) == ' ')
            continuation = [this](Point c){return charAt(c) == ' ';};
        if(charAt(currChar) != ' ')
            continuation = [this](Point c){return charAt(c) != ' ';};

        // move until exit or enter word
        do {
            bool success;
            std::tie(currChar, success) = advance(currChar);
            if(!success) return {currChar, false};
        } while(continuation(currChar));

        return {currChar, true};
    }
};

#endif //MINIMA_DOCUMENT_H
