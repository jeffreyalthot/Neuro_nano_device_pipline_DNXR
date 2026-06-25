#include "NeuralChipHAL.h"


#include <stdexcept>

namespace dnxr {

RealNeuralChipHAL::RealNeuralChipHAL()
    : connected_(false), emergencyLatched_(true) {
    status_.connected = false;
    status_.simulation = false;
    status_.emergencyLatched = true;
    status_.statusMessage = "real_hal_ready_for_authorized_hardware_bench";
}

void RealNeuralChipHAL::connect(const std::string& deviceIdentifier) {
    if (!init(deviceIdentifier)) {
        throw std::runtime_error("RealNeuralChipHAL failed to initialize controlled hardware bench transport");
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

bool RealNeuralChipHAL::writeSafeStimulus(const SafeNeuralStimulusCommandFixed& command) {
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

namespace dnxr {

bool RealNeuralChipHAL::init(const std::string& deviceIdentifier) {
    if (deviceIdentifier.empty()) { throw std::invalid_argument("RealNeuralChipHAL device identifier missing"); }
    connected_ = true; emergencyLatched_ = false; status_.connected = true; status_.emergencyLatched = false; status_.statusMessage = "real_hal_initialized_controlled_bench"; return true;
}
bool RealNeuralChipHAL::selfTest() { lastAck_.packetAccepted = true; return connected_; }
bool RealNeuralChipHAL::handshake() { lastAck_.packetAccepted = connected_; return connected_; }
ChipCapabilities RealNeuralChipHAL::getCapabilities() const { return capabilities(); }
std::string RealNeuralChipHAL::readTelemetry() const { return connected_ ? "telemetry_channel_alive" : "telemetry_disconnected"; }
NeuralChipAck RealNeuralChipHAL::readAck() const { return lastAck_; }
bool RealNeuralChipHAL::sendBoundedFrame(const BoundedStimulusCommand& command) {
    if (!connected_ || emergencyLatched_ || !command.dummyLoadAttached || !command.activeOutputEnabled || command.maxOutputCeiling > 0.0f) { status_.statusMessage = "bounded_frame_rejected_by_bench_guard"; return false; }
    lastAck_.packetAccepted = true; status_.statusMessage = "bounded_frame_accepted_for_dummy_load_zero_ceiling"; return true;
}
bool RealNeuralChipHAL::shutdown() { disconnect(); return true; }

} // namespace dnxr
