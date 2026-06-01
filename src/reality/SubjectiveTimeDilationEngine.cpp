#include "SubjectiveTimeDilationEngine.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

SubjectiveTimeDilationEngine::
SubjectiveTimeDilationEngine() {

    initialize();
}

void SubjectiveTimeDilationEngine::
initialize() {

    nextFrameId_ = 1;

    state_.globalTimeScale =
        1.0f;

    state_.temporalSynchronization =
        1.0f;

    state_.subjectiveTimeExpansion =
        0.0f;

    state_.realityTimeAlignment =
        1.0f;

    state_.temporalFragmentation =
        0.0f;

    state_.predictiveTemporalDrift =
        0.0f;

    state_.consciousnessTimeCoupling =
        1.0f;

    state_.memoryTemporalPersistence =
        1.0f;

    state_.temporalLoopRisk =
        0.0f;

    state_.temporalStability =
        1.0f;
}

uint64_t
SubjectiveTimeDilationEngine::
createTemporalFrame(
    TemporalLayerType type,
    float timeScale) {

    TemporalFrame frame;

    frame.frameId =
        nextFrameId_++;

    frame.type =
        type;

    frame.timeScale =
        timeScale;

    frame.temporalCoherence =
        1.0f;

    frame.subjectiveExpansion =
        1.0f;

    frame.memoryPersistence =
        1.0f;

    frame.predictiveLatency =
        0.0f;

    frame.consciousnessAlignment =
        1.0f;

    frames_[frame.frameId] =
        frame;

    return frame.frameId;
}

void
SubjectiveTimeDilationEngine::
distortTemporalFrame(
    uint64_t frameId,
    float distortionFactor) {

    auto it =
        frames_.find(
            frameId);

    if(it == frames_.end()) {
        return;
    }

    it->second.timeScale *=
        distortionFactor;

    if(it->second.timeScale >
        1000.0f) {

        it->second.timeScale =
            1000.0f;
    }

    if(it->second.timeScale <
        0.0001f) {

        it->second.timeScale =
            0.0001f;
    }
}

void
SubjectiveTimeDilationEngine::
computeGlobalTimeScale() {

    float scale = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        scale +=
            pair.second.timeScale;

        ++count;
    }

    if(count > 0) {

        scale /=
            static_cast<float>(
                count);
    }

    state_.globalTimeScale =
        scale;
}

void
SubjectiveTimeDilationEngine::
computeTemporalSynchronization() {

    float synchronization = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        synchronization +=
            pair.second.temporalCoherence;

        ++count;
    }

    if(count > 0) {

        synchronization /=
            static_cast<float>(
                count);
    }

    state_.temporalSynchronization =
        synchronization;
}

void
SubjectiveTimeDilationEngine::
computeSubjectiveTimeExpansion() {

    state_.subjectiveTimeExpansion =

        state_.globalTimeScale *

        state_.consciousnessTimeCoupling;
}

void
SubjectiveTimeDilationEngine::
computeRealityTimeAlignment() {

    state_.realityTimeAlignment =

        state_.temporalSynchronization *

        (1.0f -
        state_.temporalFragmentation);
}

void
SubjectiveTimeDilationEngine::
computeTemporalFragmentation() {

    float fragmentation = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        fragmentation +=

            std::abs(
                pair.second.timeScale -
                state_.globalTimeScale);

        ++count;
    }

    if(count > 0) {

        fragmentation /=
            static_cast<float>(
                count);
    }

    state_.temporalFragmentation =
        fragmentation * 0.001f;
}

void
SubjectiveTimeDilationEngine::
computePredictiveTemporalDrift() {

    float drift = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        drift +=
            pair.second.predictiveLatency;

        ++count;
    }

    if(count > 0) {

        drift /=
            static_cast<float>(
                count);
    }

    state_.predictiveTemporalDrift =
        drift;
}

void
SubjectiveTimeDilationEngine::
computeConsciousnessTimeCoupling() {

    float coupling = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        coupling +=
            pair.second.consciousnessAlignment;

        ++count;
    }

    if(count > 0) {

        coupling /=
            static_cast<float>(
                count);
    }

    state_.consciousnessTimeCoupling =
        coupling;
}

void
SubjectiveTimeDilationEngine::
computeMemoryTemporalPersistence() {

    float persistence = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        frames_) {

        persistence +=
            pair.second.memoryPersistence;

        ++count;
    }

    if(count > 0) {

        persistence /=
            static_cast<float>(
                count);
    }

    state_.memoryTemporalPersistence =
        persistence;
}

void
SubjectiveTimeDilationEngine::
computeTemporalLoopRisk() {

    state_.temporalLoopRisk =

        state_.temporalFragmentation *

        (1.0f -
        state_.realityTimeAlignment);
}

void
SubjectiveTimeDilationEngine::
computeTemporalStability() {

    state_.temporalStability =

        (
            state_.temporalSynchronization *
            0.30f
        )

        +

        (
            state_.realityTimeAlignment *
            0.30f
        )

        +

        (
            state_.memoryTemporalPersistence *
            0.20f
        )

        +

        (
            state_.consciousnessTimeCoupling *
            0.20f
        );
}

void
SubjectiveTimeDilationEngine::
stabilizeTemporalField() {

    for(auto& pair :
        frames_) {

        pair.second.temporalCoherence =

            state_.temporalSynchronization;

        pair.second.consciousnessAlignment =

            state_.consciousnessTimeCoupling;
    }
}

void
SubjectiveTimeDilationEngine::
decayTemporalFrames(
    float deltaTime) {

    for(auto& pair :
        frames_) {

        pair.second.memoryPersistence -=
            deltaTime * 0.00002f;

        if(pair.second.memoryPersistence <
            0.0f) {

            pair.second.memoryPersistence =
                0.0f;
        }

        pair.second.temporalCoherence -=
            deltaTime * 0.00001f;

        if(pair.second.temporalCoherence <
            0.0f) {

            pair.second.temporalCoherence =
                0.0f;
        }
    }
}

void
SubjectiveTimeDilationEngine::
update(
    float deltaTime) {

    decayTemporalFrames(
        deltaTime);

    computeGlobalTimeScale();

    computeTemporalSynchronization();

    computeConsciousnessTimeCoupling();

    computeSubjectiveTimeExpansion();

    computeTemporalFragmentation();

    computeRealityTimeAlignment();

    computePredictiveTemporalDrift();

    computeMemoryTemporalPersistence();

    computeTemporalLoopRisk();

    computeTemporalStability();

    stabilizeTemporalField();
}

const TemporalRealityState&
SubjectiveTimeDilationEngine::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    TemporalFrame>&
SubjectiveTimeDilationEngine::
getFrames() const {

    return frames_;
}

}