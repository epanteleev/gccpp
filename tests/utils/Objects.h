#pragma once

#include <utility>
#include "pointer/Oop.inline.h"
#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"

struct Point : public gccpp::GarbageCollected {
public:
    Point(int _x, int _y) : x(_x), y(_y) {}

    void trace(gccpp::GCOperation *visitor) noexcept override {
        (void)(visitor);
    }
public:
    int x;
    int y;
};

struct Line : public gccpp::GarbageCollected {
public:
    Line(gccpp::Oop<Point> _a, gccpp::Oop<Point> _b) :
            a(std::move(_a)), b(std::move(_b)) {}

    void trace(gccpp::GCOperation *visitor) noexcept override {
        visitor->trace(a);
        visitor->trace(b);
    }

public:
    static gccpp::Oop<Line> createLine(gccpp::Enviroment &ctx);

public:
    gccpp::Oop<Point> a;
    gccpp::Oop<Point> b;
};

struct List: public gccpp::GarbageCollected {
public:
    List() = default;
    List(int _data, gccpp::Oop<List> _next) :
            data(_data), next(std::move(_next)) {}

public:
    void trace(gccpp::GCOperation* visitor) noexcept override {
        visitor->trace(next);
    }

public:
    int data{};
    gccpp::Oop<List> next{};
};