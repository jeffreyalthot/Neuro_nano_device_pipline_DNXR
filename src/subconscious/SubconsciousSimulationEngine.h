#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace dnxr {

enum class SubconsciousPatternType {

    FEAR_PATTERN,
    DESIRE_PATTERN,
    MEMORY_ECHO,
    PREDICTIVE_PATTERN,
    EMOTIONAL_REFLEX,
    SOCIAL_IMPRINT,
    IDENTITY_FRAGMENT,
    DREAM_CONSTRUCT
};

struct SubconsciousPattern {

    uint64_t patternId;

    SubconsciousPatternType type;

    std::string label;

    float intensity;

    float persistence;

    float emotionalWeight;

    float cognitiveInfluence;

    float realityInfluence;

    std::vector<float>
    neuralWeights;
};

struct SubconsciousState {

    float subconsciousActivity;

    float emotionalPressure;

    float predictivePressure;

    float identityFragmentation;

    float memoryEchoIntensity;

    float dreamConstructIntensity;

    float subconsciousCoherence;

    float subconsciousDrift;
};

class SubconsciousSimulationEngine {
public:

    SubconsciousSimulationEngine();

    void initialize();

    uint64_t createPattern(
        SubconsciousPatternType type,
        const std::string& label);

    void injectPatternInfluence(
        uint64_t patternId,
        float influence);

    void computeSubconsciousActivity();

    void computeEmotionalPressure();

    void computePredictivePressure();

    void computeIdentityFragmentation();

    void computeMemoryEchoIntensity();

    void computeDreamConstructIntensity();

    void computeSubconsciousCoherence();

    void computeSubconsciousDrift();

    void stabilizeSubconsciousField();

    void decayPatterns(
        float deltaTime);

    void update(
        float deltaTime);

    const SubconsciousState&
    getState() const;

private:

    uint64_t nextPatternId_;

    std::unordered_map<
        uint64_t,
        SubconsciousPattern>
    patterns_;

    SubconsciousState
    state_;
};

}

namespace neuroxr {
using SubconsciousPatternType = ::dnxr::SubconsciousPatternType;
using SubconsciousPattern = ::dnxr::SubconsciousPattern;
using SubconsciousState = ::dnxr::SubconsciousState;
using SubconsciousSimulationEngine = ::dnxr::SubconsciousSimulationEngine;
}
