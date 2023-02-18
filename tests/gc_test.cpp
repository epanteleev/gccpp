#include <gtest/gtest.h>

#include "pointer/GcPtr.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/collectors/MarkAndCompactCollector.h"
#include "utils/Objects.h"
#include "gc/containers/GlobalCtx.h"
#include "pointer/GcPtrImpl.h"

class GCTest : public testing::TestWithParam<std::string> {
public:
    void SetUp() override {
        auto testparam = GetParam();
        if (testparam == "markAndSweep") {
            gc = new gccpp::MarkAndSweepCollector();
        } else if (testparam == "markAndCompact") {
            gc = new gccpp::MarkAndCompactCollector(1024);
        } else {
            assert(false); //todo
        }
        ctx = gccpp::GlobalCtx::initialize(gc);
    }
    void TearDown() override {
        delete gc;
    }

protected:
    gccpp::BasicCollector* gc{};
    std::unique_ptr<gccpp::GlobalCtx> ctx;
};

TEST_P(GCTest, creation) {
    gccpp::ThreadCtx _th(*ctx);
    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    auto line = gc->alloc<Point>(2, 3);
    gccpp::Handle _h(fr, line);
    ASSERT_EQ(line->x, 2);
    ASSERT_EQ(line->y, 3);

    ctx->poll_at_safepoint();
}

TEST_P(GCTest, test_nullptr) {
    gccpp::ThreadCtx _th(*ctx);

    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    gccpp::Handle point1(fr,gc->alloc<Point>(2, 3));
    gccpp::Handle point2(fr,gc->alloc<Point>(20, 30));
    gccpp::Handle line(fr, gc->alloc<Line>(point1, point2));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    line->a = nullptr;
    ASSERT_EQ(line->a, nullptr);
    ctx->poll_at_safepoint();
    ASSERT_EQ(line->a, nullptr);

    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);

    ctx->poll_at_safepoint();
}

TEST_P(GCTest, additional_scope) {
    gccpp::ThreadCtx _th(*ctx);

    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    gccpp::Handle line(fr, gc->alloc<Line>(nullptr, nullptr));

    ASSERT_EQ(line->a, nullptr);
    ASSERT_EQ(line->b, nullptr);

    {
        gccpp::HandleMark fr(*ctx);
        gccpp::Handle point1(fr, gc->alloc<Point>(2, 3));
        gccpp::Handle point2(fr,gc->alloc<Point>(20, 30));

        line->a = point1;
        line->b = point2;
    }

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);
    ctx->poll_at_safepoint();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    ctx->poll_at_safepoint();
}

TEST_P(GCTest, many_safepoints) {
    gccpp::ThreadCtx _th(*ctx);

    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    auto point1 = gc->alloc<Point>(2, 3);
    auto point2 = gc->alloc<Point>(20, 30);
    gccpp::Handle root(fr,gc->alloc<Line>(point1, point2));

    ctx->poll_at_safepoint();
    ASSERT_EQ(root->a->x, 2);
    ctx->poll_at_safepoint();
    ASSERT_EQ(root->b->x, 20);
    ctx->poll_at_safepoint();

    ASSERT_EQ(root->a->x, 2);
    ctx->poll_at_safepoint();
    ASSERT_EQ(root->b->x, 20);
    ctx->poll_at_safepoint();
    ASSERT_EQ(root->a->y, 3);
    ctx->poll_at_safepoint();
    ASSERT_EQ(root->b->y, 30);

    ctx->poll_at_safepoint();
    ctx->poll_at_safepoint();
}

TEST_P(GCTest, fabric_method) {
    gccpp::ThreadCtx _th(*ctx);

    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    gccpp::Handle line(fr, createLine(*gc));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);
    ctx->poll_at_safepoint();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    ctx->poll_at_safepoint();
}

TEST_P(GCTest, test_list) {
    gccpp::ThreadCtx _th(*ctx);
    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    gccpp::Handle line(fr, gc->alloc<List>(11, nullptr));
    {
        gccpp::HandleMark _fr(*ctx);
        auto last = gc->alloc<List>(33, nullptr);
        auto elem1 = gc->alloc<List>(22, last);
        line->next = elem1;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);

    ctx->poll_at_safepoint();
}

TEST_P(GCTest, loop_references) {
    gccpp::ThreadCtx _th(*ctx);

    gccpp::HandleMark fr(*ctx);
    ctx->poll_at_safepoint();

    auto _line = gc->alloc<List>(11, nullptr);
    gccpp::Handle line(fr, _line);
    {
        gccpp::HandleMark fr0(*ctx);
        auto last = gc->alloc<List>(33, nullptr);
        auto elem1 = gc->alloc<List>(22, last);
        line->next = elem1;
        last->next = line;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    ctx->poll_at_safepoint();

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    ctx->poll_at_safepoint();
}

INSTANTIATE_TEST_SUITE_P(gc_testing,
                         GCTest,
                         testing::Values("markAndSweep", "markAndCompact"));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}