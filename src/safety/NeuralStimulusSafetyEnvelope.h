#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "SafetyGovernor.h"
#include "SensoryExposureBudget.h"
#include "../hardware/NeuralChipHAL.h"
#include "../immersion/SensoryFrameSchema.h"

namespace dnxr {

struct UserNeuralCalibrationProfile {
    std::string calibrationProfileId = "default_simulation_profile";
    float maxVisualIntensity = 0.85f;
    float maxAudioIntensity = 0.80f;
    float maxHapticIntensity = 0.70f;
    float maxChemicalIntensity = 0.35f;
    float maxBodyIntensity = 0.60f;
    float maxDeltaPerFrame = 0.20f;
    float maxVestibularIntensity = 0.35f;
    float maxProprioceptiveIntensity = 0.60f;
};

struct NeuralStimulusModalityBudget {
    std::array<float, static_cast<std::size_t>(SensoryModality::Count)> peakIntensity{};
    std::array<std::size_t, static_cast<std::size_t>(SensoryModality::Count)> channelCount{};
    float aggregateLoad = 0.0f;
    bool hasFiveSenseCoverage = false;
};

class NeuralStimulusSafetyEnvelope {
public:
    explicit NeuralStimulusSafetyEnvelope(
        UserNeuralCalibrationProfile profile = UserNeuralCalibrationProfile{});

    SafeNeuralStimulusCommand buildCommand(
        std::uint64_t sequenceId,
        const std::vector<float>& sensoryFrame,
        const SafetyDecision& decision,
        StimulusCoveragePolicy coveragePolicy = StimulusCoveragePolicy::AnySafeChannel,
        const std::vector<SensoryModality>& requiredModalities = {});

    void setCalibrationProfile(const UserNeuralCalibrationProfile& profile);
    const UserNeuralCalibrationProfile& calibrationProfile() const;
    const std::vector<float>& lastBoundedChannels() const;
    const NeuralStimulusModalityBudget& lastModalityBudget() const;
    const SensoryExposureReport& lastExposureReport() const;
    void setFrameSchema(const SensoryFrameSchema& schema);
    const SensoryFrameSchema& frameSchema() const;

    static constexpr std::size_t modalityCount() {
        return static_cast<std::size_t>(SensoryModality::Count);
    }
    static SensoryModality modalityForChannel(std::size_t index);

private:
    float channelLimit(std::size_t index) const;
    bool coveragePolicySatisfied(SafeNeuralStimulusCommand& command, StimulusCoveragePolicy policy, const std::vector<SensoryModality>& requiredModalities) const;
    std::vector<float> applyEnvelope(const std::vector<float>& sensoryFrame);
    void updateModalityBudget(const std::vector<float>& boundedChannels);

private:
    UserNeuralCalibrationProfile profile_;
    std::vector<float> lastBoundedChannels_;
    SensoryFrameSchema frameSchema_;
    NeuralStimulusModalityBudget lastModalityBudget_;
    SensoryExposureBudget exposureBudget_;
    SensoryExposureReport lastExposureReport_;
};

} // namespace dnxr
