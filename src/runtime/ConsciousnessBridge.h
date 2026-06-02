#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace dnxr {

enum class ConsciousnessLayer {

    SUBCONSCIOUS,
    PRECONSCIOUS,
    CONSCIOUS,
    HYPERCONSCIOUS
};

struct ConsciousnessSignal {

    ConsciousnessLayer layer;

    float intensity;

    float stability;

    float synchronization;

    float perceptualBinding;

    float identityIntegrity;
};

struct ConsciousnessState {

    std::vector<ConsciousnessSignal>
    signals;

    float awarenessLevel;

    float selfPerception;

    float dreamStateIntensity;

    float subjectiveRealityLock;

    float dissociationRisk;
};

class ConsciousnessBridge {
public:

    ConsciousnessBridge();

    void initialize();

    void generateConsciousnessSignal(
        ConsciousnessLayer layer,
        float intensity);

    void synchronizeConsciousness();

    void computeAwarenessLevel();

    void computeSelfPerception();

    void computeRealityLock();

    void computeDissociationRisk();

    void stabilizeConsciousness();

    void update(
        float deltaTime);

    const ConsciousnessState&
    getState() const;

private:

    ConsciousnessState
    state_;
};

}
