#include "PersistentNeuroCognitiveTwin.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

PersistentNeuroCognitiveTwin::
PersistentNeuroCognitiveTwin() {

    initialize();
}

void PersistentNeuroCognitiveTwin::
initialize() {

    profile_.learningRate = 0.05f;

    profile_.emotionalSensitivity = 0.75f;

    profile_.memoryRetention = 0.92f;

    profile_.focusStability = 0.81f;

    profile_.fatigueResistance = 0.65f;

    profile_.adaptationFactor = 0.5f;

    state_.neuralCoherence = 1.0f;

    state_.memoryIntegrity = 1.0f;

    state_.emotionalConsistency = 1.0f;

    state_.adaptationLevel = 0.0f;

    state_.fatigueLevel = 0.0f;

    state_.predictiveAccuracy = 0.0f;

    state_.subjectivePresence = 1.0f;
}

void PersistentNeuroCognitiveTwin::
ingestCognitiveSnapshot(
    const CognitiveSnapshot& snapshot) {

    history_.push_back(snapshot);

    if(history_.size() > 4096) {
        history_.pop_front();
    }
}

float PersistentNeuroCognitiveTwin::
computeSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {

    size_t size =
        std::min(a.size(), b.size());

    if(size == 0)
        return 0.0f;

    float similarity = 0.0f;

    for(size_t i = 0; i < size; ++i) {

        similarity +=
            1.0f -
            std::abs(a[i] - b[i]);
    }

    return similarity /
        static_cast<float>(size);
}

void PersistentNeuroCognitiveTwin::
computeNeuralCoherence() {

    if(history_.size() < 2) {

        state_.neuralCoherence = 1.0f;
        return;
    }

    const auto& current =
        history_[history_.size() - 1];

    const auto& previous =
        history_[history_.size() - 2];

    float memorySimilarity =
        computeSimilarity(
            current.memoryState,
            previous.memoryState);

    float attentionSimilarity =
        computeSimilarity(
            current.attentionState,
            previous.attentionState);

    state_.neuralCoherence =
        (memorySimilarity * 0.5f) +
        (attentionSimilarity * 0.5f);
}

void PersistentNeuroCognitiveTwin::
computeMemoryIntegrity() {

    if(history_.empty()) {

        state_.memoryIntegrity = 0.0f;
        return;
    }

    float integrity = 0.0f;

    for(const auto& snapshot : history_) {

        for(float value : snapshot.memoryState) {

            integrity += std::abs(value);
        }
    }

    integrity /= static_cast<float>(
        history_.size());

    state_.memoryIntegrity =
        std::min(integrity * 0.001f, 1.0f);
}

void PersistentNeuroCognitiveTwin::
computeEmotionalConsistency() {

    if(history_.size() < 2) {

        state_.emotionalConsistency = 1.0f;
        return;
    }

    const auto& current =
        history_[history_.size() - 1];

    const auto& previous =
        history_[history_.size() - 2];

    state_.emotionalConsistency =
        computeSimilarity(
            current.emotionalState,
            previous.emotionalState);
}

void PersistentNeuroCognitiveTwin::
computePredictiveAccuracy() {

    state_.predictiveAccuracy =
        (state_.neuralCoherence * 0.4f) +
        (state_.memoryIntegrity * 0.3f) +
        (state_.emotionalConsistency * 0.3f);
}

void PersistentNeuroCognitiveTwin::
updateBehaviorProfile() {

    profile_.adaptationFactor +=
        state_.predictiveAccuracy *
        0.0005f;

    if(profile_.adaptationFactor > 1.0f) {
        profile_.adaptationFactor = 1.0f;
    }

    profile_.focusStability =
        state_.neuralCoherence;

    profile_.memoryRetention =
        state_.memoryIntegrity;
}

void PersistentNeuroCognitiveTwin::
updateFatigue() {

    if(history_.empty())
        return;

    const auto& current =
        history_.back();

    state_.fatigueLevel =
        current.neuralFatigue;

    state_.subjectivePresence =
        1.0f -
        (state_.fatigueLevel * 0.5f);

    if(state_.subjectivePresence < 0.0f) {
        state_.subjectivePresence = 0.0f;
    }
}

void PersistentNeuroCognitiveTwin::
trainPredictiveModel() {

    state_.adaptationLevel +=
        profile_.learningRate *
        state_.predictiveAccuracy;

    if(state_.adaptationLevel > 1.0f) {
        state_.adaptationLevel = 1.0f;
    }
}

void PersistentNeuroCognitiveTwin::
simulateNeuralAdaptation(
    float deltaTime) {

    computeNeuralCoherence();

    computeMemoryIntegrity();

    computeEmotionalConsistency();

    computePredictiveAccuracy();

    updateBehaviorProfile();

    updateFatigue();

    trainPredictiveModel();
}

const NeuroCognitiveState&
PersistentNeuroCognitiveTwin::
getState() const {

    return state_;
}

const NeuralBehaviorProfile&
PersistentNeuroCognitiveTwin::
getBehaviorProfile() const {

    return profile_;
}

}