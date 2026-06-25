#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

#include "../full_dive/FullDiveNeuralFrame.h"

#include "../immersion/SensoryFrameSchema.h"
#include "NeuralChipAck.h"
#include "ChipCapabilityNegotiation.h"
#include "../neural_chip/BoundedStimulusCommand.h"

namespace dnxr {

enum class NeuralChipRuntimeMode {
    Simulation,
    HardwareInLoop,
    ClinicalLocked
};

struct DeviceIdentity {
    std::string deviceIdHash;
    std::string firmwareHash;
    std::string certificateHash;
    std::string clinicalSessionBinding;
    bool signedIdentity = false;
};

struct NeuralChipStatus {
    bool connected = false;
    bool simulation = true;
    bool emergencyLatched = false;
    float signalQuality = 0.0f;
    float lastLatencyMs = 0.0f;
    std::string statusMessage;
};

struct NeuralChipFrame {
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestampMs = 0;
    std::vector<float> channels;
    float signalQuality = 0.0f;
    float acquisitionLatencyMs = 0.0f;
    bool simulation = true;
};

enum class StimulusCoveragePolicy {
    AnySafeChannel,
    RequiredModalities,
    FullMultisensoryFrame
};

struct SafeNeuralStimulusCommand {
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestampMs = 0;
    std::vector<float> boundedChannels;
    std::vector<float> modalityIntensities;
    float aggregateSensoryLoad = 0.0f;
    float intensityScale = 0.0f;
    float riskScore = 1.0f;
    std::string safetyMode;
    std::string calibrationProfileId;
    bool fiveSenseCoverage = false;
    StimulusCoveragePolicy coveragePolicy = StimulusCoveragePolicy::AnySafeChannel;
    std::vector<SensoryModality> requiredModalities;
    std::string coverageReason = "coverage_not_evaluated";
    bool exposureBudgetOk = true;
    bool allowStimulusDispatch = false;
};

constexpr std::size_t DNXR_STIMULUS_MODALITY_COUNT = static_cast<std::size_t>(SensoryModality::Count);
constexpr std::size_t DNXR_MAX_STIMULUS_CHANNELS = MAX_VISUAL_CHANNELS + MAX_AUDITORY_CHANNELS + MAX_HAPTIC_CHANNELS +
                                                   MAX_PROPRIOCEPTIVE_CHANNELS + MAX_VESTIBULAR_CHANNELS +
                                                   MAX_OLFACTORY_CHANNELS + MAX_GUSTATORY_CHANNELS +
                                                   MAX_INTEROCEPTIVE_CHANNELS + MAX_REALITY_ANCHOR_CHANNELS;

struct SafeNeuralStimulusCommandFixed {
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestampMs = 0;
    FixedVector<float, DNXR_MAX_STIMULUS_CHANNELS> boundedChannels;
    std::array<float, DNXR_STIMULUS_MODALITY_COUNT> modalityIntensities{};
    float aggregateSensoryLoad = 0.0f;
    float intensityScale = 0.0f;
    float riskScore = 1.0f;
    std::string safetyMode;
    std::string calibrationProfileId;
    bool fiveSenseCoverage = false;
    StimulusCoveragePolicy coveragePolicy = StimulusCoveragePolicy::AnySafeChannel;
    std::array<SensoryModality, DNXR_STIMULUS_MODALITY_COUNT> requiredModalities{};
    std::size_t requiredModalityCount = 0;
    std::string coverageReason = "coverage_not_evaluated";
    bool exposureBudgetOk = true;
    bool allowStimulusDispatch = false;
};

SafeNeuralStimulusCommandFixed toFixedCommand(const SafeNeuralStimulusCommand& command);
SafeNeuralStimulusCommand toLegacyCommand(const SafeNeuralStimulusCommandFixed& command);

class NeuralChipHAL {
public:
    virtual ~NeuralChipHAL() = default;

    virtual void connect(const std::string& deviceIdentifier) = 0;
    virtual void disconnect() = 0;
    virtual NeuralChipFrame readFrame() = 0;
    virtual bool writeSafeStimulus(const SafeNeuralStimulusCommandFixed& command) = 0;
    bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) { return command.modalityIntensities.size() == DNXR_STIMULUS_MODALITY_COUNT && writeSafeStimulus(toFixedCommand(command)); }
    virtual void emergencyStop(const std::string& reason) = 0;
    virtual NeuralChipStatus status() const = 0;
    virtual NeuralChipAck lastAck() const { return {}; }
    virtual ChipCapabilities capabilities() const { return {}; }
    virtual DeviceIdentity deviceIdentity() const { return {}; }
    virtual bool hardwareCutoffAvailable() const { return capabilities().supportsHardwareCutoff; }
    virtual bool closedLoopFeedbackAvailable() const { return capabilities().supportsClosedLoopFeedback; }
    virtual bool requiresClosedLoopFeedback() const { return false; }
};

using INeuralChipHAL = NeuralChipHAL;


class RealNeuralChipHAL : public INeuralChipHAL {
public:
    RealNeuralChipHAL();

    void connect(const std::string& deviceIdentifier) override;
    void disconnect() override;
    NeuralChipFrame readFrame() override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommandFixed& command) override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) { return command.modalityIntensities.size() == DNXR_STIMULUS_MODALITY_COUNT && writeSafeStimulus(toFixedCommand(command)); }
    bool init(const std::string& deviceIdentifier);
    bool selfTest();
    bool handshake();
    ChipCapabilities getCapabilities() const;
    std::string readTelemetry() const;
    NeuralChipAck readAck() const;
    bool sendBoundedFrame(const BoundedStimulusCommand& command);
    bool sendBoundedFrame(const RawSensoryPayload&) = delete;
    bool shutdown();
    void emergencyStop(const std::string& reason) override;
    NeuralChipStatus status() const override;
    NeuralChipAck lastAck() const override;
    bool requiresClosedLoopFeedback() const override;
    ChipCapabilities capabilities() const override;
    DeviceIdentity deviceIdentity() const override;
    bool hardwareCutoffAvailable() const override;
    bool closedLoopFeedbackAvailable() const override;

private:
    bool connected_;
    bool emergencyLatched_;
    NeuralChipStatus status_;
    NeuralChipAck lastAck_;
};


class SimulatedNeuralChipHAL : public INeuralChipHAL {
public:
    explicit SimulatedNeuralChipHAL(
        std::size_t channelCount = 64,
        NeuralChipRuntimeMode mode = NeuralChipRuntimeMode::Simulation);

    void connect(const std::string& deviceIdentifier) override;
    void disconnect() override;
    NeuralChipFrame readFrame() override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommandFixed& command) override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) { return command.modalityIntensities.size() == DNXR_STIMULUS_MODALITY_COUNT && writeSafeStimulus(toFixedCommand(command)); }
    bool init(const std::string& deviceIdentifier);
    bool selfTest();
    bool handshake();
    ChipCapabilities getCapabilities() const;
    std::string readTelemetry() const;
    NeuralChipAck readAck() const;
    bool sendBoundedFrame(const BoundedStimulusCommand& command);
    bool sendBoundedFrame(const RawSensoryPayload&) = delete;
    bool shutdown();
    void emergencyStop(const std::string& reason) override;
    NeuralChipStatus status() const override;
    NeuralChipAck lastAck() const override;
    bool requiresClosedLoopFeedback() const override;
    ChipCapabilities capabilities() const override;
    DeviceIdentity deviceIdentity() const override;
    bool hardwareCutoffAvailable() const override;
    bool closedLoopFeedbackAvailable() const override;

    const SafeNeuralStimulusCommandFixed& lastAcceptedStimulus() const;

private:
    bool validateCommand(const SafeNeuralStimulusCommandFixed& command) const;

private:
    std::size_t channelCount_;
    NeuralChipRuntimeMode mode_;
    bool connected_;
    bool emergencyLatched_;
    std::uint64_t nextSequenceId_;
    float phase_;
    NeuralChipStatus status_;
    SafeNeuralStimulusCommandFixed lastAcceptedStimulus_;
    NeuralChipAck lastAck_;
};

class HILNeuralChipHAL final : public SimulatedNeuralChipHAL {
public:
    explicit HILNeuralChipHAL(std::size_t channelCount = 64) : SimulatedNeuralChipHAL(channelCount, NeuralChipRuntimeMode::HardwareInLoop) {}
};

class ClinicalLockedNeuralChipHAL final : public RealNeuralChipHAL {};

} // namespace dnxr
