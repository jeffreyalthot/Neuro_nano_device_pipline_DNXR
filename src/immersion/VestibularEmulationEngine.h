#pragma once

#include <array>
#include <vector>

namespace dnxr {

struct VestibularState {

    std::array<float,3> angularVelocity;

    std::array<float,3> linearAcceleration;

    std::array<float,3> gravityVector;

    std::array<float,3> headOrientation;

    float equilibrium;

    float motionSicknessRisk;

    float inertialStress;
};

class VestibularEmulationEngine {
public:

    VestibularEmulationEngine();

    void initialize();

    void update(
        const std::array<float,3>& velocity,
        const std::array<float,3>& acceleration,
        const std::array<float,3>& orientation,
        float deltaTime);

    void applyArtificialGravity(
        const std::array<float,3>& gravity);

    void stabilizeVestibularSignal();

    void computeMotionSicknessRisk();

    const VestibularState& getState() const;

private:

    void updateAngularVelocity(
        const std::array<float,3>& orientation,
        float deltaTime);

    void updateLinearAcceleration(
        const std::array<float,3>& acceleration);

private:

    VestibularState state_;

    std::array<float,3> previousOrientation_;
};

}

namespace neuroxr {
using VestibularState = ::dnxr::VestibularState;
using VestibularEmulationEngine = ::dnxr::VestibularEmulationEngine;
}
