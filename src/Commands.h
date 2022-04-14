//
// Created by reschivon on 4/12/22.
//

#ifndef MINIMA_COMMANDS_H
#define MINIMA_COMMANDS_H

#include <numeric>

struct CommandContext {
private:
    std::string quantityStr = "";
public:
    int sign = 1;
    enum UNIT {CHAR, WORD, LINE, PARA};
    UNIT unit = WORD;

    void appendQuantityDigit(char d) {
        quantityStr += d;
    }
    int getQuantity(){
        if (!quantityStr.empty())
            return std::stoi(quantityStr);
        else return 1; // default
    }

    Range getWorkingRange(Document &document) {
        switch(unit) {
            case CHAR:
                return Range(document.caretPos(),
                        document.charOffset(document.caretPos(), sign * getQuantity()));
            case WORD:
                return document.wordOffset(sign * getQuantity());
            case LINE:
                return document.lineOffset(sign * getQuantity());
            case PARA:
                return Range({0,0},{0,0}); // not implemented
            default:
                return Range({0,0},{0,0});
        }
    }
};

struct CopyChunk {
    std::string start;
    std::vector<std::string> middle;
    std::string end;
};

class Command {
    std::string commandChain;
    CopyChunk copyBuf;
    Document &document;
    CommandContext context;

    bool commandChainAdd(char key) {
        key = letterLowerCase(key);
        commandChain += key;

        return tryExecCommandChain();
    }

    bool tryExecCommandChain() {
        bool actioned = false;
        context = CommandContext();
        for(char key : commandChain) {
            switch (key) {
                // context chars
                case '-':
                    context.sign = -1;
                    break;

                    // I KNOW IT'S BAD BUT IT WORKS FOR NOW
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '0':
                    context.appendQuantityDigit(char(key));
                    break;

                case 'c':
                    context.unit = CommandContext::CHAR;
                    break;
                case 'w':
                    context.unit = CommandContext::WORD;
                    break;
                case 'r':
                    context.unit = CommandContext::LINE;
                    break;
                case 'p':
                    context.unit = CommandContext::PARA;
                    break;

                    // actionable chars
                case 'd': {// delete
                    Range toDel = context.getWorkingRange(document);
                    document.deleteRange(toDel);
                    if(context.sign < 0)
                        document.moveCaret(toDel.start);
                    else
                        document.moveCaret(toDel.start);

                    actioned = true;
                    break;
                }
                case 'g': {// go to line
                    Range toGo = context.getWorkingRange(document);
                    Point dest;
                    if(context.sign < 0)
                        dest = toGo.start;
                    else
                        dest = toGo.end;

                    document.moveCaret(dest);
                    actioned = true;
                    break;
                }
                case 'h': {
                    document.moveCaret(document.lineStart());
                    actioned = true;
                    break;
                }
                case 'n': {
                    document.moveCaret(document.lineEnd());
                    actioned = true;
                    break;
                }

                default:
                    break;
            }

            if(actioned)
                break;
        }
        return actioned;
    }

    /*
     * Returns whether command was registered
     */
    static
    std::pair<bool, char> controlKey(int c) {
        // exceptions for Enter, Backspace
        if(c == 13  || c == 263)
            return {false, '\0'};

        const char *deCtrled = unctrl(c);
        bool hasControl = deCtrled[0] == '^';
        char nakedChar = deCtrled[strlen(deCtrled) - 1];

        return {hasControl, nakedChar};
    }

    bool immediateCommands(int rawKey) {
        bool validCommand = true;
        switch (char(letterLowerCase(rawKey))) {
            case 'j':
                document.moveCaretLeft();
                break;
            case 'l':
                document.moveCaretRight();
                break;
            case 'i':
                document.moveCaretUp();
                break;
            case 'k':
                document.moveCaretDown();
                break;
            case 'u':
                document.moveCaret(document.prevWord(document.caretPos()).first);
                break;
            case 'o':
                document.moveCaret(document.nextWord(document.caretPos()).first);
                break;
            case 's':
                document.toggleSelection();
                break;
            case 'c': {
                auto selection = document.getSelection();
                if (!selection.isEmpty())
                    selectionToString(selection, copyBuf);
                // dd("sel: " + copyBuf.start);
                break;
            }
            case 'x': {
                auto selection = document.getSelection();
                if (!selection.isEmpty())
                    selectionToString(selection, copyBuf);
                document.deleteRange(selection);
                break;
            }
            case 'v':
                document.insertWithinLine(copyBuf.start);
                if(copyBuf.end.empty())
                    break;
                document.newLine();
                for(const std::string &line : copyBuf.middle) {
                    document.insertWithinLine(line);
                    document.newLine();
                }
                document.insertWithinLine(copyBuf.end);
                break;
            default:
                validCommand = false;
                break;
        }
        return validCommand;
    }

    void editText(int key) {
        switch (key) {
            case KEY_BACKSPACE:
                document.deleteChars(-1);
                break;
            case KEY_DC:
                document.deleteChars(1, false);
                break;
            case KEY_ENTER:
            case 13:
//            case 10:
                document.newLine();
                break;
            case KEY_BTAB:
            case KEY_CTAB:
            case KEY_STAB:
            case KEY_CATAB:
//            case 9:
                document.insertWithinLine("    ");
                break;
            default:
                document.insertWithinLine(std::string(1, char(key)));
        }
    }

    static int letterLowerCase(int letter) {
        // inside [A-Z]
        if (65 <= letter && letter <= 90) {
            return letter + 32;
        }
        return letter;
    }

    static std::string sub(const std::string &in, size_t start, size_t end) {
        return in.substr(start, end - start);
    }

    void selectionToString(Range selection, CopyChunk &dest){
        dest.start.clear();
        dest.middle.clear();
        dest.end.clear();

        auto &lines = document.getLines();
        Point start = selection.start;
        Point end   = selection.end;

        if(start.line == end.line) {
            dest.start += sub(lines.at(start.line), start.chara, end.chara);
            return;
        }
        // begin
        dest.start += sub(lines.at(start.line), start.chara, std::string::npos);
        // mid
        for(auto it = lines.begin() + start.line + 1;
            it < lines.begin() + end.line; it++) {
            dest.middle.push_back(*it);
        }
        // end
        dest.end += sub(lines.at(end.line), 0, end.chara);
    }

public:
    explicit Command(Document& doc) : document(doc) {}

    void editModeCommand(int key) {
        auto [ctrlPressed, commandStripped]  = controlKey(key);
        bool validCommand = false;
        if(ctrlPressed) {
            validCommand = immediateCommands(commandStripped);
        } else {
            editText(key);
            validCommand = true;
        }
        if(validCommand) document.clearSelection();
    }

    bool commandModeCommand(int key) {
        bool commandExecuted = false;
        if(key == KEY_BACKSPACE) {
            backspace();
            return false;
        }
        auto [ctrlPressed, commandStripped] = controlKey(key);

        if(letterLowerCase(commandStripped) == 'y') return false;
        if(letterLowerCase(commandStripped) == 'z') return false;

        bool validCommand = immediateCommands(commandStripped);
        if(!validCommand) {
            bool actioned = commandChainAdd(commandStripped);
            if(actioned) {
                commandChain.clear();
                document.clearSelection();
                return true;
            }
        }
        return false;
    }


    std::string getCommandChain(){
        return commandChain;
    }

    void clearCommands() {
        commandChain.clear();
        context = CommandContext();
    }

    void backspace() {
        if(!commandChain.empty())
            commandChain.erase(commandChain.end()-1, commandChain.end());
    }

};

#endif //MINIMA_COMMANDS_H
