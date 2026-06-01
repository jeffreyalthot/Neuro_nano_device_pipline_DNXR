#pragma once

#include "../utils/RuntimeMetrics.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeFrame {
    std::uint64_t cycleId = 0;
    std::int64_t timestampMs = 0;
    std::vector<float> rawSignal;
    std::vector<float> cleanSignal;
    std::vector<float> features;
    std::string cognitiveState = "unknown";
    std::vector<float> biometrics;
    std::string xrAction = "none";
    std::vector<float> xrFrame;
    std::string safetyDecision = "unknown";
    RuntimeMetricsSnapshot metricsSnapshot{};
};

std::string runtimeFrameHash(const RuntimeFrame& frame);
std::string serializeRuntimeFrame(const RuntimeFrame& frame);
RuntimeFrame parseRuntimeFrame(const std::string& line, bool& ok);

} // namespace dnxr
