#include "ReplayPlayer.h"

#include <fstream>
#include <utility>

namespace dnxr {

bool ReplayPlayer::loadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return false;
    }

    std::string header;
    if (!std::getline(input, header) || header != "dnxr_replay_v1") {
        return false;
    }

    std::vector<RuntimeFrame> loadedFrames;
    std::vector<std::string> loadedHashes;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        const auto separator = line.find('|');
        if (separator == std::string::npos) {
            return false;
        }

        const std::string expectedHash = line.substr(0U, separator);
        bool ok = false;
        RuntimeFrame frame = parseRuntimeFrame(line.substr(separator + 1U), ok);
        if (!ok || runtimeFrameHash(frame) != expectedHash) {
            return false;
        }
        loadedHashes.push_back(expectedHash);
        loadedFrames.push_back(frame);
    }

    frames_ = std::move(loadedFrames);
    frameHashes_ = std::move(loadedHashes);
    cursor_ = 0U;
    return true;
}

bool ReplayPlayer::hasNext() const {
    return cursor_ < frames_.size();
}

RuntimeFrame ReplayPlayer::next() {
    return hasNext() ? frames_[cursor_++] : RuntimeFrame{};
}

void ReplayPlayer::reset() {
    cursor_ = 0U;
}

std::size_t ReplayPlayer::size() const {
    return frames_.size();
}

const std::vector<RuntimeFrame>& ReplayPlayer::frames() const {
    return frames_;
}

const std::vector<std::string>& ReplayPlayer::frameHashes() const {
    return frameHashes_;
}

} // namespace dnxr
