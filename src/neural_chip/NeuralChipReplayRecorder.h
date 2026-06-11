#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralChipPipelineDecisionTrace.h"

#include <string>
#include <vector>

namespace dnxr {

struct NeuralChipReplayRecord {
    std::uint64_t frameId = 0;
    double timestampMs = 0.0;
    std::string inputWorldSummary;
    ChipReadFrame motorReadFrame{};
    FullDiveChipFrame outputChipFrame{};
    std::vector<PipelineDecisionEvent> decisionTrace{};
    FrameCommitResult commitResult{};
    const char* holdReason = "";
};

class NeuralChipReplayRecorder {
public:
    void record(const NeuralChipReplayRecord& record);
    const std::vector<NeuralChipReplayRecord>& records() const;
    void clear();

private:
    std::vector<NeuralChipReplayRecord> records_{};
};

} // namespace dnxr
