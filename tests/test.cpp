#include <gtest/gtest.h>

#include <utility>
#include "pointer/GcPtr.h"
#include "Gc.h"

struct Point {
public:
    Point(int _x, int _y) :
            x(_x),
            y(_y) {}

public:
    int x;
    int y;
};

struct Line {
public:
    Line(tgc::GcPtr<Point> _a, tgc::GcPtr<Point> _b) :
            a(std::move(_a)), b(std::move(_b)) {}

public:
    tgc::GcPtr<Point> a;
    tgc::GcPtr<Point> b;
};

TEST(gc_test1, creation) {
    tgc::Gc gc;
    gc.enter();

    auto root = gc.gc_alloc<Point>(2, 3);

    ASSERT_EQ(root->x, 2);
    ASSERT_EQ(root->y, 3);
}

TEST(gc_test2, creation) {
    tgc::Gc gc;
    gc.enter();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);
    ASSERT_EQ(root->a->y, 3);
    ASSERT_EQ(root->b->y, 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}