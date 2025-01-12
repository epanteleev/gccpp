#pragma once

#include <gtest/gtest.h>
#include "../utils/GCTest.h"
#include "../utils/Objects.h"
#include "struct/Array.h"

TEST_P(GCTest, two_threads) {
    gccpp::ThreadEnv _th(*ctx);
    gccpp::HandleMark fr;

    auto work = [&] {
        gccpp::ThreadEnv _th(*ctx);
        gccpp::HandleMark fr;

        ctx->force_gc();

        auto point = ctx->alloc<Point>(2, 3);
        gccpp::Handle h(point);

        ASSERT_EQ(point->x, 2);
        ASSERT_EQ(point->y, 3);

        ctx->force_gc();
    };
    std::thread thread(work);
    ctx->force_gc();

    auto point = ctx->alloc<Point>(2, 3);
    gccpp::Handle h(point);

    ASSERT_EQ(point->x, 2);
    ASSERT_EQ(point->y, 3);

    ctx->force_gc();
    ctx->unmanaged_context([&] {
        thread.join();
    });
    ctx->force_gc();
}

TEST_P(GCTest, ref_leak_from_thread) {
    gccpp::ThreadEnv _th(*ctx);
    gccpp::HandleMark fr;

    gccpp::Handle<Point> leak(nullptr);

    auto work = [&] {
        gccpp::ThreadEnv _th(*ctx);
        gccpp::HandleMark fr;

        ctx->force_gc();
        ctx->safepoint();
        auto point = ctx->alloc<Point>(2, 3);
        gccpp::Handle h(point);

        ASSERT_EQ(point->x, 2);
        ASSERT_EQ(point->y, 3);

        leak = point;
        ctx->force_gc();
    };
    std::thread thread(work);
    ctx->unmanaged_context([&] {
        thread.join();
    });

    ASSERT_EQ(leak->x, 2);
    ASSERT_EQ(leak->y, 3);
}

TEST_P(GCTest, fill_array) {
    gccpp::ThreadEnv _th(*ctx);
    gccpp::HandleMark fr;

    std::size_t size = 128;
    gccpp::Handle<Array<Point>> array(Array<Point>::make(size));

    auto work = [&] {
        gccpp::ThreadEnv _th(*ctx);
        gccpp::HandleMark fr;

        ctx->force_gc();
        ctx->safepoint();

        for (std::size_t i = 64; i < size; i++) {
            auto point = ctx->alloc<Point>(i, 3);
            array->at(i) = point;
        }
        ctx->force_gc();
        ctx->safepoint();
    };

    std::thread thread(work);

    for (std::size_t i = 0; i < 64; i++) {
        auto point = ctx->alloc<Point>(i, 3);
        array->at(i) = point;
    }

    ctx->unmanaged_context([&] {
        thread.join();
    });

    ctx->force_gc();
    ctx->safepoint();
    for (std::size_t i = 0; i < size; i++) {
        ASSERT_EQ(i, array->at(i)->x);
    }
    ctx->force_gc();
    for (std::size_t i = 0; i < size; i++) {
        ASSERT_EQ(i, array->at(i)->x);
    }
}

INSTANTIATE_TEST_SUITE_P(Multithread,
                         GCTest,
                         testing::Values("markAndSweep", "markAndCompact"));