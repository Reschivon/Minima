//
// Created by reschivon on 4/12/22.
//

#ifndef MINIMA_COMMANDS_H
#define MINIMA_COMMANDS_H

class Command {
    std::string commandChain;
    Document &document;

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
                document.moveCaret(document.prevWord());
                break;
            case 'o':
                document.moveCaret(document.nextWord());
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

    void commandChainAdd(char key) {
        key = letterLowerCase(key);
        commandChain += key;
    }
public:
    explicit Command(Document& doc) : document(doc) {}

    void editModeCommand(int key) {
        auto [ctrlPressed, commandStripped]  = controlKey(key);
        if(ctrlPressed) {
            bool validCommand = immediateCommands(commandStripped);
//            if(!validCommand) {
//                commandChainAdd(commandStripped);
//            }
        } else {
            editText(key);
        }
    }

    void commandModeCommand(int key) {
        if(key == KEY_BACKSPACE) {
            backspace();
            return;
        }
        auto [ctrlPressed, commandStripped] = controlKey(key);
        bool validCommand = immediateCommands(commandStripped);
        if(!validCommand) {
            commandChainAdd(commandStripped);
        }
    }


    std::string getCommandChain(){
        return commandChain;
    }

    void clearCommands() {
        commandChain.clear();
    }

    void backspace() {
        commandChain.erase(commandChain.end()-1, commandChain.end());
    }

};

#endif //MINIMA_COMMANDS_H
