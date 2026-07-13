#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralVectorValidator.h"

namespace dnxr {

ChipReadFrame NeuralChipBus::readNeuralActivity() const {
    return readFrame_;
}

ChipReadFrame NeuralChipBus::readClosedLoopFeedback() const {
    return feedbackFrame_;
}

bool NeuralChipBus::submitSensoryFrame(const FullDiveChipFrame& frame) {
    health_.connected = true;
    const auto validation = NeuralVectorValidator::validate(frame);
    if (!validation.accepted) {
        submitSafetyHold({frame.frameId, true, validation.reason});
        return false;
    }

    health_.safeHoldActive = false;
    health_.lastFrameAccepted = isSafeForChip(frame);
    if (!health_.lastFrameAccepted) {
        submitSafetyHold({frame.frameId, true, "chip safety rules failed"});
        return false;
    }
    health_.lastFrameAccepted = true;

    lastFrame_ = frame;
    return true;
}

void NeuralChipBus::injectSimulatedMotorReadFrame(const ChipReadFrame& frame) {
    readFrame_ = frame;
}

void NeuralChipBus::injectSimulatedFeedbackFrame(const ChipReadFrame& frame) {
    feedbackFrame_ = frame;
}

bool NeuralChipBus::submitSafetyHold(const NeuralSafetyFrame& safety) {
    health_.connected = true;
    health_.safeHoldActive = safety.holdActive;
    health_.lastFrameAccepted = false;
    lastSafetyHold_ = safety;
    return true;
}

ChipHealthStatus NeuralChipBus::getHealthStatus() const {
    return health_;
}

const FullDiveChipFrame& NeuralChipBus::lastAcceptedFrame() const {
    return lastFrame_;
}

const NeuralSafetyFrame& NeuralChipBus::lastSafetyHold() const {
    return lastSafetyHold_;
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
