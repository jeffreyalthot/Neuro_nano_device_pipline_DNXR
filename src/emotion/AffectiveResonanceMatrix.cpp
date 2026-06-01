#include "AffectiveResonanceMatrix.h"

#include <cmath>

namespace dnxr {

AffectiveResonanceMatrix::
AffectiveResonanceMatrix() {

    initialize();
}

void AffectiveResonanceMatrix::
initialize() {

    nextSignalId_ = 1;

    state_.emotionalCoherence =
        1.0f;

    state_.emotionalSynchronization =
        1.0f;

    state_.affectivePressure =
        0.0f;

    state_.subconsciousEmotionalLoad =
        0.0f;

    state_.empathicResonance =
        0.0f;

    state_.emotionalDrift =
        0.0f;

    state_.socialEmotionalCoupling =
        0.0f;

    state_.immersiveEmotionalBinding =
        0.0f;
}

uint64_t
AffectiveResonanceMatrix::
createAffectiveSignal(
    AffectiveSpectrum spectrum) {

    AffectiveSignal signal;

    signal.signalId =
        nextSignalId_++;

    signal.spectrum =
        spectrum;

    signal.intensity =
        0.5f;

    signal.stability =
        1.0f;

    signal.subconsciousPenetration =
        0.5f;

    signal.memoryAssociation =
        0.5f;

    signal.socialProjection =
        0.5f;

    signal.cognitiveInfluence =
        0.5f;

    signals_[signal.signalId] =
        signal;

    return signal.signalId;
}

void
AffectiveResonanceMatrix::
amplifySignal(
    uint64_t signalId,
    float intensity) {

    auto it =
        signals_.find(
            signalId);

    if(it == signals_.end()) {
        return;
    }

    it->second.intensity +=
        intensity;

    if(it->second.intensity >
        1.0f) {

        it->second.intensity =
            1.0f;
    }
}

void
AffectiveResonanceMatrix::
computeEmotionalCoherence() {

    float coherence = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        signals_) {

        coherence +=
            pair.second.stability;

        ++count;
    }

    if(count > 0) {

        coherence /=
            static_cast<float>(
                count);
    }

    state_.emotionalCoherence =
        coherence;
}

void
AffectiveResonanceMatrix::
computeEmotionalSynchronization() {

    float synchronization = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        signals_) {

        synchronization +=
            pair.second.intensity;

        ++count;
    }

    if(count > 0) {

        synchronization /=
            static_cast<float>(
                count);
    }

    state_.emotionalSynchronization =
        synchronization;
}

void
AffectiveResonanceMatrix::
computeAffectivePressure() {

    state_.affectivePressure =

        state_.emotionalSynchronization *

        (1.0f -
        state_.emotionalCoherence);
}

void
AffectiveResonanceMatrix::
computeSubconsciousEmotionalLoad() {

    float load = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        signals_) {

        load +=
            pair.second
            .subconsciousPenetration;

        ++count;
    }

    if(count > 0) {

        load /=
            static_cast<float>(
                count);
    }

    state_.subconsciousEmotionalLoad =
        load;
}

void
AffectiveResonanceMatrix::
computeEmpathicResonance() {

    float resonance = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        signals_) {

        resonance +=
            pair.second.socialProjection;

        ++count;
    }

    if(count > 0) {

        resonance /=
            static_cast<float>(
                count);
    }

    state_.empathicResonance =
        resonance;
}

void
AffectiveResonanceMatrix::
computeEmotionalDrift() {

    state_.emotionalDrift =

        1.0f -
        state_.emotionalSynchronization;
}

void
AffectiveResonanceMatrix::
computeSocialEmotionalCoupling() {

    state_.socialEmotionalCoupling =

        state_.empathicResonance *

        state_.emotionalSynchronization;
}

void
AffectiveResonanceMatrix::
computeImmersiveEmotionalBinding() {

    state_.immersiveEmotionalBinding =

        state_.socialEmotionalCoupling *

        state_.subconsciousEmotionalLoad;
}

void
AffectiveResonanceMatrix::
stabilizeEmotionalField() {

    for(auto& pair :
        signals_) {

        pair.second.stability =

            state_.emotionalCoherence;

        pair.second.cognitiveInfluence =

            state_.immersiveEmotionalBinding;
    }
}

void
AffectiveResonanceMatrix::
decaySignals(
    float deltaTime) {

    for(auto& pair :
        signals_) {

        pair.second.intensity -=
            deltaTime * 0.0001f;

        if(pair.second.intensity <
            0.0f) {

            pair.second.intensity =
                0.0f;
        }
    }
}

void
AffectiveResonanceMatrix::
update(
    float deltaTime) {

    decaySignals(
        deltaTime);

    computeEmotionalCoherence();

    computeEmotionalSynchronization();

    computeAffectivePressure();

    computeSubconsciousEmotionalLoad();

    computeEmpathicResonance();

    computeEmotionalDrift();

    computeSocialEmotionalCoupling();

    computeImmersiveEmotionalBinding();

    stabilizeEmotionalField();
}

const AffectiveFieldState&
AffectiveResonanceMatrix::
getState() const {

    return state_;
}

}