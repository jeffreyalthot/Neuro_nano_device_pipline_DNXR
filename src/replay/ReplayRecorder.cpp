#include "ReplayRecorder.h"

#include <filesystem>
#include <fstream>

namespace dnxr {

void ReplayRecorder::record(const RuntimeFrame& frame) {
    frames_.push_back(frame);
}

bool ReplayRecorder::saveToFile(const std::string& path) const {
    const std::filesystem::path outputPath(path);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        return false;
    }

    output << "dnxr_replay_v1\n";
    for (const auto& frame : frames_) {
        output << runtimeFrameHash(frame) << '|' << serializeRuntimeFrame(frame) << '\n';
    }
    return static_cast<bool>(output);
}

const std::vector<RuntimeFrame>& ReplayRecorder::frames() const {
    return frames_;
}

void ReplayRecorder::clear() {
    frames_.clear();
}

} // namespace dnxr
