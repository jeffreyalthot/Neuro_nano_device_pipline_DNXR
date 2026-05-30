#include "SubjectiveTimeEngine.h"

#include <cmath>

namespace neuroxr {

SubjectiveTimeEngine::
SubjectiveTimeEngine() {

    initialize();
}

void SubjectiveTimeEngine::
initialize() {

    state_.currentTimeScale = 1.0f;

    state_.perceivedDuration = 0.0f;

    state_.temporalCoherence = 1.0f;

    state_.temporalDistortion = 0.0f;

    state_.immersionTimeDilation = 0.0f;
}

void SubjectiveTimeEngine::
pushFrame(
    const SubjectiveTimeFrame& frame) {

    frames_.push_back(frame);

    if(frames_.size() > 4096) {

        frames_.pop_front();
    }
}

void SubjectiveTimeEngine::
computeTimeScale() {

    if(frames_.empty())
        return;

    float scale = 0.0f;

    for(const auto& frame :
        frames_) {

        scale +=
            frame.subjectiveDeltaTime /
            frame.realDeltaTime;
    }

    scale /=
        static_cast<float>(
            frames_.size());

    state_.currentTimeScale = scale;
}

void SubjectiveTimeEngine::
computeTemporalCoherence() {

    if(frames_.size() < 2) {

        state_.temporalCoherence = 1.0f;
        return;
    }

    float coherence = 0.0f;

    for(size_t i = 1;
        i < frames_.size();
        ++i) {

        coherence +=
            1.0f -
            std::abs(
                frames_[i].subjectiveDeltaTime -
                frames_[i - 1].subjectiveDeltaTime);
    }

    coherence /=
        static_cast<float>(
            frames_.size() - 1);

    state_.temporalCoherence = coherence;
}

void SubjectiveTimeEngine::
computeTemporalDistortion() {

    state_.temporalDistortion =
        std::abs(
            state_.currentTimeScale -
            1.0f);
}

void SubjectiveTimeEngine::
computeImmersionDilation() {

    state_.immersionTimeDilation =
        state_.temporalDistortion *
        state_.temporalCoherence;
}

void SubjectiveTimeEngine::
stabilizeTemporalFlow() {

    if(state_.currentTimeScale < 0.1f) {
        state_.currentTimeScale = 0.1f;
    }

    if(state_.currentTimeScale > 10.0f) {
        state_.currentTimeScale = 10.0f;
    }
}

void SubjectiveTimeEngine::
update(
    float deltaTime) {

    computeTimeScale();

    computeTemporalCoherence();

    computeTemporalDistortion();

    computeImmersionDilation();

    stabilizeTemporalFlow();

    state_.perceivedDuration +=
        deltaTime *
        state_.currentTimeScale;
}

const SubjectiveTemporalState&
SubjectiveTimeEngine::
getState() const {

    return state_;
}

}