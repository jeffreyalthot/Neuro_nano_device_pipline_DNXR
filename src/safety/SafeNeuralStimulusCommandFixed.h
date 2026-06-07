#pragma once

#include "../full_dive/FullDiveNeuralFrame.h"
#include "../hardware/NeuralChipHAL.h"

#include <array>
#include <cstdint>
#include <string>

namespace dnxr {

constexpr std::size_t MAX_STIMULUS_CHANNELS = MAX_VISUAL_CHANNELS + MAX_AUDITORY_CHANNELS + MAX_HAPTIC_CHANNELS +
                                             MAX_PROPRIOCEPTIVE_CHANNELS + MAX_VESTIBULAR_CHANNELS +
                                             MAX_OLFACTORY_CHANNELS + MAX_GUSTATORY_CHANNELS +
                                             MAX_INTEROCEPTIVE_CHANNELS + MAX_REALITY_ANCHOR_CHANNELS;
constexpr std::size_t MODALITY_COUNT = 7;

struct SafeNeuralStimulusCommandFixed {
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestampMs = 0;
    FixedVector<float, MAX_STIMULUS_CHANNELS> boundedChannels;
    std::array<float, MODALITY_COUNT> modalityIntensities{};
    float aggregateSensoryLoad = 0.0f;
    float intensityScale = 0.0f;
    float riskScore = 1.0f;
    std::string safetyMode;
    std::string calibrationProfileId;
    bool fiveSenseCoverage = false;
    StimulusCoveragePolicy coveragePolicy = StimulusCoveragePolicy::AnySafeChannel;
    std::array<SensoryModality, MODALITY_COUNT> requiredModalities{};
    std::size_t requiredModalityCount = 0;
    std::string coverageReason = "coverage_not_evaluated";
    bool exposureBudgetOk = true;
    bool allowStimulusDispatch = false;
};

} // namespace dnxr
