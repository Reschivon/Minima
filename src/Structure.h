#include <utility>

//
// Created by reschivon on 4/14/22.
//

#ifndef MINIMA_STRUCTURE_H
#define MINIMA_STRUCTURE_H


struct Point {
    int line, chara;

    bool operator==(const Point& rhs) const {
        return line == rhs.line && chara == rhs.chara;
    }
    bool operator!=(const Point& rhs) const {
        return !(*this == rhs);
    }

    static Point origin;
};

Point Point::origin = {0, 0};

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

    static Range empty;
};

Range Range::empty = {{0, 0}, {0, 0}};

struct Action {
    enum Type {ADD, DELETE};
    Type type;
    std::string heft;
    Range range;
};

enum EditMode {EDIT=0, COMMAND=1};
enum REQUESTED_ACTION {SAVE, TOEDIT, TOCMD, NOTHING};

// Shamelessly ripped from SO
template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

std::string tab = "    ";

#endif //MINIMA_STRUCTURE_H
