#include "SyntheticProprioceptionLayer.h"

#include <cmath>

namespace dnxr {

SyntheticProprioceptionLayer::
SyntheticProprioceptionLayer() {

    frame_.fullBodyCoherence = 1.0f;

    frame_.balanceStability = 1.0f;

    frame_.movementPrecision = 1.0f;
}

void SyntheticProprioceptionLayer::
initialize(
    uint32_t limbCount) {

    frame_.limbs.clear();

    for(uint32_t i = 0;
        i < limbCount;
        ++i) {

        LimbState state;

        state.position = {0.0f,0.0f,0.0f};

        state.velocity = {0.0f,0.0f,0.0f};

        state.acceleration = {0.0f,0.0f,0.0f};

        state.muscularTension = 0.0f;

        state.jointCompression = 0.0f;

        state.proprioceptiveConfidence = 1.0f;

        frame_.limbs.push_back(state);
    }
}

void SyntheticProprioceptionLayer::
updateLimbState(
    uint32_t limbIndex,
    const LimbState& state) {

    if(limbIndex >= frame_.limbs.size())
        return;

    frame_.limbs[limbIndex] = state;
}

void SyntheticProprioceptionLayer::
computeBodyCoherence() {

    float coherence = 0.0f;

    for(const auto& limb : frame_.limbs) {

        coherence +=
            limb.proprioceptiveConfidence;
    }

    if(!frame_.limbs.empty()) {

        coherence /=
            static_cast<float>(
                frame_.limbs.size());
    }

    frame_.fullBodyCoherence = coherence;
}

void SyntheticProprioceptionLayer::
computeBalanceStability() {

    float balance = 0.0f;

    for(const auto& limb : frame_.limbs) {

        float velocityMagnitude =
            std::sqrt(
                limb.velocity[0] *
                limb.velocity[0] +

                limb.velocity[1] *
                limb.velocity[1] +

                limb.velocity[2] *
                limb.velocity[2]);

        balance +=
            1.0f /
            (1.0f + velocityMagnitude);
    }

    if(!frame_.limbs.empty()) {

        balance /=
            static_cast<float>(
                frame_.limbs.size());
    }

    frame_.balanceStability = balance;
}

void SyntheticProprioceptionLayer::
computeMovementPrecision() {

    float precision = 0.0f;

    for(const auto& limb : frame_.limbs) {

        precision +=
            1.0f -
            limb.jointCompression;
    }

    if(!frame_.limbs.empty()) {

        precision /=
            static_cast<float>(
                frame_.limbs.size());
    }

    frame_.movementPrecision = precision;
}

void SyntheticProprioceptionLayer::
stabilizeFeedbackLoop() {

    for(auto& limb : frame_.limbs) {

        limb.proprioceptiveConfidence *=
            0.999f;
    }
}

const ProprioceptiveFrame&
SyntheticProprioceptionLayer::
getFrame() const {

    return frame_;
}

}