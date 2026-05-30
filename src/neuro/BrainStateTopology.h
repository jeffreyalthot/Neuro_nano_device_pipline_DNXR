#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace neuroxr {

enum class CorticalRegionType {
    VISUAL,
    AUDITORY,
    SOMATOSENSORY,
    MOTOR,
    PREFRONTAL,
    LIMBIC,
    HIPPOCAMPAL,
    VESTIBULAR
};

struct SynapticConnection {
    uint64_t targetNeuronId;
    float weight;
    float plasticity;
    float delay;
};

struct NeuralNode {
    uint64_t id;

    float membranePotential;
    float restingPotential;
    float thresholdPotential;

    float firingRate;
    float refractoryTime;

    float oscillationPhase;
    float oscillationFrequency;

    std::array<float,3> position;

    std::vector<SynapticConnection> outgoingConnections;
};

struct CorticalRegion {
    CorticalRegionType type;

    std::string name;

    std::vector<NeuralNode> neurons;

    float coherence;
    float energyConsumption;
};

class BrainStateTopology {
public:

    BrainStateTopology();

    void initialize();

    void simulate(float deltaTime);

    void synchronizeGlobalOscillation();

    void applyHebbianPlasticity();

    void injectExternalStimulus(
        CorticalRegionType region,
        const std::vector<float>& stimulus);

    float computeGlobalCoherence() const;

    const std::vector<CorticalRegion>& getRegions() const;

private:

    void buildRegion(
        CorticalRegionType type,
        const std::string& name,
        uint64_t neuronCount);

    void updateNeuron(
        NeuralNode& neuron,
        float deltaTime);

private:

    std::vector<CorticalRegion> regions_;
};

}