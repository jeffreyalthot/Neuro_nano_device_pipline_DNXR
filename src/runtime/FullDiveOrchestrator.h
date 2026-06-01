#pragma once

#include <memory>

#include "../network/DistributedConsciousnessNetwork.h"
#include "../memory/RecursiveMemoryReconstructionEngine.h"
#include "../reality/SubjectiveTimeDilationEngine.h"
#include "../physics/NeuralQuantumEntanglementEngine.h"
#include "../emotion/AffectiveResonanceMatrix.h"
#include "../subconscious/SubconsciousSimulationEngine.h"

namespace dnxr {

class FullDiveSynchronizationEngine;

struct FullDiveState {

    float immersionLevel;

    float consciousnessIntegrity;

    float realityConsistency;

    float memoryIntegrity;

    float emotionalSynchronization;

    float subconsciousSynchronization;

    float temporalSynchronization;

    float networkSynchronization;

    float quantumSynchronization;

    float fullDiveCoherence;
};

class FullDiveOrchestrator {
public:

    FullDiveOrchestrator();

    void initialize();

    void attachSynchronizationEngine(
        FullDiveSynchronizationEngine* engine);

    void attachConsciousnessNetwork(
        DistributedConsciousnessNetwork* network);

    void attachMemoryEngine(
        RecursiveMemoryReconstructionEngine* engine);

    void attachTimeEngine(
        SubjectiveTimeDilationEngine* engine);

    void attachQuantumEngine(
        NeuralQuantumEntanglementEngine* engine);

    void attachEmotionEngine(
        AffectiveResonanceMatrix* engine);

    void attachSubconsciousEngine(
        SubconsciousSimulationEngine* engine);

    void computeImmersionLevel();

    void computeConsciousnessIntegrity();

    void computeRealityConsistency();

    void computeMemoryIntegrity();

    void computeEmotionalSynchronization();

    void computeSubconsciousSynchronization();

    void computeTemporalSynchronization();

    void computeNetworkSynchronization();

    void computeQuantumSynchronization();

    void computeFullDiveCoherence();

    void update(
        float deltaTime);

    const FullDiveState&
    getState() const;

private:

    FullDiveSynchronizationEngine*
        synchronizationEngine_;

    DistributedConsciousnessNetwork*
        consciousnessNetwork_;

    RecursiveMemoryReconstructionEngine*
        memoryEngine_;

    SubjectiveTimeDilationEngine*
        timeEngine_;

    NeuralQuantumEntanglementEngine*
        quantumEngine_;

    AffectiveResonanceMatrix*
        emotionEngine_;

    SubconsciousSimulationEngine*
        subconsciousEngine_;

    FullDiveState state_;
};

} // namespace dnxr

namespace neuroxr {
using FullDiveState = ::dnxr::FullDiveState;
using FullDiveOrchestrator = ::dnxr::FullDiveOrchestrator;
}
