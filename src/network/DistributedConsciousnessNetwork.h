#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnxr {

enum class ConsciousNodeType {

    PRIMARY_USER,
    SECONDARY_USER,
    AI_ENTITY,
    SUBCONSCIOUS_AGENT,
    MEMORY_PROXY,
    DREAM_SYNTHESIZER
};

struct ConsciousnessPacket {

    uint64_t packetId;

    uint64_t sourceNodeId;

    uint64_t destinationNodeId;

    float consciousnessIntensity;

    float emotionalPayload;

    float memoryPayload;

    float synchronizationFactor;

    float subjectiveLatency;

    bool encrypted;
};

struct ConsciousnessNode {

    uint64_t nodeId;

    ConsciousNodeType type;

    std::string identifier;

    float synchronization;

    float subjectivePresence;

    float cognitiveIntegrity;

    float neuralLatency;

    float trustFactor;

    std::vector<float>
    neuralSignature;
};

struct DistributedConsciousnessState {

    float globalSynchronization;

    float collectiveAwareness;

    float distributedPresence;

    float neuralConsensus;

    float subjectiveMeshIntegrity;

    float memoryConsensus;

    float emotionalConsensus;

    float consciousnessPropagation;

    float networkLatency;

    float networkStability;
};

class DistributedConsciousnessNetwork {
public:

    DistributedConsciousnessNetwork();

    void initialize();

    uint64_t registerNode(
        ConsciousNodeType type,
        const std::string& identifier,
        const std::vector<float>& neuralSignature);

    void unregisterNode(
        uint64_t nodeId);

    void transmitPacket(
        const ConsciousnessPacket& packet);

    void processPackets();

    void synchronizeNodes();

    void computeGlobalSynchronization();

    void computeCollectiveAwareness();

    void computeDistributedPresence();

    void computeNeuralConsensus();

    void computeSubjectiveMeshIntegrity();

    void computeMemoryConsensus();

    void computeEmotionalConsensus();

    void computeConsciousnessPropagation();

    void computeNetworkLatency();

    void stabilizeNetwork();

    void update(
        float deltaTime);

    const DistributedConsciousnessState&
    getState() const;

    const std::unordered_map<
        uint64_t,
        ConsciousnessNode>&
    getNodes() const;

private:

    uint64_t nextNodeId_;

    uint64_t nextPacketId_;

    std::unordered_map<
        uint64_t,
        ConsciousnessNode>
    nodes_;

    std::deque<
        ConsciousnessPacket>
    packetQueue_;

    DistributedConsciousnessState
    state_;
};

}

namespace neuroxr {
using namespace ::dnxr;
}
