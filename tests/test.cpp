#include <gtest/gtest.h>

#include <utility>
#include "pointer/GcPtr.h"
#include "gc/Gc.h"
#include "gc/GcCollected.h"

struct Point : public tgc::GcCollected {
public:
    Point(int _x, int _y) : x(_x), y(_y) {}

    void trace(tgc::GCOperation *visitor) noexcept override {}
public:
    int x;
    int y;
};

struct Line : public tgc::GcCollected {
public:
    Line(tgc::GcPtr<Point> _a, tgc::GcPtr<Point> _b) :
            a(std::move(_a)), b(std::move(_b)) {}

    void trace(tgc::GCOperation *visitor) noexcept override {
        visitor->trace(a);
        visitor->trace(b);
    }

public:
    tgc::GcPtr<Point> a;
    tgc::GcPtr<Point> b;
};

struct List: public tgc::GcCollected {
public:
    List(int _data, tgc::GcPtr<List> _next) :
        data(_data), next(std::move(_next)) {}

    void trace(tgc::GCOperation* visitor) noexcept override {
        tgc::GcPtr<List> begin = tgc::GcPtr<List>::from<List>(this);
        tgc::GcPtr<List> current = begin;

        while (current != nullptr) {
            visitor->trace(current);
            current = current->next;
            if (current == begin) {
                break;
            }
        }
    }
public:
    int data;
    tgc::GcPtr<List> next;
};

tgc::GcPtr<Line> createLine(tgc::Gc &gc) {
    tgc::Frame fr = gc.enter();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);
    return root;
}

TEST(gc_test1, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto root = gc.gc_alloc<Point>(2, 3);

    ASSERT_EQ(root->x, 2);
    ASSERT_EQ(root->y, 3);

    gc.safepoint();
}

TEST(gc_test_nullptr, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);

    root->a = nullptr;
    ASSERT_EQ(root->a, nullptr);
    gc.safepoint();
    ASSERT_EQ(root->a, nullptr);

    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);

    gc.safepoint();
}

TEST(gc_test_nullptr, reset) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto line = gc.gc_alloc<Line>(nullptr, nullptr);

    ASSERT_EQ(line->a, nullptr);
    ASSERT_EQ(line->b, nullptr);

    {
        tgc::Frame fr0 = gc.enter();
        auto point1 = gc.gc_alloc<Point>(2, 3);
        auto point2 = gc.gc_alloc<Point>(20, 30);

        line->a = point1;
        line->b = point2;
    }

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);
    gc.safepoint();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint();
}

TEST(gc_test_many_safepoints, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    auto root = gc.gc_alloc<Line>(point1, point2);

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);
    gc.safepoint();

    ASSERT_EQ(root->a->x, 2);
    ASSERT_EQ(root->b->x, 20);

    ASSERT_EQ(root->a->y, 3);
    ASSERT_EQ(root->b->y, 30);

    gc.safepoint();
    gc.safepoint();
}

TEST(gc_test_additional_scope, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto point1 = gc.gc_alloc<Point>(2, 3);
    auto point2 = gc.gc_alloc<Point>(20, 30);
    {
        tgc::Frame fr1 = gc.enter();
        auto root = gc.gc_alloc<Line>(point1, point2);
        ASSERT_EQ(root->a->x, 2);
        ASSERT_EQ(root->b->x, 20);
        gc.safepoint();
    }
    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);
}

TEST(gc_test_fabric_method, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto line = fr.root<Line>(createLine(gc));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);
    gc.safepoint();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    gc.safepoint();
}

TEST(gc_test_list, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        tgc::Frame fr0 = gc.enter();
        auto last = gc.gc_alloc<List>(33, nullptr);
        auto elem1 = gc.gc_alloc<List>(22, last);
        line->next = elem1;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);

    gc.safepoint();
}

TEST(gc_test_loop_reference, creation) {
    tgc::Gc gc;
    tgc::Frame fr = gc.enter();
    gc.safepoint();

    auto line = gc.gc_alloc<List>(11, nullptr);
    {
        tgc::Frame fr0 = gc.enter();
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
    gc.safepoint();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}