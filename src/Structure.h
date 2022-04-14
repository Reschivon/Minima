#include <utility>

//
// Created by reschivon on 4/14/22.
//

#ifndef MINIMA_STRUCTURE_H
#define MINIMA_STRUCTURE_H


struct Point {
    int line, chara;
};

class Range final {

    static bool goesForward(Point start, Point end) {
        if (start.line < end.line)
            return true;
        if (start.line > end.line)
            return false;
        return start.chara < end.chara;
    }
    
public:
    Point start, end;

    Range(Point start_, Point end_) {
        if(goesForward(start_, end_)) {
            start = start_;
            end = end_;
        } else {
            start = end_;
            end = start_;
        }
    }

    [[nodiscard]] bool isEmpty() const{
        return start.line == end.line && start.chara == end.chara;
    }
};

struct Action {
    enum Type {ADD, DELETE};
    Type type;
    std::string heft;
    Range range;
};

// Shamelessly ripped from SO
template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif //MINIMA_STRUCTURE_H
