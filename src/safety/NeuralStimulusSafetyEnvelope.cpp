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
    : profile_(std::move(profile)),
      frameSchema_(SensoryFrameSchema::defaultSixSenseSchema()) {}

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

const SensoryExposureReport&
NeuralStimulusSafetyEnvelope::lastExposureReport() const {
    return lastExposureReport_;
}

void NeuralStimulusSafetyEnvelope::setFrameSchema(const SensoryFrameSchema& schema) {
    frameSchema_ = schema.valid() ? schema : SensoryFrameSchema::defaultSixSenseSchema();
    lastBoundedChannels_.clear();
    lastModalityBudget_ = NeuralStimulusModalityBudget{};
    exposureBudget_.reset();
    lastExposureReport_ = SensoryExposureReport{};
}

const SensoryFrameSchema& NeuralStimulusSafetyEnvelope::frameSchema() const {
    return frameSchema_;
}

SensoryModality NeuralStimulusSafetyEnvelope::modalityForChannel(std::size_t index) {
    return SensoryFrameSchema::legacyModalityForChannel(index);
}

float NeuralStimulusSafetyEnvelope::channelLimit(std::size_t index) const {
    switch (frameSchema_.modalityForChannel(index)) {
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
        const float limit = std::clamp(channelLimit(i) / frameSchema_.safetyWeightForChannel(i), 0.0f, 1.0f);
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
        const auto modality = static_cast<std::size_t>(frameSchema_.modalityForChannel(i));
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

bool NeuralStimulusSafetyEnvelope::coveragePolicySatisfied(
    SafeNeuralStimulusCommand& command,
    StimulusCoveragePolicy policy,
    const std::vector<SensoryModality>& requiredModalities) const {

    const auto hasModality = [this](SensoryModality modality) {
        const auto index = static_cast<std::size_t>(modality);
        return index < lastModalityBudget_.channelCount.size() &&
               lastModalityBudget_.channelCount[index] > 0U &&
               lastModalityBudget_.peakIntensity[index] > 0.0f;
    };

    switch (policy) {
    case StimulusCoveragePolicy::AnySafeChannel:
        command.coverageReason = command.boundedChannels.empty()
            ? "coverage_no_safe_channels"
            : "coverage_any_safe_channel";
        return !command.boundedChannels.empty();
    case StimulusCoveragePolicy::RequiredModalities:
        for (const auto modality : requiredModalities) {
            if (!hasModality(modality)) {
                command.coverageReason = "coverage_missing_required_modality";
                return false;
            }
        }
        command.coverageReason = "coverage_required_modalities";
        return true;
    case StimulusCoveragePolicy::FullMultisensoryFrame:
        command.coverageReason = command.fiveSenseCoverage
            ? "coverage_full_multisensory_frame"
            : "coverage_missing_full_multisensory_frame";
        return command.fiveSenseCoverage;
    }

    command.coverageReason = "coverage_unknown_policy";
    return false;
}

SafeNeuralStimulusCommand NeuralStimulusSafetyEnvelope::buildCommand(
    std::uint64_t sequenceId,
    const std::vector<float>& sensoryFrame,
    const SafetyDecision& decision,
    StimulusCoveragePolicy coveragePolicy,
    const std::vector<SensoryModality>& requiredModalities) {

    SafeNeuralStimulusCommand command;
    command.sequenceId = sequenceId;
    command.monotonicTimestampMs = TimeSync::nowMs();
    command.boundedChannels = applyEnvelope(sensoryFrame);
    command.modalityIntensities.assign(
        lastModalityBudget_.peakIntensity.begin(),
        lastModalityBudget_.peakIntensity.end());
    command.aggregateSensoryLoad = lastModalityBudget_.aggregateLoad;
    command.fiveSenseCoverage = lastModalityBudget_.hasFiveSenseCoverage;
    command.coveragePolicy = coveragePolicy;
    command.requiredModalities = requiredModalities;
    command.intensityScale = std::clamp(decision.intensityScale, 0.0f, 1.0f);
    command.riskScore = std::clamp(decision.riskScore, 0.0f, 1.0f);
    command.safetyMode = decision.reason.empty() ? "unknown" : decision.reason;
    command.calibrationProfileId = profile_.calibrationProfileId;
    lastExposureReport_ = exposureBudget_.observe(lastModalityBudget_.peakIntensity, 1.0 / 60.0);
    command.exposureBudgetOk = lastExposureReport_.withinBudget;
    const bool coverageOk = coveragePolicySatisfied(command, coveragePolicy, requiredModalities);
    command.allowStimulusDispatch = decision.allowRender &&
                                 !decision.requiresEmergencyStop &&
                                 command.riskScore < 0.90f &&
                                 !command.boundedChannels.empty() &&
                                 coverageOk &&
                                 command.exposureBudgetOk;
    return command;
}

} // namespace dnxr
