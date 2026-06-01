#include "RuntimeFrame.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {

std::uint64_t fnv1a64(const std::string& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::string encodeString(const std::string& value) {
    std::ostringstream out;
    out << std::hex << std::uppercase;
    for (unsigned char ch : value) {
        if (ch == '%' || ch == '|' || ch == ',' || ch == '\n' || ch == '\r') {
            out << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
        } else {
            out << static_cast<char>(ch);
        }
    }
    return out.str();
}

int hexValue(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
    if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
    return -1;
}

std::string decodeString(const std::string& value) {
    std::string out;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2U < value.size()) {
            const int hi = hexValue(value[i + 1U]);
            const int lo = hexValue(value[i + 2U]);
            if (hi >= 0 && lo >= 0) {
                out.push_back(static_cast<char>((hi << 4) | lo));
                i += 2U;
                continue;
            }
        }
        out.push_back(value[i]);
    }
    return out;
}

std::string serializeVector(const std::vector<float>& values) {
    std::ostringstream out;
    out << std::setprecision(9);
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0U) {
            out << ',';
        }
        out << values[i];
    }
    return out.str();
}

std::vector<float> parseVector(const std::string& value, bool& ok) {
    std::vector<float> out;
    if (value.empty()) {
        return out;
    }

    std::stringstream stream(value);
    std::string token;
    while (std::getline(stream, token, ',')) {
        try {
            std::size_t parsedCharacters = 0U;
            const float parsedValue = std::stof(token, &parsedCharacters);
            if (parsedCharacters != token.size() || !std::isfinite(parsedValue)) {
                ok = false;
                return {};
            }
            out.push_back(parsedValue);
        } catch (...) {
            ok = false;
            return {};
        }
    }
    return out;
}

bool parseFiniteDouble(const std::string& token, double& out) {
    std::size_t parsedCharacters = 0U;
    const double parsedValue = std::stod(token, &parsedCharacters);
    if (parsedCharacters != token.size() || !std::isfinite(parsedValue)) {
        return false;
    }
    out = parsedValue;
    return true;
}

std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

} // namespace

std::string runtimeFrameHash(const RuntimeFrame& frame) {
    const auto digest = fnv1a64(serializeRuntimeFrame(frame));
    std::ostringstream out;
    out << "frame_" << std::hex << std::setw(16) << std::setfill('0') << digest;
    return out.str();
}

std::string serializeRuntimeFrame(const RuntimeFrame& frame) {
    std::ostringstream out;
    out << std::setprecision(9)
        << frame.cycleId << '|'
        << frame.timestampMs << '|'
        << serializeVector(frame.rawSignal) << '|'
        << serializeVector(frame.cleanSignal) << '|'
        << serializeVector(frame.features) << '|'
        << encodeString(frame.cognitiveState) << '|'
        << serializeVector(frame.biometrics) << '|'
        << encodeString(frame.xrAction) << '|'
        << serializeVector(frame.xrFrame) << '|'
        << encodeString(frame.safetyDecision) << '|'
        << frame.metricsSnapshot.totalCycles << '|'
        << frame.metricsSnapshot.safetyEventCount << '|'
        << frame.metricsSnapshot.lastLatencyMs << '|'
        << frame.metricsSnapshot.avgLatencyMs << '|'
        << frame.metricsSnapshot.jitterMs << '|'
        << frame.metricsSnapshot.p95LatencyMs << '|'
        << frame.metricsSnapshot.p99LatencyMs << '|'
        << runtimeLatencyHealthToString(frame.metricsSnapshot.latencyHealth);
    return out.str();
}

RuntimeFrame parseRuntimeFrame(const std::string& line, bool& ok) {
    ok = false;
    RuntimeFrame frame;
    const auto fields = splitFields(line);
    if (fields.size() != 18U) {
        return frame;
    }

    try {
        bool vectorsOk = true;
        frame.cycleId = static_cast<std::uint64_t>(std::stoull(fields[0]));
        frame.timestampMs = static_cast<std::int64_t>(std::stoll(fields[1]));
        frame.rawSignal = parseVector(fields[2], vectorsOk);
        frame.cleanSignal = parseVector(fields[3], vectorsOk);
        frame.features = parseVector(fields[4], vectorsOk);
        frame.cognitiveState = decodeString(fields[5]);
        frame.biometrics = parseVector(fields[6], vectorsOk);
        frame.xrAction = decodeString(fields[7]);
        frame.xrFrame = parseVector(fields[8], vectorsOk);
        frame.safetyDecision = decodeString(fields[9]);
        frame.metricsSnapshot.totalCycles = static_cast<std::uint64_t>(std::stoull(fields[10]));
        frame.metricsSnapshot.safetyEventCount = static_cast<std::uint64_t>(std::stoull(fields[11]));
        if (!parseFiniteDouble(fields[12], frame.metricsSnapshot.lastLatencyMs) ||
            !parseFiniteDouble(fields[13], frame.metricsSnapshot.avgLatencyMs) ||
            !parseFiniteDouble(fields[14], frame.metricsSnapshot.jitterMs) ||
            !parseFiniteDouble(fields[15], frame.metricsSnapshot.p95LatencyMs) ||
            !parseFiniteDouble(fields[16], frame.metricsSnapshot.p99LatencyMs)) {
            return frame;
        }
        if (fields[17] == "throttle") {
            frame.metricsSnapshot.latencyHealth = RuntimeLatencyHealth::Throttle;
        } else if (fields[17] == "fallback") {
            frame.metricsSnapshot.latencyHealth = RuntimeLatencyHealth::Fallback;
        } else {
            frame.metricsSnapshot.latencyHealth = RuntimeLatencyHealth::Stable;
        }
        ok = vectorsOk;
    } catch (...) {
        ok = false;
    }

    return frame;
}

} // namespace dnxr
