#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnxr {

enum class MemoryType {

    SHORT_TERM,
    LONG_TERM,
    EMOTIONAL,
    SPATIAL,
    SENSORIAL,
    PROCEDURAL,
    SUBCONSCIOUS
};

struct MemoryFragment {

    uint64_t id;

    MemoryType type;

    std::string context;

    std::vector<float> neuralPattern;

    std::vector<float> emotionalWeights;

    float persistenceStrength;

    float recallProbability;

    float subconsciousInfluence;

    uint64_t timestamp;
};

struct MemoryState {

    float memoryIntegrity;

    float persistenceStability;

    float recallAccuracy;

    float subconsciousActivity;

    float contextualContinuity;

    float emotionalRetention;
};

class ImmersiveMemoryPersistenceEngine {
public:

    ImmersiveMemoryPersistenceEngine();

    void initialize();

    uint64_t storeMemory(
        MemoryType type,
        const std::string& context,
        const std::vector<float>& neuralPattern,
        const std::vector<float>& emotionalWeights,
        float persistenceStrength);

    bool retrieveMemory(
        uint64_t memoryId,
        MemoryFragment& outMemory) const;

    void reinforceMemory(
        uint64_t memoryId,
        float reinforcement);

    void decayMemory(
        float deltaTime);

    void consolidateMemories();

    void computeMemoryIntegrity();

    void computeRecallAccuracy();

    void computeSubconsciousActivity();

    void computeContextualContinuity();

    void stabilizePersistenceNetwork();

    void update(
        float deltaTime);

    const MemoryState&
    getState() const;

    const std::unordered_map<
        uint64_t,
        MemoryFragment>&
    getMemories() const;

private:

    float computePatternSimilarity(
        const std::vector<float>& a,
        const std::vector<float>& b) const;

private:

    uint64_t nextMemoryId_;

    std::unordered_map<
        uint64_t,
        MemoryFragment> memories_;

    std::deque<uint64_t>
    memoryTimeline_;

    MemoryState state_;
};

}
