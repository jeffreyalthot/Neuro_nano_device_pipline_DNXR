#include "FullDiveOrchestrator.h"

#include "FullDiveSynchronizationEngine.h"
#include "../network/DistributedConsciousnessNetwork.h"
#include "../memory/RecursiveMemoryReconstructionEngine.h"
#include "../reality/SubjectiveTimeDilationEngine.h"
#include "../physics/NeuralQuantumEntanglementEngine.h"
#include "../emotion/AffectiveResonanceMatrix.h"
#include "../subconscious/SubconsciousSimulationEngine.h"

#include <algorithm>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

float inverseRisk(float value) {
    return clamp01(1.0f - value);
}

} // namespace

ResearchFullDiveOrchestrator::
ResearchFullDiveOrchestrator() {

    initialize();
}

void ResearchFullDiveOrchestrator::
initialize() {

    synchronizationEngine_ = nullptr;
    consciousnessNetwork_ = nullptr;
    memoryEngine_ = nullptr;
    timeEngine_ = nullptr;
    quantumEngine_ = nullptr;
    emotionEngine_ = nullptr;
    subconsciousEngine_ = nullptr;

    state_.immersionLevel = 0.0f;
    state_.consciousnessIntegrity = 1.0f;
    state_.realityConsistency = 1.0f;
    state_.memoryIntegrity = 1.0f;
    state_.emotionalSynchronization = 1.0f;
    state_.subconsciousSynchronization = 1.0f;
    state_.temporalSynchronization = 1.0f;
    state_.networkSynchronization = 1.0f;
    state_.quantumSynchronization = 1.0f;
    state_.fullDiveCoherence = 1.0f;
}

void ResearchFullDiveOrchestrator::
attachSynchronizationEngine(
    FullDiveSynchronizationEngine* engine) {

    synchronizationEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
attachConsciousnessNetwork(
    DistributedConsciousnessNetwork* network) {

    consciousnessNetwork_ = network;
}

void ResearchFullDiveOrchestrator::
attachMemoryEngine(
    RecursiveMemoryReconstructionEngine* engine) {

    memoryEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
attachTimeEngine(
    SubjectiveTimeDilationEngine* engine) {

    timeEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
attachQuantumEngine(
    NeuralQuantumEntanglementEngine* engine) {

    quantumEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
attachEmotionEngine(
    AffectiveResonanceMatrix* engine) {

    emotionEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
attachSubconsciousEngine(
    SubconsciousSimulationEngine* engine) {

    subconsciousEngine_ = engine;
}

void ResearchFullDiveOrchestrator::
computeImmersionLevel() {

    float baseImmersion =
        (
            state_.memoryIntegrity +
            state_.temporalSynchronization +
            state_.emotionalSynchronization +
            state_.subconsciousSynchronization +
            state_.networkSynchronization +
            state_.quantumSynchronization
        ) / 6.0f;

    if(synchronizationEngine_ != nullptr) {

        const auto& syncState =
            synchronizationEngine_->getState();

        baseImmersion =
            (baseImmersion * 0.60f) +
            (clamp01(syncState.immersionDepth) * 0.20f) +
            (clamp01(syncState.fullDiveStability) * 0.20f);
    }

    state_.immersionLevel =
        clamp01(baseImmersion);
}

void ResearchFullDiveOrchestrator::
computeConsciousnessIntegrity() {

    float integrity =
        state_.memoryIntegrity *
        state_.realityConsistency *
        state_.networkSynchronization;

    if(synchronizationEngine_ != nullptr) {

        const auto& syncState =
            synchronizationEngine_->getState();

        const float safetyGate =
            syncState.emergencyExitTriggered
            ? 0.0f
            : ((syncState.consciousnessDesynchronization ||
                syncState.neuralOverloadDetected)
               ? 0.25f
               : 1.0f);

        const float corticalContribution =
            0.50f + (clamp01(syncState.corticalStability) * 0.50f);

        integrity *=
            corticalContribution *
            clamp01(syncState.neuralSynchronization) *
            safetyGate;
    }

    state_.consciousnessIntegrity =
        clamp01(integrity);
}

void ResearchFullDiveOrchestrator::
computeRealityConsistency() {

    float consistency =
        state_.temporalSynchronization *
        state_.subconsciousSynchronization *
        state_.quantumSynchronization;

    if(synchronizationEngine_ != nullptr) {

        const auto& syncState =
            synchronizationEngine_->getState();

        consistency *=
            clamp01(syncState.vestibularStability) *
            clamp01(syncState.motorPredictionAccuracy);
    }

    state_.realityConsistency =
        clamp01(consistency);
}

void ResearchFullDiveOrchestrator::
computeMemoryIntegrity() {

    if(memoryEngine_ == nullptr)
        return;

    const auto& memoryState =
        memoryEngine_->getState();

    state_.memoryIntegrity =
        clamp01(
            memoryState.memoryIntegrity *
            memoryState.reconstructionAccuracy *
            memoryState.temporalStability *
            memoryState.neuralBindingStrength *
            memoryState.realityAnchoring *
            inverseRisk(memoryState.syntheticMemoryRisk) *
            inverseRisk(memoryState.recursiveDrift));
}

void ResearchFullDiveOrchestrator::
computeEmotionalSynchronization() {

    if(emotionEngine_ == nullptr)
        return;

    const auto& affectiveState =
        emotionEngine_->getState();

    state_.emotionalSynchronization =
        clamp01(
            (affectiveState.emotionalCoherence * 0.35f) +
            (affectiveState.emotionalSynchronization * 0.35f) +
            (affectiveState.empathicResonance * 0.10f) +
            (affectiveState.socialEmotionalCoupling * 0.10f) +
            (affectiveState.immersiveEmotionalBinding * 0.10f) -
            (affectiveState.emotionalDrift * 0.25f));
}

void ResearchFullDiveOrchestrator::
computeSubconsciousSynchronization() {

    if(subconsciousEngine_ == nullptr)
        return;

    const auto& subconsciousState =
        subconsciousEngine_->getState();

    state_.subconsciousSynchronization =
        clamp01(
            subconsciousState.subconsciousCoherence *
            inverseRisk(subconsciousState.subconsciousDrift) *
            inverseRisk(subconsciousState.identityFragmentation));
}

void ResearchFullDiveOrchestrator::
computeTemporalSynchronization() {

    if(timeEngine_ == nullptr)
        return;

    const auto& temporalState =
        timeEngine_->getState();

    state_.temporalSynchronization =
        clamp01(
            temporalState.temporalSynchronization *
            temporalState.temporalStability *
            temporalState.realityTimeAlignment *
            temporalState.consciousnessTimeCoupling *
            temporalState.memoryTemporalPersistence *
            inverseRisk(temporalState.temporalFragmentation) *
            inverseRisk(temporalState.temporalLoopRisk));
}

void ResearchFullDiveOrchestrator::
computeNetworkSynchronization() {

    if(consciousnessNetwork_ == nullptr)
        return;

    const auto& networkState =
        consciousnessNetwork_->getState();

    state_.networkSynchronization =
        clamp01(
            networkState.globalSynchronization *
            networkState.neuralConsensus *
            networkState.subjectiveMeshIntegrity *
            networkState.memoryConsensus *
            networkState.emotionalConsensus *
            networkState.networkStability *
            inverseRisk(networkState.networkLatency));
}

void ResearchFullDiveOrchestrator::
computeQuantumSynchronization() {

    if(quantumEngine_ == nullptr)
        return;

    const auto& quantumFrame =
        quantumEngine_->getFrame();

    state_.quantumSynchronization =
        clamp01(
            quantumFrame.globalCoherence *
            quantumFrame.quantumStability *
            quantumFrame.temporalIntegrity *
            (0.75f + quantumFrame.consciousnessCoupling * 0.25f) *
            inverseRisk(quantumFrame.uncertaintyFactor) *
            inverseRisk(quantumFrame.decoherenceRisk));
}

void ResearchFullDiveOrchestrator::
computeFullDiveCoherence() {

    state_.fullDiveCoherence =
        clamp01(
            state_.immersionLevel *
            state_.consciousnessIntegrity *
            state_.realityConsistency);
}

void ResearchFullDiveOrchestrator::
update(
    float deltaTime) {

    if(synchronizationEngine_ != nullptr)
        synchronizationEngine_->update(deltaTime);

    if(consciousnessNetwork_ != nullptr)
        consciousnessNetwork_->update(deltaTime);

    if(memoryEngine_ != nullptr)
        memoryEngine_->update(deltaTime);

    if(timeEngine_ != nullptr)
        timeEngine_->update(deltaTime);

    if(quantumEngine_ != nullptr)
        quantumEngine_->update(deltaTime);

    if(emotionEngine_ != nullptr)
        emotionEngine_->update(deltaTime);

    if(subconsciousEngine_ != nullptr)
        subconsciousEngine_->update(deltaTime);

    computeMemoryIntegrity();

    computeEmotionalSynchronization();

    computeSubconsciousSynchronization();

    computeTemporalSynchronization();

    computeNetworkSynchronization();

    computeQuantumSynchronization();

    computeRealityConsistency();

    computeConsciousnessIntegrity();

    computeImmersionLevel();

    computeFullDiveCoherence();
}

const FullDiveState&
ResearchFullDiveOrchestrator::
getState() const {

    return state_;
}

} // namespace dnxr
