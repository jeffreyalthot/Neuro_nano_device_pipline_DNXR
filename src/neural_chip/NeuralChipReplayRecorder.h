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
    ChipReadFrame readFrame{};
    ChipReadFrame motorReadFrame{};
    FullDiveChipFrame candidateFrame{};
    FullDiveChipFrame outputChipFrame{};
    FullDiveChipFrame committedOrHoldFrame{};
    std::vector<PipelineDecisionEvent> decisionTrace{};
    FrameCommitResult commitResult{};
    bool emergencyLatched = false;
    const char* holdReason = "";
    float sessionRisk01 = 0.0f;
    float sessionFatigue01 = 0.0f;
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
