#include "ImmersiveMemoryPersistenceEngine.h"

#include <algorithm>
#include <cmath>

namespace neuroxr {

ImmersiveMemoryPersistenceEngine::
ImmersiveMemoryPersistenceEngine() {

    initialize();
}

void ImmersiveMemoryPersistenceEngine::
initialize() {

    nextMemoryId_ = 1;

    state_.memoryIntegrity = 1.0f;

    state_.persistenceStability = 1.0f;

    state_.recallAccuracy = 1.0f;

    state_.subconsciousActivity = 0.0f;

    state_.contextualContinuity = 1.0f;

    state_.emotionalRetention = 1.0f;
}

uint64_t
ImmersiveMemoryPersistenceEngine::
storeMemory(
    MemoryType type,
    const std::string& context,
    const std::vector<float>& neuralPattern,
    const std::vector<float>& emotionalWeights,
    float persistenceStrength) {

    MemoryFragment memory;

    memory.id = nextMemoryId_++;

    memory.type = type;

    memory.context = context;

    memory.neuralPattern = neuralPattern;

    memory.emotionalWeights = emotionalWeights;

    memory.persistenceStrength =
        persistenceStrength;

    memory.recallProbability = 1.0f;

    memory.subconsciousInfluence =
        persistenceStrength * 0.5f;

    memory.timestamp =
        static_cast<uint64_t>(
            memory.id * 1000);

    memories_[memory.id] = memory;

    memoryTimeline_.push_back(memory.id);

    return memory.id;
}

bool
ImmersiveMemoryPersistenceEngine::
retrieveMemory(
    uint64_t memoryId,
    MemoryFragment& outMemory) const {

    auto it = memories_.find(memoryId);

    if(it == memories_.end())
        return false;

    outMemory = it->second;

    return true;
}

void
ImmersiveMemoryPersistenceEngine::
reinforceMemory(
    uint64_t memoryId,
    float reinforcement) {

    auto it = memories_.find(memoryId);

    if(it == memories_.end())
        return;

    it->second.persistenceStrength +=
        reinforcement;

    it->second.recallProbability +=
        reinforcement * 0.1f;

    if(it->second.recallProbability > 1.0f) {
        it->second.recallProbability = 1.0f;
    }
}

void
ImmersiveMemoryPersistenceEngine::
decayMemory(
    float deltaTime) {

    for(auto& pair : memories_) {

        auto& memory = pair.second;

        memory.persistenceStrength -=
            deltaTime * 0.001f;

        if(memory.persistenceStrength < 0.0f) {
            memory.persistenceStrength = 0.0f;
        }

        memory.recallProbability =
            memory.persistenceStrength;
    }
}

float
ImmersiveMemoryPersistenceEngine::
computePatternSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {

    size_t count =
        std::min(a.size(), b.size());

    if(count == 0)
        return 0.0f;

    float similarity = 0.0f;

    for(size_t i = 0; i < count; ++i) {

        similarity +=
            1.0f -
            std::abs(a[i] - b[i]);
    }

    return similarity /
        static_cast<float>(count);
}

void
ImmersiveMemoryPersistenceEngine::
consolidateMemories() {

    for(auto& pairA : memories_) {

        for(auto& pairB : memories_) {

            if(pairA.first == pairB.first)
                continue;

            float similarity =
                computePatternSimilarity(
                    pairA.second.neuralPattern,
                    pairB.second.neuralPattern);

            if(similarity > 0.95f) {

                pairA.second.persistenceStrength +=
                    0.0001f;
            }
        }
    }
}

void
ImmersiveMemoryPersistenceEngine::
computeMemoryIntegrity() {

    if(memories_.empty()) {

        state_.memoryIntegrity = 0.0f;
        return;
    }

    float integrity = 0.0f;

    for(const auto& pair : memories_) {

        integrity +=
            pair.second.persistenceStrength;
    }

    integrity /=
        static_cast<float>(
            memories_.size());

    state_.memoryIntegrity = integrity;
}

void
ImmersiveMemoryPersistenceEngine::
computeRecallAccuracy() {

    if(memories_.empty()) {

        state_.recallAccuracy = 0.0f;
        return;
    }

    float accuracy = 0.0f;

    for(const auto& pair : memories_) {

        accuracy +=
            pair.second.recallProbability;
    }

    accuracy /=
        static_cast<float>(
            memories_.size());

    state_.recallAccuracy = accuracy;
}

void
ImmersiveMemoryPersistenceEngine::
computeSubconsciousActivity() {

    float activity = 0.0f;

    for(const auto& pair : memories_) {

        activity +=
            pair.second.subconsciousInfluence;
    }

    if(!memories_.empty()) {

        activity /=
            static_cast<float>(
                memories_.size());
    }

    state_.subconsciousActivity = activity;
}

void
ImmersiveMemoryPersistenceEngine::
computeContextualContinuity() {

    if(memoryTimeline_.size() < 2) {

        state_.contextualContinuity = 1.0f;
        return;
    }

    float continuity = 0.0f;

    for(size_t i = 1;
        i < memoryTimeline_.size();
        ++i) {

        const auto& current =
            memories_.at(
                memoryTimeline_[i]);

        const auto& previous =
            memories_.at(
                memoryTimeline_[i - 1]);

        continuity +=
            computePatternSimilarity(
                current.neuralPattern,
                previous.neuralPattern);
    }

    continuity /=
        static_cast<float>(
            memoryTimeline_.size() - 1);

    state_.contextualContinuity =
        continuity;
}

void
ImmersiveMemoryPersistenceEngine::
stabilizePersistenceNetwork() {

    state_.persistenceStability =
        (state_.memoryIntegrity * 0.4f) +
        (state_.recallAccuracy * 0.3f) +
        (state_.contextualContinuity * 0.3f);

    state_.emotionalRetention =
        state_.memoryIntegrity *
        0.85f;
}

void
ImmersiveMemoryPersistenceEngine::
update(
    float deltaTime) {

    decayMemory(deltaTime);

    consolidateMemories();

    computeMemoryIntegrity();

    computeRecallAccuracy();

    computeSubconsciousActivity();

    computeContextualContinuity();

    stabilizePersistenceNetwork();
}

const MemoryState&
ImmersiveMemoryPersistenceEngine::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    MemoryFragment>&
ImmersiveMemoryPersistenceEngine::
getMemories() const {

    return memories_;
}

}