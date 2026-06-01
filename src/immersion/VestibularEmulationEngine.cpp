#include "VestibularEmulationEngine.h"

#include <cmath>

namespace dnxr {

VestibularEmulationEngine::
VestibularEmulationEngine() {

    initialize();
}

void VestibularEmulationEngine::
initialize() {

    state_.angularVelocity = {0.0f,0.0f,0.0f};

    state_.linearAcceleration = {0.0f,0.0f,0.0f};

    state_.gravityVector = {0.0f,-9.81f,0.0f};

    state_.headOrientation = {0.0f,0.0f,0.0f};

    state_.equilibrium = 1.0f;

    state_.motionSicknessRisk = 0.0f;

    state_.inertialStress = 0.0f;

    previousOrientation_ = {0.0f,0.0f,0.0f};
}

void VestibularEmulationEngine::
update(
    const std::array<float,3>& velocity,
    const std::array<float,3>& acceleration,
    const std::array<float,3>& orientation,
    float deltaTime) {

    updateAngularVelocity(
        orientation,
        deltaTime);

    updateLinearAcceleration(
        acceleration);

    state_.headOrientation = orientation;

    state_.inertialStress =
        std::sqrt(
            acceleration[0] * acceleration[0] +
            acceleration[1] * acceleration[1] +
            acceleration[2] * acceleration[2]);

    computeMotionSicknessRisk();

    stabilizeVestibularSignal();

    previousOrientation_ = orientation;
}

void VestibularEmulationEngine::
updateAngularVelocity(
    const std::array<float,3>& orientation,
    float deltaTime) {

    if(deltaTime <= 0.0f)
        return;

    state_.angularVelocity[0] =
        (orientation[0] -
        previousOrientation_[0]) / deltaTime;

    state_.angularVelocity[1] =
        (orientation[1] -
        previousOrientation_[1]) / deltaTime;

    state_.angularVelocity[2] =
        (orientation[2] -
        previousOrientation_[2]) / deltaTime;
}

void VestibularEmulationEngine::
updateLinearAcceleration(
    const std::array<float,3>& acceleration) {

    state_.linearAcceleration = acceleration;
}

void VestibularEmulationEngine::
applyArtificialGravity(
    const std::array<float,3>& gravity) {

    state_.gravityVector = gravity;
}

void VestibularEmulationEngine::
stabilizeVestibularSignal() {

    float stability =
        1.0f -
        (state_.motionSicknessRisk * 0.5f);

    if(stability < 0.0f)
        stability = 0.0f;

    state_.equilibrium = stability;
}

void VestibularEmulationEngine::
computeMotionSicknessRisk() {

    float angularMagnitude =
        std::sqrt(
            state_.angularVelocity[0] *
            state_.angularVelocity[0] +

            state_.angularVelocity[1] *
            state_.angularVelocity[1] +

            state_.angularVelocity[2] *
            state_.angularVelocity[2]);

    float accelerationMagnitude =
        std::sqrt(
            state_.linearAcceleration[0] *
            state_.linearAcceleration[0] +

            state_.linearAcceleration[1] *
            state_.linearAcceleration[1] +

            state_.linearAcceleration[2] *
            state_.linearAcceleration[2]);

    state_.motionSicknessRisk =
        (angularMagnitude * 0.005f) +
        (accelerationMagnitude * 0.0025f);

    if(state_.motionSicknessRisk > 1.0f)
        state_.motionSicknessRisk = 1.0f;
}

const VestibularState&
VestibularEmulationEngine::getState() const {

    return state_;
}

}