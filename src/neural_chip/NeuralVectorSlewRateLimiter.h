#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct NeuralVectorSlewPolicy {
    float maxVisualDelta01 = 0.04f;
    float maxAuditoryDelta01 = 0.06f;
    float maxTouchDelta01 = 0.08f;
    float maxVestibularDelta01 = 0.03f;
    float maxMotorDelta01 = 0.10f;
    float maxChemicalDelta01 = 0.01f;
};

class NeuralVectorSlewRateLimiter {
public:
    explicit NeuralVectorSlewRateLimiter(NeuralVectorSlewPolicy policy = {});
    FullDiveChipFrame apply(const FullDiveChipFrame& frame);
    void reset();

private:
    static float clamp01(float value);
    static float limit(float previous, float next, float maxDelta);
    NeuralVectorSlewPolicy policy_{};
    FullDiveChipFrame previous_{};
    bool hasPrevious_ = false;
};

} // namespace dnxr
