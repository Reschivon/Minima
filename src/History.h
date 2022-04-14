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
        actions.push_back(act);
        currentAction++;
        actions.erase(actions.begin() + currentAction, actions.end());
    }

    void undoLastAction() {
        if(currentAction <= 0) {
            return;
        }
        Action action = actions.at(--currentAction);
        undoAction(action);
    }

    void redoAction() {
        if(currentAction >= actions.size())
            return;
        Action action = actions.at(currentAction++);
        doAction(action);
    }

private:
    void undoAction(const Action& action) {
        freezeHist = true;
        if(action.type == Action::DELETE) {
            document.insertString(action.heft);
        }
        if(action.type == Action::ADD) {
            document.deleteRange(action.range);
        }
        freezeHist = false;
    }

    void doAction(const Action& action) {
        freezeHist = true;
        if(action.type == Action::DELETE) {
            document.deleteRange(action.range);
        }
        if(action.type == Action::ADD) {
            document.insertString(action.heft);
        }
        freezeHist = false;
    }

};

#endif //MINIMA_HISTORY_H
