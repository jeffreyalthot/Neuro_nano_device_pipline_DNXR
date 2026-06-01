#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeTraceEvent {
    std::string traceId;
    std::uint64_t frameSequence = 0;
    std::int64_t timestampMs = 0;
    std::string module;
    std::string phase;
    double latencyMs = 0.0;
    float riskScore = 0.0f;
    bool degraded = false;
    std::string message;
};

class RuntimeTraceRecorder {
public:
    RuntimeTraceEvent record(
        std::uint64_t frameSequence,
        const std::string& module,
        const std::string& phase,
        double latencyMs,
        float riskScore,
        bool degraded,
        const std::string& message);

    const std::vector<RuntimeTraceEvent>& events() const;
    const RuntimeTraceEvent* lastEvent() const;
    std::string latestTraceId() const;
    bool saveJsonl(const std::string& path) const;
    void clear();

    static std::string makeTraceId(std::uint64_t frameSequence, std::uint64_t eventSequence);

private:
    std::vector<RuntimeTraceEvent> events_;
};

} // namespace dnxr
