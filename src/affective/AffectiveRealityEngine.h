#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnxr {

enum class EmotionalStateType {

    CALM,
    FEAR,
    JOY,
    SADNESS,
    ANGER,
    EXCITEMENT,
    IMMERSION,
    DISSOCIATION
};

struct EmotionalVector {

    EmotionalStateType type;

    float intensity;

    float persistence;

    float neuralInfluence;

    float hormonalWeight;
};

struct AffectiveFrame {

    std::vector<EmotionalVector> emotions;

    float emotionalCoherence;

    float affectivePresence;

    float emotionalDrift;

    float cognitiveResonance;
};

class AffectiveRealityEngine {
public:

    AffectiveRealityEngine();

    void initialize();

    void injectEmotion(
        EmotionalStateType type,
        float intensity,
        float persistence);

    void update(
        float deltaTime);

    void computeEmotionalCoherence();

    void computeAffectivePresence();

    void computeCognitiveResonance();

    void decayEmotionalStates(
        float deltaTime);

    void stabilizeEmotionalFeedback();

    const AffectiveFrame&
    getFrame() const;

private:

    float computeEmotionCompatibility(
        EmotionalStateType a,
        EmotionalStateType b) const;

private:

    AffectiveFrame frame_;
};

}

namespace neuroxr {
using namespace ::dnxr;
}
