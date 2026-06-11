#include "neural_chip/NeuralChipBus.h"

namespace dnxr {

ChipReadFrame NeuralChipBus::readNeuralActivity() const {
    return readFrame_;
}

bool NeuralChipBus::submitSensoryFrame(const FullDiveChipFrame& frame) {
    health_.connected = true;
    health_.safeHoldActive = false;
    health_.lastFrameAccepted = isSafeForChip(frame);
    if (!health_.lastFrameAccepted) {
        health_.safeHoldActive = true;
        return false;
    }

    lastFrame_ = frame;
    readFrame_.frameId = frame.frameId;
    readFrame_.motorIntent = frame.motorIntent;
    readFrame_.signalQuality01 = frame.motorIntent.confidence01;
    return true;
}

bool NeuralChipBus::submitSafetyHold(const NeuralSafetyFrame& safety) {
    health_.connected = true;
    health_.safeHoldActive = safety.holdActive;
    health_.lastFrameAccepted = false;
    readFrame_.frameId = safety.frameId;
    return true;
}

ChipHealthStatus NeuralChipBus::getHealthStatus() const {
    return health_;
}

const FullDiveChipFrame& NeuralChipBus::lastAcceptedFrame() const {
    return lastFrame_;
}

bool NeuralChipBus::isSafeForChip(const FullDiveChipFrame& frame) {
    return frame.safety.simulationOnly &&
           frame.safety.rawSensoryPayloadBlocked &&
           frame.safety.painMappingBlocked &&
           frame.safety.patientStopSignalAlive &&
           frame.safety.emergencyCutoffAvailable &&
           frame.safety.activeCeiling01 <= DNXR_NEURAL_CHIP_ACTIVE_CEILING_01 &&
           frame.touch.painBlocked01 >= 1.0f &&
           frame.touch.burnBlocked01 >= 1.0f &&
           frame.touch.crushBlocked01 >= 1.0f &&
           frame.touch.shockBlocked01 >= 1.0f &&
           frame.realityAnchor.required &&
           frame.realityAnchor.externalWorldRecall01 > 0.0f;
}

} // namespace dnxr
