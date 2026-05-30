#include "AffectiveRealityEngine.h"

#include <algorithm>
#include <cmath>

namespace neuroxr {

AffectiveRealityEngine::
AffectiveRealityEngine() {

    initialize();
}

void AffectiveRealityEngine::
initialize() {

    frame_.emotionalCoherence = 1.0f;

    frame_.affectivePresence = 0.0f;

    frame_.emotionalDrift = 0.0f;

    frame_.cognitiveResonance = 1.0f;
}

void AffectiveRealityEngine::
injectEmotion(
    EmotionalStateType type,
    float intensity,
    float persistence) {

    EmotionalVector emotion;

    emotion.type = type;

    emotion.intensity = intensity;

    emotion.persistence = persistence;

    emotion.neuralInfluence =
        intensity * 0.85f;

    emotion.hormonalWeight =
        intensity * persistence;

    frame_.emotions.push_back(emotion);
}

float AffectiveRealityEngine::
computeEmotionCompatibility(
    EmotionalStateType a,
    EmotionalStateType b) const {

    if(a == b)
        return 1.0f;

    if(
        (a == EmotionalStateType::JOY &&
         b == EmotionalStateType::EXCITEMENT)

        ||

        (a == EmotionalStateType::CALM &&
         b == EmotionalStateType::IMMERSION)
    ) {
        return 0.85f;
    }

    return 0.35f;
}

void AffectiveRealityEngine::
computeEmotionalCoherence() {

    if(frame_.emotions.empty()) {

        frame_.emotionalCoherence = 0.0f;
        return;
    }

    float coherence = 0.0f;

    uint32_t comparisons = 0;

    for(size_t i = 0;
        i < frame_.emotions.size();
        ++i) {

        for(size_t j = i + 1;
            j < frame_.emotions.size();
            ++j) {

            coherence +=
                computeEmotionCompatibility(
                    frame_.emotions[i].type,
                    frame_.emotions[j].type);

            ++comparisons;
        }
    }

    if(comparisons > 0) {

        coherence /=
            static_cast<float>(
                comparisons);
    }

    frame_.emotionalCoherence = coherence;
}

void AffectiveRealityEngine::
computeAffectivePresence() {

    float presence = 0.0f;

    for(const auto& emotion :
        frame_.emotions) {

        presence +=
            emotion.intensity *
            emotion.persistence;
    }

    if(!frame_.emotions.empty()) {

        presence /=
            static_cast<float>(
                frame_.emotions.size());
    }

    frame_.affectivePresence = presence;
}

void AffectiveRealityEngine::
computeCognitiveResonance() {

    frame_.cognitiveResonance =
        (frame_.affectivePresence * 0.6f) +
        (frame_.emotionalCoherence * 0.4f);
}

void AffectiveRealityEngine::
decayEmotionalStates(
    float deltaTime) {

    for(auto& emotion :
        frame_.emotions) {

        emotion.intensity -=
            deltaTime *
            (1.0f -
            emotion.persistence) *
            0.05f;

        if(emotion.intensity < 0.0f)
            emotion.intensity = 0.0f;
    }
}

void AffectiveRealityEngine::
stabilizeEmotionalFeedback() {

    float drift = 0.0f;

    for(const auto& emotion :
        frame_.emotions) {

        drift +=
            std::abs(
                emotion.intensity -
                emotion.persistence);
    }

    if(!frame_.emotions.empty()) {

        drift /=
            static_cast<float>(
                frame_.emotions.size());
    }

    frame_.emotionalDrift = drift;
}

void AffectiveRealityEngine::
update(
    float deltaTime) {

    decayEmotionalStates(
        deltaTime);

    computeEmotionalCoherence();

    computeAffectivePresence();

    computeCognitiveResonance();

    stabilizeEmotionalFeedback();
}

const AffectiveFrame&
AffectiveRealityEngine::
getFrame() const {

    return frame_;
}

}