#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

enum class AffectiveSpectrum {

    JOY,
    FEAR,
    ANGER,
    SADNESS,
    CURIOSITY,
    EMPATHY,
    EUPHORIA,
    MELANCHOLY,
    TRANSCENDENCE
};

struct AffectiveSignal {

    uint64_t signalId;

    AffectiveSpectrum spectrum;

    float intensity;

    float stability;

    float subconsciousPenetration;

    float memoryAssociation;

    float socialProjection;

    float cognitiveInfluence;
};

struct AffectiveFieldState {

    float emotionalCoherence;

    float emotionalSynchronization;

    float affectivePressure;

    float subconsciousEmotionalLoad;

    float empathicResonance;

    float emotionalDrift;

    float socialEmotionalCoupling;

    float immersiveEmotionalBinding;
};

class AffectiveResonanceMatrix {
public:

    AffectiveResonanceMatrix();

    void initialize();

    uint64_t createAffectiveSignal(
        AffectiveSpectrum spectrum);

    void amplifySignal(
        uint64_t signalId,
        float intensity);

    void computeEmotionalCoherence();

    void computeEmotionalSynchronization();

    void computeAffectivePressure();

    void computeSubconsciousEmotionalLoad();

    void computeEmpathicResonance();

    void computeEmotionalDrift();

    void computeSocialEmotionalCoupling();

    void computeImmersiveEmotionalBinding();

    void stabilizeEmotionalField();

    void decaySignals(
        float deltaTime);

    void update(
        float deltaTime);

    const AffectiveFieldState&
    getState() const;

private:

    uint64_t nextSignalId_;

    std::unordered_map<
        uint64_t,
        AffectiveSignal>
    signals_;

    AffectiveFieldState
    state_;
};

}