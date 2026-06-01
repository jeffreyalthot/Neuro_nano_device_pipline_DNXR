#include "RealityCoherenceEngine.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

RealityCoherenceEngine::
RealityCoherenceEngine() {

    initialize();
}

void RealityCoherenceEngine::
initialize() {

    nextFragmentId_ = 1;

    state_.globalRealityCoherence =
        1.0f;

    state_.perceptualIntegrity =
        1.0f;

    state_.subconsciousConsistency =
        1.0f;

    state_.sensorySynchronization =
        1.0f;

    state_.subjectivePresence =
        1.0f;

    state_.immersionLock =
        1.0f;

    state_.hallucinationRisk =
        0.0f;

    state_.perceptualDrift =
        0.0f;
}

uint64_t
RealityCoherenceEngine::
createRealityFragment(
    const std::string& label) {

    RealityFragment fragment;

    fragment.id =
        nextFragmentId_++;

    fragment.label =
        label;

    fragment.persistence =
        1.0f;

    fragment.continuity =
        1.0f;

    fragment.immersionWeight =
        1.0f;

    fragment.temporalAnchor =
        0.0f;

    fragments_[fragment.id] =
        fragment;

    return fragment.id;
}

void
RealityCoherenceEngine::
injectPerceptualSignal(
    uint64_t fragmentId,
    const PerceptualSignal& signal) {

    auto it =
        fragments_.find(
            fragmentId);

    if(it == fragments_.end()) {
        return;
    }

    it->second.signals
        .push_back(signal);
}

float
RealityCoherenceEngine::
computeSignalCompatibility(
    const PerceptualSignal& a,
    const PerceptualSignal& b) const {

    float realismDelta =
        std::abs(
            a.realism -
            b.realism);

    float latencyDelta =
        std::abs(
            a.latency -
            b.latency);

    float coherenceDelta =
        std::abs(
            a.coherence -
            b.coherence);

    float alignmentDelta =
        std::abs(
            a.neuralAlignment -
            b.neuralAlignment);

    return
        1.0f -

        (
            realismDelta *
            0.25f +

            latencyDelta *
            0.25f +

            coherenceDelta *
            0.25f +

            alignmentDelta *
            0.25f
        );
}

void
RealityCoherenceEngine::
computeRealityCoherence() {

    float coherence = 0.0f;

    uint64_t comparisons = 0;

    for(const auto& fragmentPair :
        fragments_) {

        const auto& signals =
            fragmentPair.second.signals;

        for(size_t i = 0;
            i < signals.size();
            ++i) {

            for(size_t j = i + 1;
                j < signals.size();
                ++j) {

                coherence +=
                    computeSignalCompatibility(
                        signals[i],
                        signals[j]);

                ++comparisons;
            }
        }
    }

    if(comparisons > 0) {

        coherence /=
            static_cast<float>(
                comparisons);
    }

    state_.globalRealityCoherence =
        coherence;
}

void
RealityCoherenceEngine::
computePerceptualIntegrity() {

    float integrity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        integrity +=
            pair.second.persistence *
            pair.second.continuity;

        ++count;
    }

    if(count > 0) {

        integrity /=
            static_cast<float>(
                count);
    }

    state_.perceptualIntegrity =
        integrity;
}

void
RealityCoherenceEngine::
computeSubconsciousConsistency() {

    float consistency = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        for(const auto& signal :
            pair.second.signals) {

            consistency +=
                signal.subconsciousBinding;

            ++count;
        }
    }

    if(count > 0) {

        consistency /=
            static_cast<float>(
                count);
    }

    state_.subconsciousConsistency =
        consistency;
}

void
RealityCoherenceEngine::
computeSensorySynchronization() {

    float synchronization = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        for(const auto& signal :
            pair.second.signals) {

            synchronization +=
                signal.neuralAlignment;

            ++count;
        }
    }

    if(count > 0) {

        synchronization /=
            static_cast<float>(
                count);
    }

    state_.sensorySynchronization =
        synchronization;
}

void
RealityCoherenceEngine::
computeSubjectivePresence() {

    state_.subjectivePresence =

        (state_.globalRealityCoherence *
        0.30f)

        +

        (state_.perceptualIntegrity *
        0.20f)

        +

        (state_.subconsciousConsistency *
        0.20f)

        +

        (state_.sensorySynchronization *
        0.30f);
}

void
RealityCoherenceEngine::
computeImmersionLock() {

    state_.immersionLock =

        state_.subjectivePresence *

        state_.globalRealityCoherence;
}

void
RealityCoherenceEngine::
computeHallucinationRisk() {

    state_.hallucinationRisk =
        1.0f -
        state_.immersionLock;

    if(state_.hallucinationRisk < 0.0f) {

        state_.hallucinationRisk = 0.0f;
    }
}

void
RealityCoherenceEngine::
computePerceptualDrift() {

    state_.perceptualDrift =

        1.0f -
        state_.sensorySynchronization;

    if(state_.perceptualDrift < 0.0f) {

        state_.perceptualDrift = 0.0f;
    }
}

void
RealityCoherenceEngine::
stabilizeRealityMesh() {

    for(auto& pair :
        fragments_) {

        pair.second.continuity =

            state_.globalRealityCoherence;

        pair.second.immersionWeight =

            state_.subjectivePresence;
    }
}

void
RealityCoherenceEngine::
decayRealityFragments(
    float deltaTime) {

    for(auto& pair :
        fragments_) {

        pair.second.persistence -=
            deltaTime * 0.0005f;

        if(pair.second.persistence <
            0.0f) {

            pair.second.persistence =
                0.0f;
        }
    }
}

void
RealityCoherenceEngine::
update(
    float deltaTime) {

    decayRealityFragments(
        deltaTime);

    computeRealityCoherence();

    computePerceptualIntegrity();

    computeSubconsciousConsistency();

    computeSensorySynchronization();

    computeSubjectivePresence();

    computeImmersionLock();

    computeHallucinationRisk();

    computePerceptualDrift();

    stabilizeRealityMesh();
}

const RealityCoherenceState&
RealityCoherenceEngine::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    RealityFragment>&
RealityCoherenceEngine::
getFragments() const {

    return fragments_;
}

}