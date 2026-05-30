#include "../src/runtime/FullDiveOrchestrator.h"
#include "../src/runtime/FullDiveSynchronizationEngine.h"
#include "../src/network/DistributedConsciousnessNetwork.h"
#include "../src/memory/RecursiveMemoryReconstructionEngine.h"
#include "../src/reality/SubjectiveTimeDilationEngine.h"
#include "../src/physics/NeuralQuantumEntanglementEngine.h"
#include "../src/emotion/AffectiveResonanceMatrix.h"
#include "../src/subconscious/SubconsciousSimulationEngine.h"

#include <cassert>
#include <cmath>
#include <vector>

namespace {

bool withinUnitInterval(float value) {
    return value >= 0.0F && value <= 1.0F && std::isfinite(value);
}

} // namespace

int main() {
    neuroxr::FullDiveSynchronizationEngine synchronization;
    neuroxr::DistributedConsciousnessNetwork network;
    neuroxr::RecursiveMemoryReconstructionEngine memory;
    neuroxr::SubjectiveTimeDilationEngine time;
    neuroxr::NeuralQuantumEntanglementEngine quantum;
    neuroxr::AffectiveResonanceMatrix emotion;
    neuroxr::SubconsciousSimulationEngine subconscious;

    const std::vector<float> signature = {0.2F, 0.4F, 0.6F, 0.8F};
    network.registerNode(
        neuroxr::ConsciousNodeType::PRIMARY_USER,
        "primary_user",
        signature);
    network.registerNode(
        neuroxr::ConsciousNodeType::AI_ENTITY,
        "immersion_supervisor",
        signature);

    const auto memoryFragment = memory.createMemoryFragment(
        neuroxr::MemoryLayerType::LONG_TERM,
        "session_anchor");
    memory.reinforceFragment(memoryFragment, 0.1F);

    time.createTemporalFrame(neuroxr::TemporalLayerType::REALTIME, 1.0F);
    time.createTemporalFrame(neuroxr::TemporalLayerType::PREDICTIVE, 1.05F);

    const auto quantumA = quantum.createQuantumState(
        neuroxr::QuantumEntanglementType::CONSCIOUSNESS_ENTANGLEMENT);
    const auto quantumB = quantum.createQuantumState(
        neuroxr::QuantumEntanglementType::MEMORY_ENTANGLEMENT);
    quantum.entangleStates(quantumA, quantumB, 0.35F);

    const auto joy = emotion.createAffectiveSignal(
        neuroxr::AffectiveSpectrum::JOY);
    emotion.amplifySignal(joy, 0.2F);

    const auto pattern = subconscious.createPattern(
        neuroxr::SubconsciousPatternType::PREDICTIVE_PATTERN,
        "safe_predictive_anchor");
    subconscious.injectPatternInfluence(pattern, 0.05F);

    neuroxr::FullDiveOrchestrator orchestrator;
    orchestrator.attachSynchronizationEngine(&synchronization);
    orchestrator.attachConsciousnessNetwork(&network);
    orchestrator.attachMemoryEngine(&memory);
    orchestrator.attachTimeEngine(&time);
    orchestrator.attachQuantumEngine(&quantum);
    orchestrator.attachEmotionEngine(&emotion);
    orchestrator.attachSubconsciousEngine(&subconscious);

    orchestrator.update(0.016F);

    const auto& state = orchestrator.getState();
    assert(withinUnitInterval(state.immersionLevel));
    assert(withinUnitInterval(state.consciousnessIntegrity));
    assert(withinUnitInterval(state.realityConsistency));
    assert(withinUnitInterval(state.memoryIntegrity));
    assert(withinUnitInterval(state.emotionalSynchronization));
    assert(withinUnitInterval(state.subconsciousSynchronization));
    assert(withinUnitInterval(state.temporalSynchronization));
    assert(withinUnitInterval(state.networkSynchronization));
    assert(withinUnitInterval(state.quantumSynchronization));
    assert(withinUnitInterval(state.fullDiveCoherence));

    assert(state.memoryIntegrity > 0.0F);
    assert(state.temporalSynchronization > 0.0F);
    assert(state.networkSynchronization > 0.0F);
    assert(state.fullDiveCoherence > 0.0F);

    return 0;
}
