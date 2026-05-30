#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

enum class TemporalLayerType {

    REALTIME,
    ACCELERATED,
    DECELERATED,
    PARALLEL,
    FRACTURED,
    LOOPING,
    PREDICTIVE,
    SUBCONSCIOUS
};

struct TemporalFrame {

    uint64_t frameId;

    TemporalLayerType type;

    float timeScale;

    float temporalCoherence;

    float subjectiveExpansion;

    float memoryPersistence;

    float predictiveLatency;

    float consciousnessAlignment;
};

struct TemporalRealityState {

    float globalTimeScale;

    float temporalSynchronization;

    float subjectiveTimeExpansion;

    float realityTimeAlignment;

    float temporalFragmentation;

    float predictiveTemporalDrift;

    float consciousnessTimeCoupling;

    float memoryTemporalPersistence;

    float temporalLoopRisk;

    float temporalStability;
};

class SubjectiveTimeDilationEngine {
public:

    SubjectiveTimeDilationEngine();

    void initialize();

    uint64_t createTemporalFrame(
        TemporalLayerType type,
        float timeScale);

    void distortTemporalFrame(
        uint64_t frameId,
        float distortionFactor);

    void computeGlobalTimeScale();

    void computeTemporalSynchronization();

    void computeSubjectiveTimeExpansion();

    void computeRealityTimeAlignment();

    void computeTemporalFragmentation();

    void computePredictiveTemporalDrift();

    void computeConsciousnessTimeCoupling();

    void computeMemoryTemporalPersistence();

    void computeTemporalLoopRisk();

    void computeTemporalStability();

    void stabilizeTemporalField();

    void decayTemporalFrames(
        float deltaTime);

    void update(
        float deltaTime);

    const TemporalRealityState&
    getState() const;

    const std::unordered_map<
        uint64_t,
        TemporalFrame>&
    getFrames() const;

private:

    uint64_t nextFrameId_;

    std::unordered_map<
        uint64_t,
        TemporalFrame>
    frames_;

    TemporalRealityState
    state_;
};

}