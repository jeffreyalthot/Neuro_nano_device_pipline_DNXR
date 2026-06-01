#include "FileReplaySignalProvider.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

const std::vector<float>& fallbackFrame() {
    static const std::vector<float> frame = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};
    return frame;
}

std::string trim(const std::string& value) {
    auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();
    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

bool parseFrame(const std::string& line, std::vector<float>& frame) {
    const std::string normalizedLine = trim(line);
    if (normalizedLine.empty() || normalizedLine.front() == '#') {
        return false;
    }

    std::vector<float> parsed;
    std::stringstream stream(normalizedLine);
    std::string token;
    while (std::getline(stream, token, ',')) {
        token = trim(token);
        if (token.empty()) {
            return false;
        }

        std::stringstream valueStream(token);
        float value = 0.0f;
        valueStream >> value;
        if (!valueStream || !valueStream.eof() || !std::isfinite(value)) {
            return false;
        }
        parsed.push_back(value);
    }

    if (parsed.empty()) {
        return false;
    }
    frame = std::move(parsed);
    return true;
}

} // namespace

FileReplaySignalProvider::FileReplaySignalProvider(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        Logger::warn("[FileReplaySignalProvider] Could not open replay signal file: " + path);
        useFallbackFrame("missing_file");
        return;
    }

    std::size_t invalidLineCount = 0U;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<float> frame;
        if (parseFrame(line, frame)) {
            frames_.push_back(std::move(frame));
        } else if (!trim(line).empty() && trim(line).front() != '#') {
            ++invalidLineCount;
        }
    }

    if (invalidLineCount > 0U) {
        Logger::warn("[FileReplaySignalProvider] Ignored invalid replay signal lines: " + std::to_string(invalidLineCount));
    }
    if (frames_.empty()) {
        useFallbackFrame("empty_or_invalid_file");
    }
}

std::vector<float> FileReplaySignalProvider::nextSignal(int cycle) {
    if (frames_.empty()) {
        useFallbackFrame("empty_runtime_frame_cache");
    }

    const auto safeCycle = cycle < 0 ? 0U : static_cast<std::size_t>(cycle);
    const auto index = safeCycle % frames_.size();
    return frames_[index];
}

std::string FileReplaySignalProvider::name() const {
    return "file_replay";
}

std::size_t FileReplaySignalProvider::frameCount() const {
    return frames_.size();
}

bool FileReplaySignalProvider::usingFallback() const {
    return usingFallback_;
}

void FileReplaySignalProvider::useFallbackFrame(const std::string& reason) {
    frames_.clear();
    frames_.push_back(fallbackFrame());
    usingFallback_ = true;
    Logger::warn("[FileReplaySignalProvider] Using fallback replay frame: " + reason);
}

} // namespace dnxr
