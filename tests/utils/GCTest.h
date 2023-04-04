#pragma once
#include <gtest/gtest.h>

#include "pointer/Oop.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/collectors/MarkAndCompactCollector.h"
#include "gc/containers/Enviroment.h"

#include "struct/Array.h"
#include "Objects.h"

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
        gccpp::Enviroment::init(std::move(gc));
        ctx = &gccpp::Enviroment::context();
    }

protected:
    gccpp::Enviroment* ctx{};
};