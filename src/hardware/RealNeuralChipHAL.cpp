#include "NeuralChipHAL.h"

#include "../clinical/ClinicalModeGate.h"

#include <stdexcept>

namespace dnxr {

RealNeuralChipHAL::RealNeuralChipHAL()
    : connected_(false), emergencyLatched_(true) {
    status_.connected = false;
    status_.simulation = false;
    status_.emergencyLatched = true;
    status_.statusMessage = "clinical_real_hal_locked_no_human_use";
}

void RealNeuralChipHAL::connect(const std::string& deviceIdentifier) {
    if (deviceIdentifier.empty()) {
        throw std::invalid_argument("RealNeuralChipHAL device identifier missing");
    }
    ClinicalModeGate gate;
    if (!gate.canConnectClinicalDevice({}, 0)) {
        connected_ = false;
        emergencyLatched_ = true;
        status_.connected = false;
        status_.emergencyLatched = true;
        status_.statusMessage = "clinical_mode_gate_denied_connection";
        throw std::runtime_error("RealNeuralChipHAL is locked until the complete signed clinical evidence chain is verified");
    }
}

void RealNeuralChipHAL::disconnect() {
    connected_ = false;
    status_.connected = false;
    status_.statusMessage = "disconnected_locked";
}

NeuralChipFrame RealNeuralChipHAL::readFrame() {
    NeuralChipFrame frame;
    frame.simulation = false;
    return frame;
}

bool RealNeuralChipHAL::writeSafeStimulus(const SafeNeuralStimulusCommand& command) {
    (void)command;
    status_.statusMessage = "stimulus_rejected_by_real_hal_clinical_lock";
    emergencyLatched_ = true;
    status_.emergencyLatched = true;
    return false;
}

void RealNeuralChipHAL::emergencyStop(const std::string& reason) {
    connected_ = false;
    emergencyLatched_ = true;
    status_.connected = false;
    status_.emergencyLatched = true;
    status_.statusMessage = "emergency_stop:" + reason;
}

NeuralChipStatus RealNeuralChipHAL::status() const {
    NeuralChipStatus s = status_;
    s.connected = connected_;
    s.emergencyLatched = emergencyLatched_;
    s.simulation = false;
    return s;
}

} // namespace dnxr

namespace dnxr {

NeuralChipAck RealNeuralChipHAL::lastAck() const {
    return lastAck_;
}

bool RealNeuralChipHAL::requiresClosedLoopFeedback() const {
    return true;
}

} // namespace dnxr

namespace dnxr {

ChipCapabilities RealNeuralChipHAL::capabilities() const {
    ChipCapabilities c;
    c.maxSafeRateHz = 0.0f;
    c.supportsHardwareCutoff = false;
    c.supportsClosedLoopFeedback = false;
    return c;
}

DeviceIdentity RealNeuralChipHAL::deviceIdentity() const {
    DeviceIdentity id;
    id.deviceIdHash = "real_hal_locked_no_identity";
    id.firmwareHash = "unattested";
    id.signedIdentity = false;
    return id;
}

bool RealNeuralChipHAL::hardwareCutoffAvailable() const { return false; }
bool RealNeuralChipHAL::closedLoopFeedbackAvailable() const { return false; }

} // namespace dnxr
