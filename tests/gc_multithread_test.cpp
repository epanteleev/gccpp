#include <gtest/gtest.h>

#include "pointer/GcPtr.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/collectors/MarkAndCompactCollector.h"
#include "utils/Objects.h"
#include "gc/containers/GlobalCtx.h"

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
    gccpp::HandleMark fr(_th);

    auto work = [&] {
        gccpp::ThreadCtx _th(*ctx);
        gccpp::HandleMark fr(_th);

        ctx->poll_at_safepoint();

        auto point = gc->alloc<Point>(2, 3);
        gccpp::Handle h(fr, point);

        ASSERT_EQ(point->x, 2);
        ASSERT_EQ(point->y, 3);

        ctx->poll_at_safepoint();
    };
    std::thread thread(work);
    ctx->poll_at_safepoint();

    auto point = gc->alloc<Point>(2, 3);
    gccpp::Handle h(fr, point);

    ASSERT_EQ(point->x, 2);
    ASSERT_EQ(point->y, 3);

    ctx->poll_at_safepoint();
    ctx->call_unmanaged([&] {
        thread.join();
    });
    ctx->poll_at_safepoint();
}

INSTANTIATE_TEST_SUITE_P(Multithread,
                         GCTest,
                         testing::Values("markAndSweep", "markAndCompact"));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}