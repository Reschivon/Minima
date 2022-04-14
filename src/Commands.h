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
                return Range(doc.caret(),
                             doc.charOffset(doc.caret(), sign * getQuantity()));
            case WORD:
                return doc.wordOffset(doc.caret(), sign * getQuantity());
            case LINE:
                return doc.lineOffset(doc.caret(), sign * getQuantity());
            case PARA:
                return Range({0,0},{0,0}); // not implemented
            default:
                return Range({0,0},{0,0});
        }
    }
};

class Command {
    std::string commandChain;
    std::string copyBuf;
    Document &doc;
    CommandContext context;
    History &history;

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
                    Range toDel = context.getWorkingRange(doc);
                    doc.deleteRange(toDel);
                    doc.setCaret(toDel.start);

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
                doc.toggleSelection();
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
                doc.insertString("    ");
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


public:
    explicit Command(Document& doc, History &history) : doc(doc), history(history) {}

    void editModeCommand(int key) {
        auto [ctrlPressed, commandStripped] = controlKey(key);
        bool validCommand = false;
        if(ctrlPressed) {
            validCommand = immediateCommands(commandStripped);
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

        // if(letterLowerCase(commandStripped) == 'y') return false;
        // if(letterLowerCase(commandStripped) == 'z') return false;

        bool validCommand = immediateCommands(commandStripped);
        if(!validCommand) {
            bool actioned = commandChainAdd(commandStripped);
            if(actioned) {
                commandChain.clear();
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
