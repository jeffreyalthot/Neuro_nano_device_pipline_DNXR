#include "ConsciousnessBridge.h"

#include <cmath>

namespace neuroxr {

ConsciousnessBridge::
ConsciousnessBridge() {

    initialize();
}

void ConsciousnessBridge::
initialize() {

    state_.signals.clear();

    state_.awarenessLevel = 1.0f;

    state_.selfPerception = 1.0f;

    state_.dreamStateIntensity = 0.0f;

    state_.subjectiveRealityLock = 1.0f;

    state_.dissociationRisk = 0.0f;
}

void ConsciousnessBridge::
generateConsciousnessSignal(
    ConsciousnessLayer layer,
    float intensity) {

    ConsciousnessSignal signal;

    signal.layer = layer;

    signal.intensity = intensity;

    signal.stability = 1.0f;

    signal.synchronization = 1.0f;

    signal.perceptualBinding =
        intensity * 0.8f;

    signal.identityIntegrity =
        intensity;

    state_.signals.push_back(signal);
}

void ConsciousnessBridge::
synchronizeConsciousness() {

    for(auto& signal :
        state_.signals) {

        signal.synchronization =
            std::sin(signal.intensity) *
            0.5f +
            0.5f;
    }
}

void ConsciousnessBridge::
computeAwarenessLevel() {

    float awareness = 0.0f;

    for(const auto& signal :
        state_.signals) {

        awareness +=
            signal.intensity *
            signal.synchronization;
    }

    if(!state_.signals.empty()) {

        awareness /=
            static_cast<float>(
                state_.signals.size());
    }

    state_.awarenessLevel =
        awareness;
}

void ConsciousnessBridge::
computeSelfPerception() {

    float perception = 0.0f;

    for(const auto& signal :
        state_.signals) {

        perception +=
            signal.identityIntegrity;
    }

    if(!state_.signals.empty()) {

        perception /=
            static_cast<float>(
                state_.signals.size());
    }

    state_.selfPerception =
        perception;
}

void ConsciousnessBridge::
computeRealityLock() {

    state_.subjectiveRealityLock =

        (state_.awarenessLevel * 0.5f)

        +

        (state_.selfPerception * 0.5f);
}

void ConsciousnessBridge::
computeDissociationRisk() {

    state_.dissociationRisk =
        1.0f -
        state_.subjectiveRealityLock;

    if(state_.dissociationRisk < 0.0f) {

        state_.dissociationRisk = 0.0f;
    }
}

void ConsciousnessBridge::
stabilizeConsciousness() {

    state_.dreamStateIntensity =

        1.0f -
        state_.awarenessLevel;
}

void ConsciousnessBridge::
update(
    float deltaTime) {

    synchronizeConsciousness();

    computeAwarenessLevel();

    computeSelfPerception();

    computeRealityLock();

    computeDissociationRisk();

    stabilizeConsciousness();
}

const ConsciousnessState&
ConsciousnessBridge::
getState() const {

    return state_;
}

}