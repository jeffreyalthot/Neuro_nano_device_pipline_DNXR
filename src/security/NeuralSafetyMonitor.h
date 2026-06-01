#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <vector>

namespace dnxr {

enum class NeuralThreatLevel {

    SAFE,
    WARNING,
    DANGEROUS,
    CRITICAL
};

struct NeuralSafetySample {

    float corticalStress;

    float neuralTemperature;

    float oscillationInstability;

    float dissociationRisk;

    float synapticOverload;

    float emotionalInstability;

    float consciousnessIntegrity;

    float memoryFragmentation;
};

struct NeuralSafetyState {

    NeuralThreatLevel threatLevel;

    float overallSafety;

    float neuralIntegrity;

    float emergencyProbability;

    bool forcedDisconnectRequired;

    bool memoryProtectionEnabled;

    bool cognitiveIsolationEnabled;

    bool emotionalLimiterEnabled;
};

class NeuralSafetyMonitor {
public:

    NeuralSafetyMonitor();

    void initialize();

    void pushSafetySample(
        const NeuralSafetySample& sample);

    void computeThreatLevel();

    void computeOverallSafety();

    void computeEmergencyProbability();

    void evaluateForcedDisconnect();

    void activateMemoryProtection();

    void activateCognitiveIsolation();

    void activateEmotionalLimiter();

    void stabilizeNeuralLoad();

    void update(
        float deltaTime);

    const NeuralSafetyState&
    getState() const;

private:

    std::deque<NeuralSafetySample>
    samples_;

    NeuralSafetyState
    state_;
};

}

namespace neuroxr {
using namespace ::dnxr;
}
