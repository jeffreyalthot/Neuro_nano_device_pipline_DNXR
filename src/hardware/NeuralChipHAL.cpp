#include "NeuralChipHAL.h"

#include "../utils/TimeSync.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

} // namespace

SafeNeuralStimulusCommandFixed toFixedCommand(const SafeNeuralStimulusCommand& command) {
    SafeNeuralStimulusCommandFixed fixed;
    fixed.sequenceId = command.sequenceId;
    fixed.monotonicTimestampMs = command.monotonicTimestampMs;
    for (float channel : command.boundedChannels) {
        if (!fixed.boundedChannels.try_push_back(channel)) break;
    }
    fixed.modalityIntensities.fill(0.0f);
    const auto modalityCount = std::min(command.modalityIntensities.size(), fixed.modalityIntensities.size());
    for (std::size_t i = 0; i < modalityCount; ++i) fixed.modalityIntensities[i] = command.modalityIntensities[i];
    fixed.aggregateSensoryLoad = command.aggregateSensoryLoad;
    fixed.intensityScale = command.intensityScale;
    fixed.riskScore = command.riskScore;
    fixed.safetyMode = command.safetyMode;
    fixed.calibrationProfileId = command.calibrationProfileId;
    fixed.fiveSenseCoverage = command.fiveSenseCoverage;
    fixed.coveragePolicy = command.coveragePolicy;
    fixed.requiredModalityCount = std::min(command.requiredModalities.size(), fixed.requiredModalities.size());
    for (std::size_t i = 0; i < fixed.requiredModalityCount; ++i) fixed.requiredModalities[i] = command.requiredModalities[i];
    fixed.coverageReason = command.coverageReason;
    fixed.exposureBudgetOk = command.exposureBudgetOk;
    fixed.allowStimulusDispatch = command.allowStimulusDispatch;
    return fixed;
}

SafeNeuralStimulusCommand toLegacyCommand(const SafeNeuralStimulusCommandFixed& command) {
    SafeNeuralStimulusCommand legacy;
    legacy.sequenceId = command.sequenceId;
    legacy.monotonicTimestampMs = command.monotonicTimestampMs;
    legacy.boundedChannels.assign(command.boundedChannels.begin(), command.boundedChannels.end());
    legacy.modalityIntensities.assign(command.modalityIntensities.begin(), command.modalityIntensities.end());
    legacy.aggregateSensoryLoad = command.aggregateSensoryLoad;
    legacy.intensityScale = command.intensityScale;
    legacy.riskScore = command.riskScore;
    legacy.safetyMode = command.safetyMode;
    legacy.calibrationProfileId = command.calibrationProfileId;
    legacy.fiveSenseCoverage = command.fiveSenseCoverage;
    legacy.coveragePolicy = command.coveragePolicy;
    legacy.requiredModalities.assign(command.requiredModalities.begin(), command.requiredModalities.begin() + static_cast<std::ptrdiff_t>(command.requiredModalityCount));
    legacy.coverageReason = command.coverageReason;
    legacy.exposureBudgetOk = command.exposureBudgetOk;
    legacy.allowStimulusDispatch = command.allowStimulusDispatch;
    return legacy;
}

SimulatedNeuralChipHAL::SimulatedNeuralChipHAL(
    std::size_t channelCount,
    NeuralChipRuntimeMode mode)
    : channelCount_(std::max<std::size_t>(1, channelCount)),
      mode_(mode),
      connected_(false),
      emergencyLatched_(false),
      nextSequenceId_(1),
      phase_(0.0f) {

#if DNXR_SIMULATION_ONLY
    if (mode_ != NeuralChipRuntimeMode::Simulation) {
        throw std::runtime_error(
            "DNXR_SIMULATION_ONLY forbids hardware-in-loop and clinical neural chip modes");
    }
#endif

    status_.simulation = (mode_ == NeuralChipRuntimeMode::Simulation);
    status_.statusMessage = "disconnected";
}

void SimulatedNeuralChipHAL::connect(const std::string& deviceIdentifier) {
#if DNXR_SIMULATION_ONLY
    if (mode_ != NeuralChipRuntimeMode::Simulation) {
        throw std::runtime_error("DNXR_SIMULATION_ONLY only permits simulated neural chip connections");
    }
#endif

    if (deviceIdentifier.empty()) {
        throw std::invalid_argument("NeuralChipHAL device identifier missing");
    }

    if (mode_ == NeuralChipRuntimeMode::ClinicalLocked) {
        throw std::runtime_error(
            "ClinicalLocked mode cannot connect without regulated authorization");
    }

    connected_ = true;
    emergencyLatched_ = false;
    status_.connected = true;
    status_.emergencyLatched = false;
    status_.statusMessage = "connected:" + deviceIdentifier;
}

void SimulatedNeuralChipHAL::disconnect() {
    connected_ = false;
    status_.connected = false;
    status_.statusMessage = "disconnected";
}

NeuralChipFrame SimulatedNeuralChipHAL::readFrame() {
    if (!connected_ || emergencyLatched_) {
        return {};
    }

    NeuralChipFrame frame;
    frame.sequenceId = nextSequenceId_++;
    frame.monotonicTimestampMs = TimeSync::nowMs();
    frame.channels.resize(channelCount_);

    phase_ += 0.013f;
    float energy = 0.0f;
    for (std::size_t i = 0; i < channelCount_; ++i) {
        const float t = phase_ + static_cast<float>(i) * 0.017f;
        const float value = (std::sin(t * 3.0f) * 0.28f) +
                            (std::cos(t * 0.7f) * 0.12f);
        frame.channels[i] = std::clamp(value, -1.0f, 1.0f);
        energy += std::abs(frame.channels[i]);
    }

    frame.signalQuality = clamp01(1.0f - (energy / static_cast<float>(channelCount_)) * 0.18f);
    frame.acquisitionLatencyMs = 0.35f;
    frame.simulation = (mode_ == NeuralChipRuntimeMode::Simulation);

    status_.signalQuality = frame.signalQuality;
    status_.lastLatencyMs = frame.acquisitionLatencyMs;
    status_.statusMessage = "frame_read";
    return frame;
}

bool SimulatedNeuralChipHAL::validateCommand(
    const SafeNeuralStimulusCommandFixed& command) const {

    if (!connected_ || emergencyLatched_) {
        return false;
    }

    if (mode_ == NeuralChipRuntimeMode::ClinicalLocked || !command.allowStimulusDispatch) {
        return false;
    }

    if (command.boundedChannels.empty() || command.calibrationProfileId.empty()) {
        return false;
    }

    if (command.riskScore < 0.0f || command.riskScore >= 0.90f) {
        return false;
    }

    if (command.intensityScale < 0.0f || command.intensityScale > 1.0f) {
        return false;
    }

    if (command.aggregateSensoryLoad < 0.0f || command.aggregateSensoryLoad > 1.0f) {
        return false;
    }

    if (command.modalityIntensities.size() != DNXR_STIMULUS_MODALITY_COUNT || !command.exposureBudgetOk) {
        return false;
    }

    if (command.coveragePolicy == StimulusCoveragePolicy::FullMultisensoryFrame &&
        !command.fiveSenseCoverage) {
        return false;
    }

    if (command.coveragePolicy == StimulusCoveragePolicy::RequiredModalities) {
        for (std::size_t m = 0; m < command.requiredModalityCount; ++m) {
            const auto index = static_cast<std::size_t>(command.requiredModalities[m]);
            if (index >= command.modalityIntensities.size() || command.modalityIntensities[index] <= 0.0f) {
                return false;
            }
        }
    }

    for (float intensity : command.modalityIntensities) {
        if (!std::isfinite(intensity) || intensity < 0.0f || intensity > 1.0f) {
            return false;
        }
    }

    for (float channel : command.boundedChannels) {
        if (!std::isfinite(channel) || channel < -1.0f || channel > 1.0f) {
            return false;
        }
    }

    return true;
}

bool SimulatedNeuralChipHAL::writeSafeStimulus(
    const SafeNeuralStimulusCommandFixed& command) {

    lastAck_ = {};
    lastAck_.packetId = command.sequenceId;
    lastAck_.latencyMeasuredMs = 0.4f;
    lastAck_.deviceTemperature = 37.0f;
    lastAck_.signalQuality = status_.signalQuality > 0.0f ? status_.signalQuality : 0.99f;
    lastAck_.signedAck = true;
    lastAck_.signatureAlgorithm = "SIM-HMAC-SHA256";
    lastAck_.signature = "simulated_signed_ack";
    lastAck_.transportState = NeuralChipAckTransportState::TransportAccepted;
    if (!validateCommand(command)) {
        lastAck_.packetRejected = true;
        lastAck_.transportState = NeuralChipAckTransportState::TransportRejected;
        lastAck_.firmwareState = NeuralChipAckFirmwareState::FirmwareRejected;
        lastAck_.hardwareState = NeuralChipAckHardwareState::HardwareRejected;
        lastAck_.notApplied = true;
        lastAck_.hardwareCutoffActive = emergencyLatched_;
        lastAck_.rejectReason = "command_failed_hal_validation";
        return false;
    }

    lastAcceptedStimulus_ = command;
    lastAck_.packetAccepted = true;
    lastAck_.firmwareValidated = true;
    lastAck_.firmwareState = NeuralChipAckFirmwareState::FirmwareValidated;
    lastAck_.applied = true;
    lastAck_.hardwareState = NeuralChipAckHardwareState::HardwareApplied;
    lastAck_.notApplied = false;
    lastAck_.feedbackAvailable = (mode_ != NeuralChipRuntimeMode::ClinicalLocked);
    lastAck_.feedbackConfirmed = lastAck_.feedbackAvailable;
    lastAck_.feedbackState = lastAck_.feedbackAvailable ? NeuralChipAckFeedbackState::FeedbackConfirmed : NeuralChipAckFeedbackState::NotConfirmed;
    lastAck_.appliedChannelCount = static_cast<std::uint32_t>(command.boundedChannels.size());
    status_.statusMessage = "stimulus_accepted:" + command.safetyMode;
    return true;
}

void SimulatedNeuralChipHAL::emergencyStop(const std::string& reason) {
    emergencyLatched_ = true;
    connected_ = false;
    status_.connected = false;
    status_.emergencyLatched = true;
    status_.statusMessage = "emergency_stop:" + reason;
}

NeuralChipStatus SimulatedNeuralChipHAL::status() const {
    NeuralChipStatus status = status_;
    status.connected = connected_;
    status.emergencyLatched = emergencyLatched_;
    status.simulation = (mode_ == NeuralChipRuntimeMode::Simulation);
    return status;
}

const SafeNeuralStimulusCommandFixed& SimulatedNeuralChipHAL::lastAcceptedStimulus() const {
    return lastAcceptedStimulus_;
}

} // namespace dnxr

namespace dnxr {

NeuralChipAck SimulatedNeuralChipHAL::lastAck() const {
    return lastAck_;
}

bool SimulatedNeuralChipHAL::requiresClosedLoopFeedback() const {
    return mode_ != NeuralChipRuntimeMode::Simulation;
}

} // namespace dnxr

namespace dnxr {

ChipCapabilities SimulatedNeuralChipHAL::capabilities() const {
    ChipCapabilities c;
    c.canVisual = true;
    c.canAuditory = true;
    c.canHaptic = true;
    c.canVestibular = true;
    c.canProprioceptive = true;
    c.canOlfactory = true;
    c.canGustatory = true;
    c.maxSafeRateHz = 120.0f;
    c.maxPacketSize = static_cast<int>(channelCount_ * sizeof(float));
    c.supportsHardwareCutoff = !emergencyLatched_;
    c.supportsClosedLoopFeedback = true;
    return c;
}

DeviceIdentity SimulatedNeuralChipHAL::deviceIdentity() const {
    DeviceIdentity id;
    id.deviceIdHash = "simulated_device_identity_hash";
    id.firmwareHash = "simulated_firmware_hash";
    id.certificateHash = "simulated_certificate_hash";
    id.clinicalSessionBinding = mode_ == NeuralChipRuntimeMode::Simulation ? "simulation" : "hil";
    id.signedIdentity = true;
    return id;
}

bool SimulatedNeuralChipHAL::hardwareCutoffAvailable() const { return capabilities().supportsHardwareCutoff; }
bool SimulatedNeuralChipHAL::closedLoopFeedbackAvailable() const { return capabilities().supportsClosedLoopFeedback; }

} // namespace dnxr
