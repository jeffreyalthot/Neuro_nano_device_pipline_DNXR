#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

struct CognitiveSnapshot {

    uint64_t timestamp;

    float cognitiveLoad;

    float emotionalStress;

    float neuralFatigue;

    float immersionLevel;

    std::vector<float> memoryState;

    std::vector<float> attentionState;

    std::vector<float> emotionalState;
};

struct NeuralBehaviorProfile {

    float learningRate;

    float emotionalSensitivity;

    float memoryRetention;

    float focusStability;

    float fatigueResistance;

    float adaptationFactor;
};

struct NeuroCognitiveState {

    float neuralCoherence;

    float memoryIntegrity;

    float emotionalConsistency;

    float adaptationLevel;

    float fatigueLevel;

    float predictiveAccuracy;

    float subjectivePresence;
};

class PersistentNeuroCognitiveTwin {
public:

    PersistentNeuroCognitiveTwin();

    void initialize();

    void ingestCognitiveSnapshot(
        const CognitiveSnapshot& snapshot);

    void simulateNeuralAdaptation(
        float deltaTime);

    void computeNeuralCoherence();

    void computeMemoryIntegrity();

    void computeEmotionalConsistency();

    void computePredictiveAccuracy();

    void updateBehaviorProfile();

    void updateFatigue();

    void trainPredictiveModel();

    const NeuroCognitiveState&
    getState() const;

    const NeuralBehaviorProfile&
    getBehaviorProfile() const;

private:

    float computeSimilarity(
        const std::vector<float>& a,
        const std::vector<float>& b) const;

private:

    std::deque<CognitiveSnapshot> history_;

    NeuralBehaviorProfile profile_;

    NeuroCognitiveState state_;
};

}