#include "NeuralSafetyMonitor.h"

#include <algorithm>

namespace dnxr {

NeuralSafetyMonitor::
NeuralSafetyMonitor() {

    initialize();
}

void NeuralSafetyMonitor::
initialize() {

    state_.threatLevel =
        NeuralThreatLevel::SAFE;

    state_.overallSafety = 1.0f;

    state_.neuralIntegrity = 1.0f;

    state_.emergencyProbability = 0.0f;

    state_.forcedDisconnectRequired =
        false;

    state_.memoryProtectionEnabled =
        false;

    state_.cognitiveIsolationEnabled =
        false;

    state_.emotionalLimiterEnabled =
        false;
}

void NeuralSafetyMonitor::
pushSafetySample(
    const NeuralSafetySample& sample) {

    samples_.push_back(sample);

    if(samples_.size() > 4096) {

        samples_.pop_front();
    }
}

void NeuralSafetyMonitor::
computeThreatLevel() {

    if(samples_.empty())
        return;

    const auto& sample =
        samples_.back();

    float danger =

        sample.corticalStress *

        0.20f +

        sample.neuralTemperature *

        0.10f +

        sample.oscillationInstability *

        0.15f +

        sample.dissociationRisk *

        0.20f +

        sample.synapticOverload *

        0.15f +

        sample.emotionalInstability *

        0.10f +

        sample.memoryFragmentation *

        0.10f;

    if(danger < 0.25f) {

        state_.threatLevel =
            NeuralThreatLevel::SAFE;
    }
    else if(danger < 0.50f) {

        state_.threatLevel =
            NeuralThreatLevel::WARNING;
    }
    else if(danger < 0.75f) {

        state_.threatLevel =
            NeuralThreatLevel::DANGEROUS;
    }
    else {

        state_.threatLevel =
            NeuralThreatLevel::CRITICAL;
    }
}

void NeuralSafetyMonitor::
computeOverallSafety() {

    if(samples_.empty()) {

        state_.overallSafety = 1.0f;
        return;
    }

    const auto& sample =
        samples_.back();

    float instability =

        sample.corticalStress *

        0.15f +

        sample.oscillationInstability *

        0.15f +

        sample.dissociationRisk *

        0.20f +

        sample.synapticOverload *

        0.20f +

        sample.emotionalInstability *

        0.15f +

        sample.memoryFragmentation *

        0.15f;

    state_.overallSafety =
        1.0f - instability;

    if(state_.overallSafety < 0.0f) {

        state_.overallSafety = 0.0f;
    }
}

void NeuralSafetyMonitor::
computeEmergencyProbability() {

    state_.emergencyProbability =
        1.0f -
        state_.overallSafety;
}

void NeuralSafetyMonitor::
evaluateForcedDisconnect() {

    state_.forcedDisconnectRequired =

        state_.threatLevel ==
        NeuralThreatLevel::CRITICAL ||

        state_.emergencyProbability >
        0.85f;
}

void NeuralSafetyMonitor::
activateMemoryProtection() {

    state_.memoryProtectionEnabled =

        state_.threatLevel ==
        NeuralThreatLevel::DANGEROUS ||

        state_.threatLevel ==
        NeuralThreatLevel::CRITICAL;
}

void NeuralSafetyMonitor::
activateCognitiveIsolation() {

    state_.cognitiveIsolationEnabled =

        state_.emergencyProbability >
        0.70f;
}

void NeuralSafetyMonitor::
activateEmotionalLimiter() {

    state_.emotionalLimiterEnabled =

        state_.overallSafety <
        0.45f;
}

void NeuralSafetyMonitor::
stabilizeNeuralLoad() {

    state_.neuralIntegrity =

        state_.overallSafety *

        (1.0f -
        state_.emergencyProbability);
}

void NeuralSafetyMonitor::
update(
    float deltaTime) {

    computeThreatLevel();

    computeOverallSafety();

    computeEmergencyProbability();

    evaluateForcedDisconnect();

    activateMemoryProtection();

    activateCognitiveIsolation();

    activateEmotionalLimiter();

    stabilizeNeuralLoad();
}

const NeuralSafetyState&
NeuralSafetyMonitor::
getState() const {

    return state_;
}

}