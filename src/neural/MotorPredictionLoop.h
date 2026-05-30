#pragma once

#include <array>
#include <deque>
#include <vector>
#include <cstdint>

namespace neuroxr {

struct MotorState {

    std::array<float,3> position;

    std::array<float,3> velocity;

    std::array<float,3> acceleration;

    std::array<float,3> predictedPosition;

    std::array<float,3> predictedVelocity;

    float motorConfidence;

    float predictionError;

    float correctionIntensity;
};

struct MotorPredictionFrame {

    std::vector<MotorState> motors;

    float globalPredictionAccuracy;

    float globalCorrectionFactor;

    float motorCoherence;

    float reflexStability;
};

class MotorPredictionLoop {
public:

    MotorPredictionLoop();

    void initialize(
        uint32_t motorCount);

    void updateMotorState(
        uint32_t motorIndex,
        const MotorState& state);

    void predictNextFrame(
        float deltaTime);

    void computePredictionErrors();

    void applyCorrectiveFeedback();

    void stabilizeReflexLoop();

    void computeMotorCoherence();

    void computePredictionAccuracy();

    void update(
        float deltaTime);

    const MotorPredictionFrame&
    getFrame() const;

private:

    MotorPredictionFrame frame_;
};

}