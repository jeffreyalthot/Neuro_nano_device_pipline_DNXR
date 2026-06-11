#include "full_dive/chip_pipeline/SanitizeFailureFrameFactory.h"

#include <cmath>

namespace dnxr {
namespace {
std::uint64_t safeFrameId(std::uint64_t id) { return id == 0 ? 1 : id; }
double safeTimestamp(double timestampMs) { return std::isfinite(timestampMs) && timestampMs >= 0.0 ? timestampMs : 0.0; }
}

FullDiveChipFrame SanitizeFailureFrameFactory::makeSanitizedFailureProbe(std::uint64_t fallbackFrameId,
                                                                         double fallbackTimestampMs,
                                                                         const char*) {
    FullDiveChipFrame frame{};
    frame.frameId = safeFrameId(fallbackFrameId);
    frame.timestampMs = safeTimestamp(fallbackTimestampMs);
    frame.realityAnchor.orientation01 = 1.0f;
    frame.realityAnchor.identityContinuity01 = 1.0f;
    frame.realityAnchor.sessionBoundary01 = 1.0f;
    frame.realityAnchor.externalWorldRecall01 = 1.0f;
    frame.realityAnchor.required = true;
    frame.safety.simulationOnly = true;
    frame.safety.patientStopSignalAlive = true;
    frame.safety.emergencyCutoffAvailable = true;
    frame.safety.painMappingBlocked = true;
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.safety.aggregateRisk01 = 0.0f;
    frame.safety.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    frame.motorIntent.stopIntent01 = 1.0f;
    return frame;
}

FullDiveChipFrame SanitizeFailureFrameFactory::makeFailureProbe(const NeuralChipOnlyWorldFrame& rawWorld,
                                                                const WorldFrameSanitizationResult&, 
                                                                std::uint64_t fallbackFrameId,
                                                                double fallbackTimestampMs) {
    const std::uint64_t id = rawWorld.frameId != 0 ? rawWorld.frameId : fallbackFrameId;
    const double timestamp = std::isfinite(rawWorld.timestampMs) && rawWorld.timestampMs >= 0.0 ? rawWorld.timestampMs : fallbackTimestampMs;
    return makeSanitizedFailureProbe(id, timestamp);
}

} // namespace dnxr
