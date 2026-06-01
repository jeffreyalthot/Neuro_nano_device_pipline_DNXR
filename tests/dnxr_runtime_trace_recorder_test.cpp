#include "../src/utils/RuntimeTraceRecorder.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_runtime_trace_recorder_test: " << message << '\n';
        return false;
    }
    return true;
}

} // namespace

int main() {
    dnxr::RuntimeTraceRecorder recorder;
    const auto first = recorder.record(7U, "safety", "evaluate", 1.25, 0.40F, false, "nominal");
    const auto second = recorder.record(7U, "render", "handoff", std::nan(""), 2.0F, true, "degraded \"frame\"");

    if (!require(first.traceId == "trace_00000007_00000001", "first trace id should be deterministic") ||
        !require(second.traceId == "trace_00000007_00000002", "second trace id should increment") ||
        !require(recorder.events().size() == 2U, "recorder should contain two events") ||
        !require(second.latencyMs == 0.0, "non-finite latency should be neutralized") ||
        !require(second.riskScore == 1.0F, "risk score should clamp to one") ||
        !require(recorder.latestTraceId() == second.traceId, "latest trace id should match last event")) {
        return 1;
    }

    const std::string path = "build/dnxr_runtime_trace_recorder_test.jsonl";
    if (!require(recorder.saveJsonl(path), "trace recorder should save jsonl")) {
        return 1;
    }

    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    const std::string contents = buffer.str();
    std::remove(path.c_str());

    if (!require(contents.find("\"trace_id\":\"trace_00000007_00000001\"") != std::string::npos,
                 "jsonl should include trace id") ||
        !require(contents.find("degraded \\\"frame\\\"") != std::string::npos,
                 "jsonl should escape quotes") ||
        !require(contents.find("\"degraded\":true") != std::string::npos,
                 "jsonl should encode degraded flag")) {
        return 1;
    }

    recorder.clear();
    if (!require(recorder.events().empty(), "clear should remove events") ||
        !require(recorder.latestTraceId() == "trace_00000000_00000000", "empty latest trace id should be stable")) {
        return 1;
    }

    return 0;
}
