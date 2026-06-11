#include "PerceivedBodyPoseEstimator.h"

#include <algorithm>

namespace dnxr {
namespace {
float pick(const std::vector<float>& v, std::size_t i, float fallback) { return i < v.size() ? v[i] : fallback; }
}

BodyPoseState PerceivedBodyPoseEstimator::estimate(const NeuralChipFrame& neuralFrame,
                                                   const std::vector<float>& headsetSignals,
                                                   const std::vector<float>& bodySensors,
                                                   const std::vector<float>& proprioceptiveModel) const {
    BodyPoseState p;
    p.headPose = pick(headsetSignals, 0, pick(neuralFrame.channels, 0, 0.0f));
    p.spinePose = pick(bodySensors, 0, pick(proprioceptiveModel, 0, 0.0f));
    p.armPosition = pick(bodySensors, 1, pick(proprioceptiveModel, 1, 0.0f));
    p.handState = pick(bodySensors, 2, pick(proprioceptiveModel, 2, 0.0f));
    p.legState = pick(bodySensors, 3, pick(proprioceptiveModel, 3, 0.0f));
    p.balance = std::clamp(pick(bodySensors, 4, pick(proprioceptiveModel, 4, 1.0f)), 0.0f, 1.0f);
    p.virtualBodyScale = 1.0f;
    p.virtualLimbOwnership = std::clamp(neuralFrame.signalQuality, 0.0f, 1.0f);
    if (bodySensors.size() >= 6U) {
        p.contactPoints = {bodySensors[5]};
    } else if (p.virtualLimbOwnership > 0.0f && p.balance > 0.0f) {
        p.contactPoints = {0.01f};
    }
    return p;
}

bool PerceivedBodyPoseEstimator::isComplete(const BodyPoseState& pose) {
    return pose.balance > 0.0f && pose.virtualBodyScale > 0.0f && pose.virtualLimbOwnership > 0.0f && !pose.contactPoints.empty();
}

} // namespace dnxr
