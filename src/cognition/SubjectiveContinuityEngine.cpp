#include "SubjectiveContinuityEngine.h"

#include <cmath>

namespace dnxr {

SubjectiveContinuityEngine::
SubjectiveContinuityEngine() {

    initialize();
}

void SubjectiveContinuityEngine::
initialize() {

    state_.continuity = 1.0f;

    state_.identityStability = 1.0f;

    state_.temporalCoherence = 1.0f;

    state_.immersionDepth = 0.0f;

    state_.dissociationRisk = 0.0f;
}

void SubjectiveContinuityEngine::
pushPerceptionFrame(
    const ContinuityFrame& frame) {

    frameHistory_.push_back(frame);

    if(frameHistory_.size() > 2048) {
        frameHistory_.pop_front();
    }
}

float SubjectiveContinuityEngine::
computeFrameSimilarity(
    const ContinuityFrame& a,
    const ContinuityFrame& b) const {

    size_t count =
        std::min(
            a.perceptionData.size(),
            b.perceptionData.size());

    if(count == 0)
        return 0.0f;

    float similarity = 0.0f;

    for(size_t i = 0; i < count; ++i) {

        similarity +=
            1.0f -
            std::abs(
                a.perceptionData[i] -
                b.perceptionData[i]);
    }

    return similarity /
        static_cast<float>(count);
}

void SubjectiveContinuityEngine::
computeIdentityStability() {

    if(frameHistory_.size() < 2) {
        state_.identityStability = 1.0f;
        return;
    }

    const auto& latest =
        frameHistory_[frameHistory_.size() - 1];

    const auto& previous =
        frameHistory_[frameHistory_.size() - 2];

    state_.identityStability =
        computeFrameSimilarity(
            latest,
            previous);
}

void SubjectiveContinuityEngine::
computeTemporalCoherence() {

    if(frameHistory_.empty()) {

        state_.temporalCoherence = 0.0f;
        return;
    }

    float coherence = 0.0f;

    for(size_t i = 1;
        i < frameHistory_.size();
        ++i) {

        coherence +=
            computeFrameSimilarity(
                frameHistory_[i - 1],
                frameHistory_[i]);
    }

    state_.temporalCoherence =
        coherence /
        static_cast<float>(
            frameHistory_.size() - 1);
}

void SubjectiveContinuityEngine::
computeImmersionDepth() {

    state_.immersionDepth =
        (state_.identityStability * 0.4f) +
        (state_.temporalCoherence * 0.6f);
}

void SubjectiveContinuityEngine::
computeDissociationRisk() {

    state_.dissociationRisk =
        1.0f -
        state_.immersionDepth;

    if(state_.dissociationRisk < 0.0f)
        state_.dissociationRisk = 0.0f;
}

void SubjectiveContinuityEngine::
update(float deltaTime) {
    (void)deltaTime;

    computeIdentityStability();

    computeTemporalCoherence();

    computeImmersionDepth();

    computeDissociationRisk();

    state_.continuity =
        state_.immersionDepth *
        (1.0f -
        state_.dissociationRisk);
}

const SubjectiveState&
SubjectiveContinuityEngine::getState() const {

    return state_;
}

}