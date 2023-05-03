//
// Created by user on 04.04.23.
//
#include "Objects.h"

gccpp::Oop<Line> Line::createLine(gccpp::Environment &ctx) {
    auto point1 = ctx.alloc<Point>(2, 3);
    auto point2 = ctx.alloc<Point>(20, 30);
    auto root = ctx.alloc<Line>(point1, point2);
    return root;
}
