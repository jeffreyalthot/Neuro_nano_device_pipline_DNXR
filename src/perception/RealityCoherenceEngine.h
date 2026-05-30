#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

enum class PerceptualLayer {

    VISUAL,
    AUDITORY,
    TACTILE,
    OLFACTORY,
    GUSTATORY,
    VESTIBULAR,
    PROPRIOCEPTIVE,
    COGNITIVE
};

struct PerceptualSignal {

    PerceptualLayer layer;

    float signalStrength;

    float realism;

    float latency;

    float coherence;

    float subconsciousBinding;

    float neuralAlignment;
};

struct RealityFragment {

    uint64_t id;

    std::string label;

    std::vector<PerceptualSignal>
    signals;

    float persistence;

    float continuity;

    float immersionWeight;

    float temporalAnchor;
};

struct RealityCoherenceState {

    float globalRealityCoherence;

    float perceptualIntegrity;

    float subconsciousConsistency;

    float sensorySynchronization;

    float subjectivePresence;

    float immersionLock;

    float hallucinationRisk;

    float perceptualDrift;
};

class RealityCoherenceEngine {
public:

    RealityCoherenceEngine();

    void initialize();

    uint64_t createRealityFragment(
        const std::string& label);

    void injectPerceptualSignal(
        uint64_t fragmentId,
        const PerceptualSignal& signal);

    void computeRealityCoherence();

    void computePerceptualIntegrity();

    void computeSubconsciousConsistency();

    void computeSensorySynchronization();

    void computeSubjectivePresence();

    void computeImmersionLock();

    void computeHallucinationRisk();

    void computePerceptualDrift();

    void stabilizeRealityMesh();

    void decayRealityFragments(
        float deltaTime);

    void update(
        float deltaTime);

    const RealityCoherenceState&
    getState() const;

    const std::unordered_map<
        uint64_t,
        RealityFragment>&
    getFragments() const;

private:

    float computeSignalCompatibility(
        const PerceptualSignal& a,
        const PerceptualSignal& b) const;

private:

    uint64_t nextFragmentId_;

    std::unordered_map<
        uint64_t,
        RealityFragment>
    fragments_;

    RealityCoherenceState
    state_;
};

}