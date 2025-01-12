#pragma once
#include <gtest/gtest.h>

#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/collectors/MarkAndCompactCollector.h"
#include "gc/containers/Environment.h"

class GCTest : public testing::TestWithParam<std::string> {
public:
    void SetUp() override {
        auto testparam = GetParam();
        std::unique_ptr<gccpp::BasicCollector> gc;
        if (testparam == "markAndSweep") {
            gc = std::make_unique<gccpp::MarkAndSweepCollector>(4096 * 10);
        } else if (testparam == "markAndCompact") {
            gc = std::make_unique<gccpp::MarkAndCompactCollector>(4096 * 10);
        } else {
            assert(false); //todo
        }
        gccpp::Environment::init(std::move(gc));
        ctx = &gccpp::Environment::context();
    }

protected:
    gccpp::Environment* ctx{};
};