#include "ConsciousDreamingEngine.h"

#include <cmath>

namespace neuroxr {

ConsciousDreamingEngine::
ConsciousDreamingEngine() {

    initialize();
}

void ConsciousDreamingEngine::
initialize() {

    nextSequenceId_ = 1;

    state_.dreamIntensity = 0.0f;

    state_.lucidAwareness = 0.0f;

    state_.subconsciousSynchronization = 1.0f;

    state_.narrativeContinuity = 1.0f;

    state_.emotionalDreamBinding = 0.0f;

    state_.realitySeparation = 0.0f;

    state_.predictiveSimulationStrength = 0.0f;
}

uint64_t
ConsciousDreamingEngine::
createDreamSequence(
    const std::string& title) {

    DreamSequence sequence;

    sequence.id =
        nextSequenceId_++;

    sequence.title =
        title;

    sequence.stability =
        1.0f;

    sequence.persistence =
        1.0f;

    sequence.immersionLevel =
        1.0f;

    sequence.memoryFusion =
        0.0f;

    sequences_[sequence.id] =
        sequence;

    return sequence.id;
}

void
ConsciousDreamingEngine::
injectDreamSignal(
    uint64_t sequenceId,
    const DreamSignal& signal) {

    auto it =
        sequences_.find(
            sequenceId);

    if(it == sequences_.end()) {
        return;
    }

    it->second.signals
        .push_back(signal);
}

void
ConsciousDreamingEngine::
computeDreamIntensity() {

    float intensity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        sequences_) {

        for(const auto& signal :
            pair.second.signals) {

            intensity +=
                signal.intensity *
                signal.realism;

            ++count;
        }
    }

    if(count > 0) {

        intensity /=
            static_cast<float>(
                count);
    }

    state_.dreamIntensity =
        intensity;
}

void
ConsciousDreamingEngine::
computeLucidAwareness() {

    float awareness = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        sequences_) {

        for(const auto& signal :
            pair.second.signals) {

            awareness +=
                signal.narrativeCoherence;

            ++count;
        }
    }

    if(count > 0) {

        awareness /=
            static_cast<float>(
                count);
    }

    state_.lucidAwareness =
        awareness;
}

void
ConsciousDreamingEngine::
computeSubconsciousSynchronization() {

    float synchronization = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        sequences_) {

        for(const auto& signal :
            pair.second.signals) {

            synchronization +=
                signal.subconsciousDepth;

            ++count;
        }
    }

    if(count > 0) {

        synchronization /=
            static_cast<float>(
                count);
    }

    state_.subconsciousSynchronization =
        synchronization;
}

void
ConsciousDreamingEngine::
computeNarrativeContinuity() {

    float continuity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        sequences_) {

        continuity +=
            pair.second.stability *
            pair.second.persistence;

        ++count;
    }

    if(count > 0) {

        continuity /=
            static_cast<float>(
                count);
    }

    state_.narrativeContinuity =
        continuity;
}

void
ConsciousDreamingEngine::
computeEmotionalDreamBinding() {

    float emotionalBinding = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        sequences_) {

        for(const auto& signal :
            pair.second.signals) {

            emotionalBinding +=
                signal.emotionalInfluence;

            ++count;
        }
    }

    if(count > 0) {

        emotionalBinding /=
            static_cast<float>(
                count);
    }

    state_.emotionalDreamBinding =
        emotionalBinding;
}

void
ConsciousDreamingEngine::
computeRealitySeparation() {

    state_.realitySeparation =

        state_.dreamIntensity *

        (1.0f -
        state_.lucidAwareness);
}

void
ConsciousDreamingEngine::
computePredictiveSimulationStrength() {

    state_.predictiveSimulationStrength =

        state_.subconsciousSynchronization *

        state_.dreamIntensity;
}

void
ConsciousDreamingEngine::
stabilizeDreamSpace() {

    for(auto& pair :
        sequences_) {

        pair.second.immersionLevel =

            state_.dreamIntensity;

        pair.second.memoryFusion =

            state_.emotionalDreamBinding;
    }
}

void
ConsciousDreamingEngine::
decayDreamSequences(
    float deltaTime) {

    for(auto& pair :
        sequences_) {

        pair.second.persistence -=
            deltaTime * 0.0002f;

        if(pair.second.persistence <
            0.0f) {

            pair.second.persistence =
                0.0f;
        }
    }
}

void
ConsciousDreamingEngine::
update(
    float deltaTime) {

    decayDreamSequences(
        deltaTime);

    computeDreamIntensity();

    computeLucidAwareness();

    computeSubconsciousSynchronization();

    computeNarrativeContinuity();

    computeEmotionalDreamBinding();

    computeRealitySeparation();

    computePredictiveSimulationStrength();

    stabilizeDreamSpace();
}

const DreamingState&
ConsciousDreamingEngine::
getState() const {

    return state_;
}

}