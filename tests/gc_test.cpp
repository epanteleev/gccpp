#include <gtest/gtest.h>

#include <utility>
#include "pointer/GcPtr.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/GCCollected.h"
#include "gc/operations/GCOperation.h"
#include "gc/collectors/MarkAndCompactCollector.h"

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

gccpp::GcPtr<Line> createLine(gccpp::GC &gc) {
    gccpp::Frame fr = gc.enter();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);
    return root;
}

TEST(gc_test1, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto root = gc.gc_alloc<Point>(2, 3);

    ASSERT_EQ(root->x, 2);
    ASSERT_EQ(root->y, 3);

    gc.safepoint_at_poll();
}

TEST(gc_test_nullptr, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);

    root->a = nullptr;
    ASSERT_EQ(root->a, nullptr);
    gc.safepoint_at_poll();
    ASSERT_EQ(root->a, nullptr);

    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);

    gc.safepoint_at_poll();
}

TEST(gc_test_nullptr, reset) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<Line>(nullptr, nullptr);

    ASSERT_EQ(line->a, nullptr);
    ASSERT_EQ(line->b, nullptr);

    {
        gccpp::Frame fr0 = gc.enter();
        auto point1 = gc.gc_alloc<Point>(2, 3);
        auto point2 = gc.gc_alloc<Point>(20, 30);

        line->a = point1;
        line->b = point2;
    }

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint_at_poll();
}

TEST(gc_test_many_safepoints, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);
    gc.safepoint_at_poll();

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);

    ASSERT_EQ(root->a->y, 3);
    ASSERT_EQ(root->b->y, 30);

    gc.safepoint_at_poll();
    gc.safepoint_at_poll();
}

TEST(gc_test_additional_scope, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    {
        gccpp::Frame fr1 = gc.enter();
        auto root = gc.gc_alloc<Line>(point1, point2);
        ASSERT_EQ(root->a->x, 2);
        ASSERT_EQ(root->b->x, 20);
        gc.safepoint_at_poll();
    }
    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);
}

TEST(gc_test_fabric_method, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = fr.root<Line>(createLine(gc));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint_at_poll();
}

TEST(gc_test_list, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        gccpp::Frame fr0 = gc.enter();
        auto last = gc.gc_alloc<List>(33, nullptr);
        auto elem1 = gc.gc_alloc<List>(22, last);
        line->next = elem1;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);

    gc.safepoint_at_poll();
}

TEST(gc_test_loop_references, creation) {
    gccpp::MarkAndSweepCollector gc;
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        gccpp::Frame fr0 = gc.enter();
        auto last = gc.gc_alloc<List>(33, nullptr);
        auto elem1 = gc.gc_alloc<List>(22, last);
        line->next = elem1;
        last->next = line;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);

    gc.safepoint_at_poll();
}

//Copying GC

TEST(copying_gc_test1, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto root = gc.gc_alloc<Point>(2, 3);

    ASSERT_EQ(root->x, 2);
    ASSERT_EQ(root->y, 3);

    gc.safepoint_at_poll();
}

TEST(copying_gc_test_nullptr, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);

    root->a = nullptr;
    ASSERT_EQ(root->a, nullptr);
    gc.safepoint_at_poll();
    ASSERT_EQ(root->a, nullptr);

    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);

    gc.safepoint_at_poll();
}

TEST(copying_gc_test_nullptr, reset) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<Line>(nullptr, nullptr);

    ASSERT_EQ(line->a, nullptr);
    ASSERT_EQ(line->b, nullptr);

    {
        gccpp::Frame fr0 = gc.enter();
        auto point1 = gc.gc_alloc<Point>(2, 3);
        auto point2 = gc.gc_alloc<Point>(20, 30);

        line->a = point1;
        line->b = point2;
    }

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint_at_poll();
}

TEST(copying_gc_test_many_safepoints, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    gc.safepoint_at_poll();
    ASSERT_EQ(root->a->x, 2);
    gc.safepoint_at_poll();
    ASSERT_EQ(root->b->x, 20);
    gc.safepoint_at_poll();

    ASSERT_EQ(root->a->x, 2);
    gc.safepoint_at_poll();
    ASSERT_EQ(root->b->x, 20);

    ASSERT_EQ(root->a->y, 3);
    gc.safepoint_at_poll();
    ASSERT_EQ(root->b->y, 30);

    gc.safepoint_at_poll();
    gc.safepoint_at_poll();
}

TEST(copying_gc_test_additional_scope, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    {
        gccpp::Frame fr1 = gc.enter();
        auto root = gc.gc_alloc<Line>(point1, point2);
        ASSERT_EQ(root->a->x, 2);
        ASSERT_EQ(root->b->x, 20);
        gc.safepoint_at_poll();
    }
    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);
}

TEST(copying_gc_test_fabric_method, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = fr.root<Line>(createLine(gc));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint_at_poll();
}

TEST(copying_gc_test_list, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        gccpp::Frame fr0 = gc.enter();
        auto last = gc.gc_alloc<List>(33, nullptr);
        auto elem1 = gc.gc_alloc<List>(22, last);
        line->next = elem1;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);

    gc.safepoint_at_poll();
}

TEST(copying_gc_test_loop_references, creation) {
    gccpp::MarkAndCompactCollector gc(1024);
    gccpp::Frame fr = gc.enter();
    gc.safepoint_at_poll();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        gccpp::Frame fr0 = gc.enter();
        auto last = gc.gc_alloc<List>(33, nullptr);
        auto elem1 = gc.gc_alloc<List>(22, last);
        line->next = elem1;
        last->next = line;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    gc.safepoint_at_poll();

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    gc.safepoint_at_poll();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}