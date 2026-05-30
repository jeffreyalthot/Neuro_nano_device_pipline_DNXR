#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

enum class DreamLayer {

    LIGHT_DREAM,
    DEEP_DREAM,
    LUCID_DREAM,
    SUBCONSCIOUS_SIMULATION,
    MEMORY_RECONSTRUCTION,
    PREDICTIVE_DREAMING
};

struct DreamSignal {

    DreamLayer layer;

    float intensity;

    float realism;

    float emotionalInfluence;

    float subconsciousDepth;

    float temporalDistortion;

    float narrativeCoherence;
};

struct DreamSequence {

    uint64_t id;

    std::string title;

    std::vector<DreamSignal>
    signals;

    float stability;

    float persistence;

    float immersionLevel;

    float memoryFusion;
};

struct DreamingState {

    float dreamIntensity;

    float lucidAwareness;

    float subconsciousSynchronization;

    float narrativeContinuity;

    float emotionalDreamBinding;

    float realitySeparation;

    float predictiveSimulationStrength;
};

class ConsciousDreamingEngine {
public:

    ConsciousDreamingEngine();

    void initialize();

    uint64_t createDreamSequence(
        const std::string& title);

    void injectDreamSignal(
        uint64_t sequenceId,
        const DreamSignal& signal);

    void computeDreamIntensity();

    void computeLucidAwareness();

    void computeSubconsciousSynchronization();

    void computeNarrativeContinuity();

    void computeEmotionalDreamBinding();

    void computeRealitySeparation();

    void computePredictiveSimulationStrength();

    void stabilizeDreamSpace();

    void decayDreamSequences(
        float deltaTime);

    void update(
        float deltaTime);

    const DreamingState&
    getState() const;

private:

    uint64_t nextSequenceId_;

    std::unordered_map<
        uint64_t,
        DreamSequence>
    sequences_;

    DreamingState
    state_;
};

}