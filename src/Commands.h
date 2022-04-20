//
// Created by reschivon on 4/12/22.
//

#ifndef MINIMA_COMMANDS_H
#define MINIMA_COMMANDS_H

#include <numeric>
#include "Document.h"
#include "History.h"

struct CommandContext {
private:
    std::string quantityStr;
public:
    int sign = 1;
    enum UNIT {CHAR, WORD, LINE, PARA};
    UNIT unit = WORD;
    std::string literalString;

    void appendQuantityDigit(char d) {
        quantityStr += d;
    }
    int getQuantity(){
        if (!quantityStr.empty())
            return std::stoi(quantityStr);
        else return 1; // default
    }

    Range getWorkingRange(Document &doc) {
        switch(unit) {
            case CHAR:
                return {doc.caret(),
                        doc.charOffset(doc.caret(), sign * getQuantity())};
            case WORD:
                return doc.wordOffset(doc.caret(), sign * getQuantity());
            case LINE:
                return doc.lineOffset(doc.caret(), sign * getQuantity());
            case PARA:
                return doc.paraOffset(doc.caret(), sign * getQuantity()); // not implemented
            default:
                return Range::empty;
        }
    }
};

class Command {
    Document &doc;
    CommandContext context;
    History &history;

    std::string prevCommandChain;
    std::string commandChain;

    std::string copyBuf;

    // state
    bool typingString = false;



public:
    explicit Command(Document& doc, History &history) : doc(doc), history(history) {}


    REQUESTED_ACTION eatKey(int key, EditMode mode) {

        // toggle mode
        if(key == 27) { //ESC
            mode = static_cast<enum EditMode>((mode + 1) % 2);
            clearCommands();
            return mode == EDIT ? TOEDIT : TOCMD;
        }

        if(key == 410) { // ignore resize
            goto end;
        }

        static bool wasJustScrolling = false;
        MEVENT event;
        if(key == KEY_MOUSE && (getmouse(&event) == OK)) {
            if(event.bstate & BUTTON5_PRESSED) {
                doc.setCaret({doc.line()+1, doc.chara()});
                wasJustScrolling = true;
            }else if(event.bstate & BUTTON4_PRESSED) {
                doc.setCaret({doc.line()-1, doc.chara()});
                wasJustScrolling = true;
            }
            goto end;
        }

        if(mode == EDIT) {
            editModeCommand(key);
        }else if(mode == COMMAND) {
            if(key == 'q' || key == 'Q')
                return SAVE;
            commandModeCommand(key);
        }

        // valid use of goto, shut up
        end:
        return NOTHING;
    }


    void editModeCommand(int key) {
        auto [ctrlPressed, commandStripped] = controlKey(key);
        bool validCommand = false;
        if(ctrlPressed) {
            validCommand = immediateCommands(commandStripped, key);
        } else {
            editText(key);
            validCommand = true;
        }
    }

    bool commandModeCommand(int key) {
        bool commandExecuted = false;
        if(key == KEY_BACKSPACE) {
            backspace();
            return false;
        }

        auto [ctrlPressed, commandStripped] = controlKey(key);

        if(typingString) {
            return commandChainAdd(commandStripped);
        }

        if(immediateCommands(commandStripped, key))
            return true;
        else
            return commandChainAdd(commandStripped);
    }

    bool commandChainAdd(char key) {
        commandChain += letterLowerCase(key);

        bool actioned = tryExecCommandChain();
        if(actioned) {
            prevCommandChain = commandChain;
            commandChain.clear();
        }
        return actioned;
    }

    bool tryExecCommandChain() {
        bool actioned = false;
        context = CommandContext();

        bool escaped = false;
        for(char key : commandChain) {
            if(key == '\'') {
                typingString = true;
                continue;
            }

            if(typingString) {
                char letter = char(key);
                if(letter == ' ') {
                    typingString = false;
                    continue;
                }
                if(letter == '\\') {
                    escaped = true;
                    continue;
                }
                if(escaped && key == 'n') {
                    context.literalString += '\n';
                    escaped = false;
                } else {
                    context.literalString += letter;
                }
                continue;
            }

            switch (key) {
                // context chars
                case '-':
                case 'b':
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

                /* actionable chars */
                case 'd': {
                    Range toDel = Range::empty;
                    if(!doc.getSelection().isEmpty())
                        // delete selection
                        toDel = doc.getSelection();
                    else // delete range from command context
                        toDel = context.getWorkingRange(doc);
                    doc.deleteRange(toDel);
                    doc.setSelection(Range::empty);
                    actioned = true;
                    break;
                }
                case 'g': {// go to line
                    Range range = context.getWorkingRange(doc);
                    Point dest = context.sign < 0 ? range.start : range.end;
                    doc.setCaret(dest);
                    actioned = true;
                    break;
                }
                case 'h': {
                    doc.setCaret(Document::lineStart(doc.caret()));
                    actioned = true;
                    break;
                }
                case 'n': {
                    doc.setCaret(doc.lineEnd(doc.caret()));
                    actioned = true;
                    break;
                }
                case 'a': {
                    commandChain = prevCommandChain;
                    tryExecCommandChain();
                    actioned = true;
                    break;
                }
                case 'f': {
                    if(context.literalString.empty()) {
                        dd("search string is empty");
                        break;
                    }
                    auto search = doc.search(doc.charOffset(doc.caret(), 1), context.literalString, context.sign);
                    if(!search.second)
                        dd("Reached end of file");
                    else {
                        doc.setSelection(search.first);
                        doc.setCaret(search.first.start);
                    }
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

    bool wasJustShiftin = false;
    bool immediateCommands(int strippedKey, int rawKey) {
        bool validCommand = true;
        switch (char(rawKey)) {
            // shift held
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'U':
            case 'O':
                if(!doc.isSelecting())
                    doc.startSelection();
                wasJustShiftin = true;
                break;
            default:
                if(wasJustShiftin)
                    doc.stopSelection();
                wasJustShiftin = false;
                break;
        }

        switch (char(letterLowerCase(strippedKey))) {
            case 'j':
                doc.setCaret(doc.charOffset(doc.caret(), -1));
                break;
            case 'l':
                doc.setCaret(doc.charOffset(doc.caret(), 1));
                break;
            case 'i':
                doc.setCaret({doc.line() - 1, doc.chara()});
                break;
            case 'k':
                doc.setCaret({doc.line() + 1, doc.chara()});
                break;
            case 'u':
                doc.setCaret(doc.wordOffset(doc.caret(), -1).start);
                break;
            case 'o':
                doc.setCaret(doc.wordOffset(doc.caret(), 1).end);
                break;
            case 's':
                if(!commandChain.empty() && !doc.isSelecting()) {
                    doc.startSelection();
                    Range range = context.getWorkingRange(doc);
                    Point dest = context.sign < 0 ? range.start : range.end;
                    doc.setCaret(dest);
                } else {
                    doc.toggleSelection();
                }
                break;
            case 'c': {
                auto selection = doc.getSelection();
                if (!selection.isEmpty())
                    copyBuf = doc.selectionToString(selection);
                break;
            }
            case 'x': {
                auto selection = doc.getSelection();
                if (!selection.isEmpty())
                    copyBuf = doc.selectionToString(selection);
                doc.deleteRange(selection);
                break;
            }
            case 'v':
                doc.insertString(copyBuf);
                break;
            case 'z':
                history.undoLastAction();
                break;
            case 'y':
                history.redoAction();
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
                doc.deleteRange({doc.caret(), doc.charOffset(doc.caret(), -1)});
                doc.stopSelection();
                break;
            case KEY_DC:
                doc.deleteRange({doc.caret(), doc.charOffset(doc.caret(), 1)});
                break;
            case KEY_ENTER:
            case 13:
//            case 10:
                doc.insertString("\n");
                break;
            case KEY_BTAB:
            case KEY_CTAB:
            case KEY_STAB:
            case KEY_CATAB:
//            case 9:
                doc.insertString(tab);
                break;
            default:
                doc.insertString(std::string(1, char(key)));
        }
    }

    static int letterLowerCase(int letter) {
        // inside [A-Z]
        if (65 <= letter && letter <= 90) {
            return letter + 32;
        }
        return letter;
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
