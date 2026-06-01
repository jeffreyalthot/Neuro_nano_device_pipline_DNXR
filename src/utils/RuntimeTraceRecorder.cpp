#include "RuntimeTraceRecorder.h"

#include "TimeSync.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <system_error>

namespace dnxr {
namespace {

double finiteOrZero(double value) {
    return std::isfinite(value) ? value : 0.0;
}

float clamp01(float value) {
    if (!std::isfinite(value)) {
        return 0.0f;
    }
    return std::max(0.0f, std::min(1.0f, value));
}

std::string escapeJson(const std::string& value) {
    std::ostringstream out;
    for (unsigned char ch : value) {
        switch (ch) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (ch < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch);
                } else {
                    out << static_cast<char>(ch);
                }
                break;
        }
    }
    return out.str();
}

std::string serializeJson(const RuntimeTraceEvent& event) {
    std::ostringstream out;
    out << std::setprecision(9)
        << "{\"trace_id\":\"" << escapeJson(event.traceId) << "\","
        << "\"frame_sequence\":" << event.frameSequence << ','
        << "\"timestamp_ms\":" << event.timestampMs << ','
        << "\"module\":\"" << escapeJson(event.module) << "\","
        << "\"phase\":\"" << escapeJson(event.phase) << "\","
        << "\"latency_ms\":" << event.latencyMs << ','
        << "\"risk_score\":" << event.riskScore << ','
        << "\"degraded\":" << (event.degraded ? "true" : "false") << ','
        << "\"message\":\"" << escapeJson(event.message) << "\"}";
    return out.str();
}

} // namespace

RuntimeTraceEvent RuntimeTraceRecorder::record(
    std::uint64_t frameSequence,
    const std::string& module,
    const std::string& phase,
    double latencyMs,
    float riskScore,
    bool degraded,
    const std::string& message) {

    RuntimeTraceEvent event;
    event.frameSequence = frameSequence;
    event.traceId = makeTraceId(frameSequence, static_cast<std::uint64_t>(events_.size() + 1U));
    event.timestampMs = TimeSync::nowMs();
    event.module = module.empty() ? "runtime" : module;
    event.phase = phase.empty() ? "unspecified" : phase;
    event.latencyMs = std::max(0.0, finiteOrZero(latencyMs));
    event.riskScore = clamp01(riskScore);
    event.degraded = degraded;
    event.message = message;
    events_.push_back(event);
    return event;
}

const std::vector<RuntimeTraceEvent>& RuntimeTraceRecorder::events() const {
    return events_;
}

const RuntimeTraceEvent* RuntimeTraceRecorder::lastEvent() const {
    return events_.empty() ? nullptr : &events_.back();
}

std::string RuntimeTraceRecorder::latestTraceId() const {
    return events_.empty() ? makeTraceId(0U, 0U) : events_.back().traceId;
}

bool RuntimeTraceRecorder::saveJsonl(const std::string& path) const {
    const std::filesystem::path outputPath(path);
    if (outputPath.has_parent_path()) {
        std::error_code error;
        std::filesystem::create_directories(outputPath.parent_path(), error);
        if (error) {
            return false;
        }
    }

    std::ofstream output(outputPath, std::ios::trunc);
    if (!output) {
        return false;
    }

    for (const auto& event : events_) {
        output << serializeJson(event) << '\n';
    }
    return static_cast<bool>(output);
}

void RuntimeTraceRecorder::clear() {
    events_.clear();
}

std::string RuntimeTraceRecorder::makeTraceId(std::uint64_t frameSequence, std::uint64_t eventSequence) {
    std::ostringstream out;
    out << "trace_" << std::setw(8) << std::setfill('0') << frameSequence
        << '_' << std::setw(8) << std::setfill('0') << eventSequence;
    return out.str();
}

} // namespace dnxr
