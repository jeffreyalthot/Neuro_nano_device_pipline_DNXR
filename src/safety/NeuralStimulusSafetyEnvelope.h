#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "SafetyGovernor.h"
#include "../hardware/NeuralChipHAL.h"

namespace dnxr {

struct UserNeuralCalibrationProfile {
    std::string calibrationProfileId = "default_simulation_profile";
    float maxVisualIntensity = 0.85f;
    float maxAudioIntensity = 0.80f;
    float maxHapticIntensity = 0.70f;
    float maxChemicalIntensity = 0.35f;
    float maxBodyIntensity = 0.60f;
    float maxDeltaPerFrame = 0.20f;
};

enum class SensoryModality : std::size_t {
    Visual = 0,
    Auditory = 1,
    HapticTouch = 2,
    ChemicalTasteSmell = 3,
    ProprioceptiveVestibular = 4,
    Count = 5
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
        const SafetyDecision& decision);

    void setCalibrationProfile(const UserNeuralCalibrationProfile& profile);
    const UserNeuralCalibrationProfile& calibrationProfile() const;
    const std::vector<float>& lastBoundedChannels() const;
    const NeuralStimulusModalityBudget& lastModalityBudget() const;

    static constexpr std::size_t modalityCount() {
        return static_cast<std::size_t>(SensoryModality::Count);
    }
    static SensoryModality modalityForChannel(std::size_t index);

private:
    float channelLimit(std::size_t index) const;
    std::vector<float> applyEnvelope(const std::vector<float>& sensoryFrame);
    void updateModalityBudget(const std::vector<float>& boundedChannels);

private:
    UserNeuralCalibrationProfile profile_;
    std::vector<float> lastBoundedChannels_;
    NeuralStimulusModalityBudget lastModalityBudget_;
};

} // namespace dnxr
