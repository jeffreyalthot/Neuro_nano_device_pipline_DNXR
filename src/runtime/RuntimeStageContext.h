#pragma once

#include <cstdint>

namespace dnxr {

struct RuntimeStageContext {
    std::uint64_t cycleId = 0;
    double timestamp = 0.0;
    double deltaTimeSeconds = 0.0;
};

} // namespace dnxr
