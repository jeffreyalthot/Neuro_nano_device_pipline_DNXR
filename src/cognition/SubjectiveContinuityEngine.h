#pragma once

#include <deque>
#include <vector>

namespace dnxr {

struct ContinuityFrame {

    std::vector<float> perceptionData;

    float emotionalWeight;

    float temporalWeight;

    float identityWeight;
};

struct SubjectiveState {

    float continuity;

    float identityStability;

    float temporalCoherence;

    float immersionDepth;

    float dissociationRisk;
};

class SubjectiveContinuityEngine {
public:

    SubjectiveContinuityEngine();

    void initialize();

    void pushPerceptionFrame(
        const ContinuityFrame& frame);

    void update(float deltaTime);

    void computeIdentityStability();

    void computeTemporalCoherence();

    void computeImmersionDepth();

    void computeDissociationRisk();

    const SubjectiveState&
    getState() const;

private:

    float computeFrameSimilarity(
        const ContinuityFrame& a,
        const ContinuityFrame& b) const;

private:

    std::deque<ContinuityFrame> frameHistory_;

    SubjectiveState state_;
};

}
