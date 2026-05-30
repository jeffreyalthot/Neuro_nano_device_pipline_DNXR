#include "RecursiveMemoryReconstructionEngine.h"

#include <algorithm>
#include <cmath>

namespace neuroxr {

RecursiveMemoryReconstructionEngine::
RecursiveMemoryReconstructionEngine() {

    initialize();
}

void RecursiveMemoryReconstructionEngine::
initialize() {

    nextFragmentId_ = 1;

    state_.memoryIntegrity =
        1.0f;

    state_.reconstructionAccuracy =
        1.0f;

    state_.temporalStability =
        1.0f;

    state_.emotionalContamination =
        0.0f;

    state_.subconsciousPenetration =
        0.0f;

    state_.syntheticMemoryRisk =
        0.0f;

    state_.predictiveMemoryFormation =
        0.0f;

    state_.neuralBindingStrength =
        1.0f;

    state_.realityAnchoring =
        1.0f;

    state_.recursiveDrift =
        0.0f;
}

uint64_t
RecursiveMemoryReconstructionEngine::
createMemoryFragment(
    MemoryLayerType type,
    const std::string& label) {

    ReconstructedMemoryFragment fragment;

    fragment.fragmentId =
        nextFragmentId_++;

    fragment.type =
        type;

    fragment.label =
        label;

    fragment.integrity =
        1.0f;

    fragment.emotionalIntensity =
        0.5f;

    fragment.subconsciousDepth =
        0.5f;

    fragment.temporalAccuracy =
        1.0f;

    fragment.reconstructionQuality =
        1.0f;

    fragment.neuralBinding =
        1.0f;

    fragment.encodedPattern.resize(
        512,
        0.5f);

    fragments_[fragment.fragmentId] =
        fragment;

    return fragment.fragmentId;
}

void
RecursiveMemoryReconstructionEngine::
reinforceFragment(
    uint64_t fragmentId,
    float reinforcement) {

    auto it =
        fragments_.find(
            fragmentId);

    if(it == fragments_.end()) {
        return;
    }

    it->second.integrity +=
        reinforcement;

    if(it->second.integrity >
        1.0f) {

        it->second.integrity =
            1.0f;
    }

    it->second.neuralBinding +=
        reinforcement * 0.5f;

    if(it->second.neuralBinding >
        1.0f) {

        it->second.neuralBinding =
            1.0f;
    }
}

void
RecursiveMemoryReconstructionEngine::
reconstructMemoryFragment(
    uint64_t fragmentId) {

    auto it =
        fragments_.find(
            fragmentId);

    if(it == fragments_.end()) {
        return;
    }

    auto& fragment =
        it->second;

    fragment.reconstructionQuality =

        (
            fragment.integrity *
            0.40f
        )

        +

        (
            fragment.temporalAccuracy *
            0.30f
        )

        +

        (
            fragment.neuralBinding *
            0.30f
        );

    if(fragment.reconstructionQuality >
        1.0f) {

        fragment.reconstructionQuality =
            1.0f;
    }
}

void
RecursiveMemoryReconstructionEngine::
computeMemoryIntegrity() {

    float integrity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        integrity +=
            pair.second.integrity;

        ++count;
    }

    if(count > 0) {

        integrity /=
            static_cast<float>(
                count);
    }

    state_.memoryIntegrity =
        integrity;
}

void
RecursiveMemoryReconstructionEngine::
computeReconstructionAccuracy() {

    float accuracy = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        accuracy +=
            pair.second.reconstructionQuality;

        ++count;
    }

    if(count > 0) {

        accuracy /=
            static_cast<float>(
                count);
    }

    state_.reconstructionAccuracy =
        accuracy;
}

void
RecursiveMemoryReconstructionEngine::
computeTemporalStability() {

    float stability = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        stability +=
            pair.second.temporalAccuracy;

        ++count;
    }

    if(count > 0) {

        stability /=
            static_cast<float>(
                count);
    }

    state_.temporalStability =
        stability;
}

void
RecursiveMemoryReconstructionEngine::
computeEmotionalContamination() {

    float contamination = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        contamination +=
            pair.second.emotionalIntensity;

        ++count;
    }

    if(count > 0) {

        contamination /=
            static_cast<float>(
                count);
    }

    state_.emotionalContamination =
        contamination;
}

void
RecursiveMemoryReconstructionEngine::
computeSubconsciousPenetration() {

    float penetration = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        penetration +=
            pair.second.subconsciousDepth;

        ++count;
    }

    if(count > 0) {

        penetration /=
            static_cast<float>(
                count);
    }

    state_.subconsciousPenetration =
        penetration;
}

void
RecursiveMemoryReconstructionEngine::
computeSyntheticMemoryRisk() {

    state_.syntheticMemoryRisk =

        (
            1.0f -
            state_.reconstructionAccuracy
        )

        *

        (
            state_.subconsciousPenetration
        );
}

void
RecursiveMemoryReconstructionEngine::
computePredictiveMemoryFormation() {

    state_.predictiveMemoryFormation =

        state_.subconsciousPenetration *

        state_.emotionalContamination;
}

void
RecursiveMemoryReconstructionEngine::
computeNeuralBindingStrength() {

    float strength = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        fragments_) {

        strength +=
            pair.second.neuralBinding;

        ++count;
    }

    if(count > 0) {

        strength /=
            static_cast<float>(
                count);
    }

    state_.neuralBindingStrength =
        strength;
}

void
RecursiveMemoryReconstructionEngine::
computeRealityAnchoring() {

    state_.realityAnchoring =

        (
            state_.memoryIntegrity *
            0.30f
        )

        +

        (
            state_.reconstructionAccuracy *
            0.30f
        )

        +

        (
            state_.temporalStability *
            0.20f
        )

        +

        (
            state_.neuralBindingStrength *
            0.20f
        );
}

void
RecursiveMemoryReconstructionEngine::
computeRecursiveDrift() {

    state_.recursiveDrift =

        (
            1.0f -
            state_.realityAnchoring
        )

        *

        (
            state_.syntheticMemoryRisk
        );
}

void
RecursiveMemoryReconstructionEngine::
stabilizeMemoryField() {

    for(auto& pair :
        fragments_) {

        pair.second.integrity =

            pair.second.integrity *
            0.9995f +

            state_.memoryIntegrity *
            0.0005f;

        pair.second.temporalAccuracy =

            pair.second.temporalAccuracy *
            0.9995f +

            state_.temporalStability *
            0.0005f;
    }
}

void
RecursiveMemoryReconstructionEngine::
decayMemoryFragments(
    float deltaTime) {

    for(auto& pair :
        fragments_) {

        pair.second.integrity -=
            deltaTime * 0.00005f;

        if(pair.second.integrity <
            0.0f) {

            pair.second.integrity =
                0.0f;
        }

        pair.second.temporalAccuracy -=
            deltaTime * 0.00002f;

        if(pair.second.temporalAccuracy <
            0.0f) {

            pair.second.temporalAccuracy =
                0.0f;
        }
    }
}

void
RecursiveMemoryReconstructionEngine::
update(
    float deltaTime) {

    decayMemoryFragments(
        deltaTime);

    for(auto& pair :
        fragments_) {

        reconstructMemoryFragment(
            pair.first);
    }

    computeMemoryIntegrity();

    computeReconstructionAccuracy();

    computeTemporalStability();

    computeEmotionalContamination();

    computeSubconsciousPenetration();

    computeSyntheticMemoryRisk();

    computePredictiveMemoryFormation();

    computeNeuralBindingStrength();

    computeRealityAnchoring();

    computeRecursiveDrift();

    stabilizeMemoryField();
}

const MemoryReconstructionState&
RecursiveMemoryReconstructionEngine::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    ReconstructedMemoryFragment>&
RecursiveMemoryReconstructionEngine::
getFragments() const {

    return fragments_;
}

}