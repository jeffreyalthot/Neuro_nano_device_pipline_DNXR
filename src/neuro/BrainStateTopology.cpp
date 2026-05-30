#include "BrainStateTopology.h"

#include <cmath>
#include <random>

namespace neuroxr {

BrainStateTopology::BrainStateTopology() {
    initialize();
}

void BrainStateTopology::initialize() {

    regions_.clear();

    buildRegion(CorticalRegionType::VISUAL, "Visual Cortex", 4096);
    buildRegion(CorticalRegionType::AUDITORY, "Auditory Cortex", 2048);
    buildRegion(CorticalRegionType::SOMATOSENSORY, "Somatosensory Cortex", 4096);
    buildRegion(CorticalRegionType::MOTOR, "Motor Cortex", 4096);
    buildRegion(CorticalRegionType::PREFRONTAL, "Prefrontal Cortex", 8192);
    buildRegion(CorticalRegionType::LIMBIC, "Limbic System", 2048);
    buildRegion(CorticalRegionType::HIPPOCAMPAL, "Hippocampal Network", 2048);
    buildRegion(CorticalRegionType::VESTIBULAR, "Vestibular Cortex", 1024);
}

void BrainStateTopology::buildRegion(
    CorticalRegionType type,
    const std::string& name,
    uint64_t neuronCount) {

    CorticalRegion region;

    region.type = type;
    region.name = name;
    region.coherence = 0.0f;
    region.energyConsumption = 0.0f;

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for(uint64_t i = 0; i < neuronCount; ++i) {

        NeuralNode neuron;

        neuron.id = i;

        neuron.membranePotential = -70.0f;
        neuron.restingPotential = -70.0f;
        neuron.thresholdPotential = -55.0f;

        neuron.firingRate = 0.0f;
        neuron.refractoryTime = 0.0f;

        neuron.oscillationPhase = 0.0f;
        neuron.oscillationFrequency = 30.0f;

        neuron.position = {
            dist(rng),
            dist(rng),
            dist(rng)
        };

        for(uint64_t c = 0; c < 32; ++c) {

            SynapticConnection connection;

            connection.targetNeuronId = (i + c + 1) % neuronCount;
            connection.weight = dist(rng);
            connection.plasticity = 0.005f;
            connection.delay = 0.001f * float(c);

            neuron.outgoingConnections.push_back(connection);
        }

        region.neurons.push_back(neuron);
    }

    regions_.push_back(region);
}

void BrainStateTopology::updateNeuron(
    NeuralNode& neuron,
    float deltaTime) {

    neuron.oscillationPhase +=
        neuron.oscillationFrequency * deltaTime;

    neuron.membranePotential +=
        std::sin(neuron.oscillationPhase) * 0.25f;

    if(neuron.membranePotential >= neuron.thresholdPotential) {

        neuron.firingRate = 1.0f;

        neuron.membranePotential =
            neuron.restingPotential;

    } else {

        neuron.firingRate *= 0.99f;
    }
}

void BrainStateTopology::simulate(float deltaTime) {

    for(auto& region : regions_) {

        float coherenceAccumulator = 0.0f;

        for(auto& neuron : region.neurons) {

            updateNeuron(neuron, deltaTime);

            coherenceAccumulator +=
                neuron.firingRate;
        }

        region.coherence =
            coherenceAccumulator /
            static_cast<float>(region.neurons.size());
    }
}

void BrainStateTopology::synchronizeGlobalOscillation() {

    float globalCoherence =
        computeGlobalCoherence();

    for(auto& region : regions_) {

        for(auto& neuron : region.neurons) {

            neuron.oscillationFrequency +=
                globalCoherence * 0.01f;
        }
    }
}

void BrainStateTopology::applyHebbianPlasticity() {

    for(auto& region : regions_) {

        for(auto& neuron : region.neurons) {

            for(auto& connection :
                neuron.outgoingConnections) {

                connection.weight +=
                    neuron.firingRate *
                    connection.plasticity;
            }
        }
    }
}

void BrainStateTopology::injectExternalStimulus(
    CorticalRegionType regionType,
    const std::vector<float>& stimulus) {

    for(auto& region : regions_) {

        if(region.type != regionType)
            continue;

        for(size_t i = 0;
            i < region.neurons.size() &&
            i < stimulus.size();
            ++i) {

            region.neurons[i].membranePotential +=
                stimulus[i];
        }
    }
}

float BrainStateTopology::computeGlobalCoherence() const {

    float coherence = 0.0f;

    for(const auto& region : regions_) {
        coherence += region.coherence;
    }

    return coherence /
        static_cast<float>(regions_.size());
}

const std::vector<CorticalRegion>&
BrainStateTopology::getRegions() const {
    return regions_;
}

}