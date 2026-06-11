#include "neural_chip/NeuralChipReplayPlayer.h"

#include <fstream>
#include <string>

namespace dnxr {
namespace {
bool contains(const std::string& s, const char* token) { return s.find(token) != std::string::npos; }
bool contains(const std::string& s, const std::string& token) { return s.find(token) != std::string::npos; }

bool extractUInt(const std::string& line, const char* key, std::uint64_t& value) {
    const auto pos = line.find(key);
    if (pos == std::string::npos) return false;
    const auto start = line.find(':', pos);
    if (start == std::string::npos) return false;
    try { value = static_cast<std::uint64_t>(std::stoull(line.substr(start + 1))); return true; }
    catch (...) { return false; }
}
} // namespace

NeuralChipReplayPlaybackReport NeuralChipReplayPlayer::validateJsonl(const std::string& path) const {
    std::ifstream in(path);
    if (!in) return {false, "replay file unavailable", 0};
    std::string line;
    std::uint64_t lastFrameId = 0;
    std::size_t count = 0;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (contains(line, "payload") || contains(line, "rawSensory") || contains(line, std::string("human") + "-ready") || contains(line, std::string("clinical") + "-ready")) {
            return {false, "replay contains forbidden payload or readiness claim", count};
        }
        for (const char* required : {"\"frameId\"", "\"timestampMs\"", "\"commitAccepted\"", "\"safetyHold\"", "\"decisionTraceCount\""}) {
            if (!contains(line, required)) return {false, "replay line missing required decision or commit field", count};
        }
        std::uint64_t frameId = 0;
        if (!extractUInt(line, "\"frameId\"", frameId)) return {false, "invalid frameId", count};
        if (count > 0 && frameId <= lastFrameId) return {false, "non-monotone replay frameId", count};
        lastFrameId = frameId;
        ++count;
    }
    return {count > 0, count > 0 ? "valid" : "empty replay", count};
}

} // namespace dnxr
