#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../immersion/SensoryFrameSchema.h"

namespace dnxr {

enum class NeuralChipRuntimeMode {
    Simulation,
    HardwareInLoop,
    ClinicalLocked
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

class NeuralChipHAL {
public:
    virtual ~NeuralChipHAL() = default;

    virtual void connect(const std::string& deviceIdentifier) = 0;
    virtual void disconnect() = 0;
    virtual NeuralChipFrame readFrame() = 0;
    virtual bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) = 0;
    virtual void emergencyStop(const std::string& reason) = 0;
    virtual NeuralChipStatus status() const = 0;
};

class RealNeuralChipHAL final : public NeuralChipHAL {
public:
    RealNeuralChipHAL();

    void connect(const std::string& deviceIdentifier) override;
    void disconnect() override;
    NeuralChipFrame readFrame() override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) override;
    void emergencyStop(const std::string& reason) override;
    NeuralChipStatus status() const override;

private:
    bool connected_;
    bool emergencyLatched_;
    NeuralChipStatus status_;
};


class SimulatedNeuralChipHAL final : public NeuralChipHAL {
public:
    explicit SimulatedNeuralChipHAL(
        std::size_t channelCount = 64,
        NeuralChipRuntimeMode mode = NeuralChipRuntimeMode::Simulation);

    void connect(const std::string& deviceIdentifier) override;
    void disconnect() override;
    NeuralChipFrame readFrame() override;
    bool writeSafeStimulus(const SafeNeuralStimulusCommand& command) override;
    void emergencyStop(const std::string& reason) override;
    NeuralChipStatus status() const override;

    const SafeNeuralStimulusCommand& lastAcceptedStimulus() const;

private:
    bool validateCommand(const SafeNeuralStimulusCommand& command) const;

private:
    std::size_t channelCount_;
    NeuralChipRuntimeMode mode_;
    bool connected_;
    bool emergencyLatched_;
    std::uint64_t nextSequenceId_;
    float phase_;
    NeuralChipStatus status_;
    SafeNeuralStimulusCommand lastAcceptedStimulus_;
};

} // namespace dnxr
