#include "SubconsciousSimulationEngine.h"

#include <cmath>

namespace neuroxr {

SubconsciousSimulationEngine::
SubconsciousSimulationEngine() {

    initialize();
}

void SubconsciousSimulationEngine::
initialize() {

    nextPatternId_ = 1;

    state_.subconsciousActivity =
        0.0f;

    state_.emotionalPressure =
        0.0f;

    state_.predictivePressure =
        0.0f;

    state_.identityFragmentation =
        0.0f;

    state_.memoryEchoIntensity =
        0.0f;

    state_.dreamConstructIntensity =
        0.0f;

    state_.subconsciousCoherence =
        1.0f;

    state_.subconsciousDrift =
        0.0f;
}

uint64_t
SubconsciousSimulationEngine::
createPattern(
    SubconsciousPatternType type,
    const std::string& label) {

    SubconsciousPattern pattern;

    pattern.patternId =
        nextPatternId_++;

    pattern.type =
        type;

    pattern.label =
        label;

    pattern.intensity =
        1.0f;

    pattern.persistence =
        1.0f;

    pattern.emotionalWeight =
        0.5f;

    pattern.cognitiveInfluence =
        0.5f;

    pattern.realityInfluence =
        0.5f;

    patterns_[pattern.patternId] =
        pattern;

    return pattern.patternId;
}

void
SubconsciousSimulationEngine::
injectPatternInfluence(
    uint64_t patternId,
    float influence) {

    auto it =
        patterns_.find(
            patternId);

    if(it == patterns_.end()) {
        return;
    }

    it->second.intensity +=
        influence;

    if(it->second.intensity >
        1.0f) {

        it->second.intensity =
            1.0f;
    }
}

void
SubconsciousSimulationEngine::
computeSubconsciousActivity() {

    float activity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        activity +=
            pair.second.intensity;

        ++count;
    }

    if(count > 0) {

        activity /=
            static_cast<float>(
                count);
    }

    state_.subconsciousActivity =
        activity;
}

void
SubconsciousSimulationEngine::
computeEmotionalPressure() {

    float pressure = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        pressure +=
            pair.second.emotionalWeight *
            pair.second.intensity;

        ++count;
    }

    if(count > 0) {

        pressure /=
            static_cast<float>(
                count);
    }

    state_.emotionalPressure =
        pressure;
}

void
SubconsciousSimulationEngine::
computePredictivePressure() {

    float pressure = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        pressure +=
            pair.second.cognitiveInfluence;

        ++count;
    }

    if(count > 0) {

        pressure /=
            static_cast<float>(
                count);
    }

    state_.predictivePressure =
        pressure;
}

void
SubconsciousSimulationEngine::
computeIdentityFragmentation() {

    float fragmentation = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        fragmentation +=
            pair.second.realityInfluence *
            0.5f;

        ++count;
    }

    if(count > 0) {

        fragmentation /=
            static_cast<float>(
                count);
    }

    state_.identityFragmentation =
        fragmentation;
}

void
SubconsciousSimulationEngine::
computeMemoryEchoIntensity() {

    float echo = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        if(pair.second.type ==
            SubconsciousPatternType::
            MEMORY_ECHO) {

            echo +=
                pair.second.intensity;

            ++count;
        }
    }

    if(count > 0) {

        echo /=
            static_cast<float>(
                count);
    }

    state_.memoryEchoIntensity =
        echo;
}

void
SubconsciousSimulationEngine::
computeDreamConstructIntensity() {

    float intensity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        patterns_) {

        if(pair.second.type ==
            SubconsciousPatternType::
            DREAM_CONSTRUCT) {

            intensity +=
                pair.second.intensity;

            ++count;
        }
    }

    if(count > 0) {

        intensity /=
            static_cast<float>(
                count);
    }

    state_.dreamConstructIntensity =
        intensity;
}

void
SubconsciousSimulationEngine::
computeSubconsciousCoherence() {

    state_.subconsciousCoherence =

        1.0f -

        (
            state_.identityFragmentation *
            0.5f +

            state_.subconsciousDrift *
            0.5f
        );

    if(state_.subconsciousCoherence <
        0.0f) {

        state_.subconsciousCoherence =
            0.0f;
    }
}

void
SubconsciousSimulationEngine::
computeSubconsciousDrift() {

    state_.subconsciousDrift =

        state_.emotionalPressure *

        (1.0f -
        state_.predictivePressure);
}

void
SubconsciousSimulationEngine::
stabilizeSubconsciousField() {

    for(auto& pair :
        patterns_) {

        pair.second.persistence =

            state_.subconsciousCoherence;

        pair.second.realityInfluence =

            1.0f -
            state_.subconsciousDrift;
    }
}

void
SubconsciousSimulationEngine::
decayPatterns(
    float deltaTime) {

    for(auto& pair :
        patterns_) {

        pair.second.intensity -=
            deltaTime * 0.0001f;

        if(pair.second.intensity <
            0.0f) {

            pair.second.intensity =
                0.0f;
        }
    }
}

void
SubconsciousSimulationEngine::
update(
    float deltaTime) {

    decayPatterns(
        deltaTime);

    computeSubconsciousActivity();

    computeEmotionalPressure();

    computePredictivePressure();

    computeIdentityFragmentation();

    computeMemoryEchoIntensity();

    computeDreamConstructIntensity();

    computeSubconsciousDrift();

    computeSubconsciousCoherence();

    stabilizeSubconsciousField();
}

const SubconsciousState&
SubconsciousSimulationEngine::
getState() const {

    return state_;
}

}