#pragma once

#include "../utils/GCTest.h"

TEST_P(GCTest, creation) {
    gccpp::ThreadEnv _th(*ctx);
    gccpp::HandleMark fr;
    ctx->force_gc();

    auto line = ctx->alloc<Point>(2, 3);
    gccpp::Handle _h(line);
    ASSERT_EQ(line->x, 2);
    ASSERT_EQ(line->y, 3);

    ctx->force_gc();
}

TEST_P(GCTest, test_nullptr) {
    gccpp::ThreadEnv _th(*ctx);

    gccpp::HandleMark fr;
    ctx->force_gc();

    gccpp::Handle point1(ctx->alloc<Point>(2, 3));
    gccpp::Handle point2(ctx->alloc<Point>(20, 30));
    gccpp::Handle line(ctx->alloc<Line>(point1, point2));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    line->a = nullptr;
    ASSERT_EQ(line->a, nullptr);
    ctx->force_gc();
    ASSERT_EQ(line->a, nullptr);

    ASSERT_EQ(point1->x, 2);
    ASSERT_EQ(point1->y, 3);

    ASSERT_EQ(point2->x, 20);
    ASSERT_EQ(point2->y, 30);

    ctx->force_gc();
}

TEST_P(GCTest, additional_scope) {
    gccpp::ThreadEnv _th(*ctx);

    gccpp::HandleMark fr;
    ctx->force_gc();

    gccpp::Handle line(ctx->alloc<Line>(nullptr, nullptr));

    ASSERT_EQ(line->a, nullptr);
    ASSERT_EQ(line->b, nullptr);

    {
        gccpp::HandleMark fr1;
        gccpp::Handle point1(ctx->alloc<Point>(2, 3));
        gccpp::Handle point2(ctx->alloc<Point>(20, 30));

        line->a = point1;
        line->b = point2;
    }

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);
    ctx->force_gc();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    ctx->force_gc();
}

TEST_P(GCTest, many_safepoints) {
    gccpp::ThreadEnv _th(*ctx);

    gccpp::HandleMark fr;
    ctx->force_gc();

    auto point1 = ctx->alloc<Point>(2, 3);
    auto point2 = ctx->alloc<Point>(20, 30);
    gccpp::Handle root(ctx->alloc<Line>(point1, point2));

    ctx->force_gc();
    ASSERT_EQ(root->a->x, 2);
    ctx->force_gc();
    ASSERT_EQ(root->b->x, 20);
    ctx->force_gc();

    ASSERT_EQ(root->a->x, 2);
    ctx->force_gc();
    ASSERT_EQ(root->b->x, 20);
    ctx->force_gc();
    ASSERT_EQ(root->a->y, 3);
    ctx->force_gc();
    ASSERT_EQ(root->b->y, 30);

    ctx->force_gc();
    ctx->force_gc();
}

TEST_P(GCTest, fabric_method) {
    gccpp::ThreadEnv _th(*ctx);

    gccpp::HandleMark fr;
    ctx->force_gc();

    gccpp::Handle line(Line::createLine(*ctx));

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);
    ctx->force_gc();

    ASSERT_EQ(line->a->x, 2);
    ASSERT_EQ(line->b->x, 20);

    ASSERT_EQ(line->a->y, 3);
    ASSERT_EQ(line->b->y, 30);

    ctx->force_gc();
}

TEST_P(GCTest, test_list) {
    gccpp::ThreadEnv _th(*ctx);
    gccpp::HandleMark fr;
    ctx->force_gc();

    gccpp::Handle line(ctx->alloc<List>(11, nullptr));
    {
        gccpp::HandleMark _fr;
        auto last = ctx->alloc<List>(33, nullptr);
        auto elem1 = ctx->alloc<List>(22, last);
        line->next = elem1;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);

    ctx->force_gc();
}

TEST_P(GCTest, loop_references) {
    gccpp::ThreadEnv _th(*ctx);

    gccpp::HandleMark fr;
    ctx->force_gc();

    auto _line = ctx->alloc<List>(11, nullptr);
    gccpp::Handle line(_line);
    {
        gccpp::HandleMark fr0;
        auto last = ctx->alloc<List>(33, nullptr);
        auto elem1 = ctx->alloc<List>(22, last);
        line->next = elem1;
        last->next = line;
    }

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    ctx->force_gc();

    ASSERT_EQ(line->data, 11);
    ASSERT_EQ(line->next->data, 22);
    ASSERT_EQ(line->next->next->data, 33);
    ASSERT_EQ(line->next->next->next->data, 11);
    ASSERT_EQ(line->next->next->next->next->data, 22);
    ctx->force_gc();
}

INSTANTIATE_TEST_SUITE_P(gc_testing,
                         GCTest,
                         testing::Values("markAndSweep", "markAndCompact"));