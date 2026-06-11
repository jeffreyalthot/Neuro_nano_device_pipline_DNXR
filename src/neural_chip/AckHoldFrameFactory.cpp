#include "neural_chip/AckHoldFrameFactory.h"

namespace dnxr {

FullDiveChipFrame AckHoldFrameFactory::makeAckHoldFrame(std::uint64_t frameId, double timestampMs) const {
    FullDiveChipFrame frame{};
    frame.frameId = frameId == 0 ? 1 : frameId;
    frame.timestampMs = timestampMs < 0.0 ? 0.0 : timestampMs;
    frame.motorIntent.stopIntent01 = 1.0f;
    frame.realityAnchor = NeuralRealityAnchorVector{};
    frame.safety.simulationOnly = true;
    frame.safety.patientStopSignalAlive = true;
    frame.safety.emergencyCutoffAvailable = true;
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.safety.painMappingBlocked = true;
    frame.safety.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    frame.safety.aggregateRisk01 = 0.20f;
    return frame;
}

FullDiveChipFrame AckHoldFrameFactory::makeStateHoldFrame(const PipelineStateDecision& decision, std::uint64_t frameId, double timestampMs) const {
    FullDiveChipFrame frame = makeAckHoldFrame(frameId, timestampMs);
    if (decision.holdKind == PipelineHoldKind::Emergency || decision.holdKind == PipelineHoldKind::Shutdown) {
        frame.safety.aggregateRisk01 = 1.0f;
    }
    return frame;
}

} // namespace dnxr
