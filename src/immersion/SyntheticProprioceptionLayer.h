#pragma once

#include <array>
#include <vector>
#include <cstdint>

namespace neuroxr {

struct LimbState {

    std::array<float,3> position;

    std::array<float,3> velocity;

    std::array<float,3> acceleration;

    float muscularTension;

    float jointCompression;

    float proprioceptiveConfidence;
};

struct ProprioceptiveFrame {

    std::vector<LimbState> limbs;

    float fullBodyCoherence;

    float balanceStability;

    float movementPrecision;
};

class SyntheticProprioceptionLayer {
public:

    SyntheticProprioceptionLayer();

    void initialize(
        uint32_t limbCount);

    void updateLimbState(
        uint32_t limbIndex,
        const LimbState& state);

    void computeBodyCoherence();

    void computeBalanceStability();

    void computeMovementPrecision();

    void stabilizeFeedbackLoop();

    const ProprioceptiveFrame&
    getFrame() const;

private:

    ProprioceptiveFrame frame_;
};

}