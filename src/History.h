//
// Created by reschivon on 4/13/22.
//

#ifndef MINIMA_HISTORY_H
#define MINIMA_HISTORY_H

#include <variant>
#include "Structure.h"
#include "Document.h"


class History {
    Document &document;
    std::vector<Action> actions{};
    int currentAction = 0;
    bool freezeHist = false;

public:
    explicit History(Document &doc) : document(doc) {
    }

    void addAction(Action act) {
        if(freezeHist)
            return;

        actions.erase(actions.begin() + currentAction, actions.end());

        actions.push_back(act);
        currentAction++;
    }

    void undoLastAction() {
        if(currentAction <= 0) {
            dd("Nothing to undo");
            return;
        }
        Action action = actions.at(--currentAction);
        undoAction(action);
    }

    void redoAction() {
        if(currentAction >= actions.size()) {
            dd("At most recent");
            return;
        }
        Action action = actions.at(currentAction++);
        doAction(action);
    }

private:
    void undoAction(const Action& action) {
        freezeHist = true;
        Point origCaret = document.caret();

        if(action.type == Action::DELETE) {
            document.setCaret(action.range.start);
            document.insertString(action.heft);
        }
        if(action.type == Action::ADD) {
            document.deleteRange(action.range);
        }

        document.setCaret(origCaret);
        freezeHist = false;
    }

    void doAction(const Action& action) {
        freezeHist = true;
        Point origCaret = document.caret();

        if(action.type == Action::DELETE) {
            document.deleteRange(action.range);
        }
        if(action.type == Action::ADD) {
            document.setCaret(action.range.start);
            document.insertString(action.heft);
        }

        document.setCaret(origCaret);
        freezeHist = false;
    }

};

#endif //MINIMA_HISTORY_H
