#include "MotorPredictionLoop.h"

#include <cmath>

namespace dnxr {

MotorPredictionLoop::
MotorPredictionLoop() {

    frame_.globalPredictionAccuracy = 1.0f;

    frame_.globalCorrectionFactor = 0.0f;

    frame_.motorCoherence = 1.0f;

    frame_.reflexStability = 1.0f;
}

void MotorPredictionLoop::
initialize(
    uint32_t motorCount) {

    frame_.motors.clear();

    for(uint32_t i = 0;
        i < motorCount;
        ++i) {

        MotorState state;

        state.position = {0.0f,0.0f,0.0f};

        state.velocity = {0.0f,0.0f,0.0f};

        state.acceleration = {0.0f,0.0f,0.0f};

        state.predictedPosition = {0.0f,0.0f,0.0f};

        state.predictedVelocity = {0.0f,0.0f,0.0f};

        state.motorConfidence = 1.0f;

        state.predictionError = 0.0f;

        state.correctionIntensity = 0.0f;

        frame_.motors.push_back(state);
    }
}

void MotorPredictionLoop::
updateMotorState(
    uint32_t motorIndex,
    const MotorState& state) {

    if(motorIndex >= frame_.motors.size())
        return;

    frame_.motors[motorIndex] = state;
}

void MotorPredictionLoop::
predictNextFrame(
    float deltaTime) {

    for(auto& motor :
        frame_.motors) {

        motor.predictedVelocity[0] =
            motor.velocity[0] +
            (motor.acceleration[0] *
            deltaTime);

        motor.predictedVelocity[1] =
            motor.velocity[1] +
            (motor.acceleration[1] *
            deltaTime);

        motor.predictedVelocity[2] =
            motor.velocity[2] +
            (motor.acceleration[2] *
            deltaTime);

        motor.predictedPosition[0] =
            motor.position[0] +
            (motor.predictedVelocity[0] *
            deltaTime);

        motor.predictedPosition[1] =
            motor.position[1] +
            (motor.predictedVelocity[1] *
            deltaTime);

        motor.predictedPosition[2] =
            motor.position[2] +
            (motor.predictedVelocity[2] *
            deltaTime);
    }
}

void MotorPredictionLoop::
computePredictionErrors() {

    float totalError = 0.0f;

    for(auto& motor :
        frame_.motors) {

        float error =
            std::abs(
                motor.predictedPosition[0] -
                motor.position[0])

            +

            std::abs(
                motor.predictedPosition[1] -
                motor.position[1])

            +

            std::abs(
                motor.predictedPosition[2] -
                motor.position[2]);

        motor.predictionError = error;

        totalError += error;
    }

    if(!frame_.motors.empty()) {

        frame_.globalCorrectionFactor =
            totalError /
            static_cast<float>(
                frame_.motors.size());
    }
}

void MotorPredictionLoop::
applyCorrectiveFeedback() {

    for(auto& motor :
        frame_.motors) {

        motor.correctionIntensity =
            motor.predictionError *
            0.5f;

        motor.motorConfidence =
            1.0f -
            motor.correctionIntensity;

        if(motor.motorConfidence < 0.0f) {

            motor.motorConfidence = 0.0f;
        }
    }
}

void MotorPredictionLoop::
stabilizeReflexLoop() {

    float stability = 0.0f;

    for(const auto& motor :
        frame_.motors) {

        stability +=
            motor.motorConfidence;
    }

    if(!frame_.motors.empty()) {

        stability /=
            static_cast<float>(
                frame_.motors.size());
    }

    frame_.reflexStability = stability;
}

void MotorPredictionLoop::
computeMotorCoherence() {

    float coherence = 0.0f;

    for(const auto& motor :
        frame_.motors) {

        coherence +=
            1.0f -
            motor.predictionError;
    }

    if(!frame_.motors.empty()) {

        coherence /=
            static_cast<float>(
                frame_.motors.size());
    }

    frame_.motorCoherence = coherence;
}

void MotorPredictionLoop::
computePredictionAccuracy() {

    frame_.globalPredictionAccuracy =
        (frame_.motorCoherence * 0.6f) +
        (frame_.reflexStability * 0.4f);
}

void MotorPredictionLoop::
update(
    float deltaTime) {

    predictNextFrame(
        deltaTime);

    computePredictionErrors();

    applyCorrectiveFeedback();

    stabilizeReflexLoop();

    computeMotorCoherence();

    computePredictionAccuracy();
}

const MotorPredictionFrame&
MotorPredictionLoop::
getFrame() const {

    return frame_;
}

}