#include "neural_chip/NeuralChipSessionReport.h"

#include <algorithm>

namespace dnxr {

NeuralChipSessionReport NeuralChipSessionReportBuilder::build(const NeuralChipSessionState& state, const NeuralChipReplayRecorder& recorder) {
    NeuralChipSessionReport out;
    out.sessionId = state.sessionId;
    out.acceptedFrames = state.acceptedFrames;
    out.rejectedFrames = state.rejectedFrames;
    out.holdFrames = state.holdFrames;
    out.maxRisk01 = state.sessionRisk01;
    out.maxFatigue01 = state.sessionFatigue01;
    out.mostReducedModality = "unknown";
    for (const auto& record : recorder.records()) {
        out.maxRisk01 = std::max(out.maxRisk01, record.sessionRisk01);
        out.maxFatigue01 = std::max(out.maxFatigue01, record.sessionFatigue01);
        out.sessionDigestHash = record.sessionDigestHash;
        out.sessionDigestFrameCount = record.sessionDigestFrameCount;
        if (record.emergencyLatched) ++out.latchCount;
        for (const auto& event : record.decisionTrace) {
            const std::string stage = event.stage ? event.stage : "";
            const std::string modality = event.modality ? event.modality : "";
            const std::string rule = event.rule ? event.rule : "";
            if (modality.find("olfactory") != std::string::npos || modality.find("gustatory") != std::string::npos) ++out.chemicalHoldCount;
            if (modality.find("realityAnchor") != std::string::npos || rule.find("reality anchor") != std::string::npos) ++out.realityAnchorFailureCount;
            out.lastDecisionSummaries.push_back(stage + ":" + modality + ":" + rule);
            if (out.lastDecisionSummaries.size() > 10) out.lastDecisionSummaries.erase(out.lastDecisionSummaries.begin());
        }
    }
    return out;
}

} // namespace dnxr
