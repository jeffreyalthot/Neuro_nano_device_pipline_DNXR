#include "NeuralStimulusSafetyEnvelope.h"

#include "../utils/TimeSync.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace dnxr {
namespace {

float finiteOrZero(float value) {
    return std::isfinite(value) ? value : 0.0f;
}

} // namespace

NeuralStimulusSafetyEnvelope::NeuralStimulusSafetyEnvelope(
    UserNeuralCalibrationProfile profile)
    : profile_(std::move(profile)) {}

void NeuralStimulusSafetyEnvelope::setCalibrationProfile(
    const UserNeuralCalibrationProfile& profile) {

    profile_ = profile;
    lastBoundedChannels_.clear();
    lastModalityBudget_ = NeuralStimulusModalityBudget{};
}

const UserNeuralCalibrationProfile&
NeuralStimulusSafetyEnvelope::calibrationProfile() const {

    return profile_;
}

const std::vector<float>&
NeuralStimulusSafetyEnvelope::lastBoundedChannels() const {

    return lastBoundedChannels_;
}

const NeuralStimulusModalityBudget&
NeuralStimulusSafetyEnvelope::lastModalityBudget() const {

    return lastModalityBudget_;
}

SensoryModality NeuralStimulusSafetyEnvelope::modalityForChannel(std::size_t index) {
    if (index <= 2U) {
        return SensoryModality::Visual;
    }
    if (index <= 4U) {
        return SensoryModality::Auditory;
    }
    if (index <= 6U) {
        return SensoryModality::HapticTouch;
    }
    if (index <= 8U) {
        return SensoryModality::ChemicalTasteSmell;
    }
    if (index <= 10U) {
        return SensoryModality::Proprioceptive;
    }
    return SensoryModality::Vestibular;
}

float NeuralStimulusSafetyEnvelope::channelLimit(std::size_t index) const {
    switch (modalityForChannel(index)) {
        case SensoryModality::Visual:
            return profile_.maxVisualIntensity;
        case SensoryModality::Auditory:
            return profile_.maxAudioIntensity;
        case SensoryModality::HapticTouch:
            return profile_.maxHapticIntensity;
        case SensoryModality::ChemicalTasteSmell:
            return profile_.maxChemicalIntensity;
        case SensoryModality::Proprioceptive:
            return profile_.maxProprioceptiveIntensity;
        case SensoryModality::Vestibular:
            return profile_.maxVestibularIntensity;
        case SensoryModality::Count:
            return profile_.maxVestibularIntensity;
    }

    return profile_.maxVestibularIntensity;
}

std::vector<float> NeuralStimulusSafetyEnvelope::applyEnvelope(
    const std::vector<float>& sensoryFrame) {

    std::vector<float> bounded;
    bounded.reserve(sensoryFrame.size());

    if (lastBoundedChannels_.size() != sensoryFrame.size()) {
        lastBoundedChannels_.assign(sensoryFrame.size(), 0.0f);
    }

    const float maxDelta = std::clamp(profile_.maxDeltaPerFrame, 0.01f, 1.0f);

    for (std::size_t i = 0; i < sensoryFrame.size(); ++i) {
        const float limit = std::clamp(channelLimit(i), 0.0f, 1.0f);
        const float target = std::clamp(finiteOrZero(sensoryFrame[i]), -limit, limit);
        const float previous = lastBoundedChannels_[i];
        const float delta = std::clamp(target - previous, -maxDelta, maxDelta);
        bounded.push_back(std::clamp(previous + delta, -limit, limit));
    }

    lastBoundedChannels_ = bounded;
    updateModalityBudget(lastBoundedChannels_);
    return bounded;
}

void NeuralStimulusSafetyEnvelope::updateModalityBudget(
    const std::vector<float>& boundedChannels) {

    NeuralStimulusModalityBudget budget;
    float total = 0.0f;

    for (std::size_t i = 0; i < boundedChannels.size(); ++i) {
        const auto modality = static_cast<std::size_t>(modalityForChannel(i));
        const float intensity = std::abs(finiteOrZero(boundedChannels[i]));
        budget.peakIntensity[modality] = std::max(budget.peakIntensity[modality], intensity);
        ++budget.channelCount[modality];
        total += intensity;
    }

    budget.aggregateLoad = boundedChannels.empty()
        ? 0.0f
        : std::clamp(total / static_cast<float>(boundedChannels.size()), 0.0f, 1.0f);
    budget.hasFiveSenseCoverage = std::all_of(
        budget.channelCount.begin(),
        budget.channelCount.end(),
        [](std::size_t count) { return count > 0U; });

    lastModalityBudget_ = budget;
}

SafeNeuralStimulusCommand NeuralStimulusSafetyEnvelope::buildCommand(
    std::uint64_t sequenceId,
    const std::vector<float>& sensoryFrame,
    const SafetyDecision& decision) {

    SafeNeuralStimulusCommand command;
    command.sequenceId = sequenceId;
    command.monotonicTimestampMs = TimeSync::nowMs();
    command.boundedChannels = applyEnvelope(sensoryFrame);
    command.modalityIntensities.assign(
        lastModalityBudget_.peakIntensity.begin(),
        lastModalityBudget_.peakIntensity.end());
    command.aggregateSensoryLoad = lastModalityBudget_.aggregateLoad;
    command.fiveSenseCoverage = lastModalityBudget_.hasFiveSenseCoverage;
    command.intensityScale = std::clamp(decision.intensityScale, 0.0f, 1.0f);
    command.riskScore = std::clamp(decision.riskScore, 0.0f, 1.0f);
    command.safetyMode = decision.reason.empty() ? "unknown" : decision.reason;
    command.calibrationProfileId = profile_.calibrationProfileId;
    command.allowStimulusDispatch = decision.allowRender &&
                                 !decision.requiresEmergencyStop &&
                                 command.riskScore < 0.90f &&
                                 !command.boundedChannels.empty() &&
                                 command.fiveSenseCoverage;
    return command;
}

} // namespace dnxr
