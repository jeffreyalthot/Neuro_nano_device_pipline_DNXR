#pragma once

#include <deque>
#include <vector>

namespace neuroxr {

struct SubjectiveTimeFrame {

    float realDeltaTime;

    float subjectiveDeltaTime;

    float cognitiveDensity;

    float immersionFactor;

    float temporalCompression;

    float temporalExpansion;
};

struct SubjectiveTemporalState {

    float currentTimeScale;

    float perceivedDuration;

    float temporalCoherence;

    float temporalDistortion;

    float immersionTimeDilation;
};

class SubjectiveTimeEngine {
public:

    SubjectiveTimeEngine();

    void initialize();

    void pushFrame(
        const SubjectiveTimeFrame& frame);

    void update(
        float deltaTime);

    void computeTimeScale();

    void computeTemporalCoherence();

    void computeTemporalDistortion();

    void computeImmersionDilation();

    void stabilizeTemporalFlow();

    const SubjectiveTemporalState&
    getState() const;

private:

    std::deque<SubjectiveTimeFrame>
    frames_;

    SubjectiveTemporalState state_;
};

}