#include "DistributedConsciousnessNetwork.h"

#include <algorithm>
#include <cmath>

namespace neuroxr {

DistributedConsciousnessNetwork::
DistributedConsciousnessNetwork() {

    initialize();
}

void DistributedConsciousnessNetwork::
initialize() {

    nextNodeId_ = 1;

    nextPacketId_ = 1;

    state_.globalSynchronization =
        1.0f;

    state_.collectiveAwareness =
        0.0f;

    state_.distributedPresence =
        0.0f;

    state_.neuralConsensus =
        1.0f;

    state_.subjectiveMeshIntegrity =
        1.0f;

    state_.memoryConsensus =
        1.0f;

    state_.emotionalConsensus =
        1.0f;

    state_.consciousnessPropagation =
        0.0f;

    state_.networkLatency =
        0.0f;

    state_.networkStability =
        1.0f;
}

uint64_t
DistributedConsciousnessNetwork::
registerNode(
    ConsciousNodeType type,
    const std::string& identifier,
    const std::vector<float>& neuralSignature) {

    ConsciousnessNode node;

    node.nodeId =
        nextNodeId_++;

    node.type =
        type;

    node.identifier =
        identifier;

    node.synchronization =
        1.0f;

    node.subjectivePresence =
        1.0f;

    node.cognitiveIntegrity =
        1.0f;

    node.neuralLatency =
        0.0f;

    node.trustFactor =
        1.0f;

    node.neuralSignature =
        neuralSignature;

    nodes_[node.nodeId] =
        node;

    return node.nodeId;
}

void
DistributedConsciousnessNetwork::
unregisterNode(
    uint64_t nodeId) {

    nodes_.erase(nodeId);
}

void
DistributedConsciousnessNetwork::
transmitPacket(
    const ConsciousnessPacket& packet) {

    packetQueue_.push_back(packet);

    if(packetQueue_.size() > 100000) {

        packetQueue_.pop_front();
    }
}

void
DistributedConsciousnessNetwork::
processPackets() {

    for(auto& packet :
        packetQueue_) {

        auto sourceIt =
            nodes_.find(
                packet.sourceNodeId);

        auto destinationIt =
            nodes_.find(
                packet.destinationNodeId);

        if(sourceIt == nodes_.end() ||
           destinationIt == nodes_.end()) {

            continue;
        }

        destinationIt->second
            .synchronization +=

            packet.synchronizationFactor *
            0.0001f;

        destinationIt->second
            .subjectivePresence +=

            packet.consciousnessIntensity *
            0.0001f;

        destinationIt->second
            .trustFactor +=

            packet.emotionalPayload *
            0.00005f;

        if(destinationIt->second
            .trustFactor > 1.0f) {

            destinationIt->second
                .trustFactor = 1.0f;
        }
    }
}

void
DistributedConsciousnessNetwork::
synchronizeNodes() {

    for(auto& pair :
        nodes_) {

        pair.second.synchronization =

            pair.second.synchronization *
            0.999f +

            state_.globalSynchronization *
            0.001f;
    }
}

void
DistributedConsciousnessNetwork::
computeGlobalSynchronization() {

    float synchronization = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        nodes_) {

        synchronization +=
            pair.second.synchronization;

        ++count;
    }

    if(count > 0) {

        synchronization /=
            static_cast<float>(
                count);
    }

    state_.globalSynchronization =
        synchronization;
}

void
DistributedConsciousnessNetwork::
computeCollectiveAwareness() {

    float awareness = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        nodes_) {

        awareness +=
            pair.second.subjectivePresence;

        ++count;
    }

    if(count > 0) {

        awareness /=
            static_cast<float>(
                count);
    }

    state_.collectiveAwareness =
        awareness;
}

void
DistributedConsciousnessNetwork::
computeDistributedPresence() {

    state_.distributedPresence =

        state_.collectiveAwareness *

        state_.globalSynchronization;
}

void
DistributedConsciousnessNetwork::
computeNeuralConsensus() {

    float consensus = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        nodes_) {

        consensus +=
            pair.second.cognitiveIntegrity;

        ++count;
    }

    if(count > 0) {

        consensus /=
            static_cast<float>(
                count);
    }

    state_.neuralConsensus =
        consensus;
}

void
DistributedConsciousnessNetwork::
computeSubjectiveMeshIntegrity() {

    state_.subjectiveMeshIntegrity =

        (state_.globalSynchronization *
        0.5f)

        +

        (state_.neuralConsensus *
        0.5f);
}

void
DistributedConsciousnessNetwork::
computeMemoryConsensus() {

    float consensus = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        nodes_) {

        consensus +=
            pair.second.trustFactor;

        ++count;
    }

    if(count > 0) {

        consensus /=
            static_cast<float>(
                count);
    }

    state_.memoryConsensus =
        consensus;
}

void
DistributedConsciousnessNetwork::
computeEmotionalConsensus() {

    state_.emotionalConsensus =

        state_.memoryConsensus *

        state_.collectiveAwareness;
}

void
DistributedConsciousnessNetwork::
computeConsciousnessPropagation() {

    state_.consciousnessPropagation =

        state_.distributedPresence *

        state_.subjectiveMeshIntegrity;
}

void
DistributedConsciousnessNetwork::
computeNetworkLatency() {

    float latency = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        nodes_) {

        latency +=
            pair.second.neuralLatency;

        ++count;
    }

    if(count > 0) {

        latency /=
            static_cast<float>(
                count);
    }

    state_.networkLatency =
        latency;
}

void
DistributedConsciousnessNetwork::
stabilizeNetwork() {

    state_.networkStability =

        (state_.subjectiveMeshIntegrity *
        0.4f)

        +

        (state_.memoryConsensus *
        0.2f)

        +

        (state_.emotionalConsensus *
        0.2f)

        +

        (1.0f -
        state_.networkLatency) *
        0.2f;
}

void
DistributedConsciousnessNetwork::
update(
    float deltaTime) {

    processPackets();

    synchronizeNodes();

    computeGlobalSynchronization();

    computeCollectiveAwareness();

    computeDistributedPresence();

    computeNeuralConsensus();

    computeSubjectiveMeshIntegrity();

    computeMemoryConsensus();

    computeEmotionalConsensus();

    computeConsciousnessPropagation();

    computeNetworkLatency();

    stabilizeNetwork();
}

const DistributedConsciousnessState&
DistributedConsciousnessNetwork::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    ConsciousnessNode>&
DistributedConsciousnessNetwork::
getNodes() const {

    return nodes_;
}

}