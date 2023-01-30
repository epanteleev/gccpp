#pragma once

#include <utility>
#include "pointer/GcPtr.h"
#include "gc/GCCollected.h"
#include "gc/operations/GCOperation.h"

struct Point : public gccpp::GCCollected {
public:
    Point(int _x, int _y) : x(_x), y(_y) {}

    void trace(gccpp::GCOperation *visitor) noexcept override {}
public:
    int x;
    int y;
};

struct Line : public gccpp::GCCollected {
public:
    Line(gccpp::GcPtr<Point> _a, gccpp::GcPtr<Point> _b) :
            a(std::move(_a)), b(std::move(_b)) {}

    void trace(gccpp::GCOperation *visitor) noexcept override {
        visitor->trace(a);
        visitor->trace(b);
    }

public:
    gccpp::GcPtr<Point> a;
    gccpp::GcPtr<Point> b;
};

struct List: public gccpp::GCCollected {
public:
    List(int _data, gccpp::GcPtr<List> _next) :
            data(_data), next(std::move(_next)) {}

    void trace(gccpp::GCOperation* visitor) noexcept override {
        visitor->trace(next);
    }
public:
    int data;
    gccpp::GcPtr<List> next;
};