#include "neural_chip/NeuralChipTimebase.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

double NeuralChipTimebase::nowMs() const { return nowMs_; }
void NeuralChipTimebase::setSimulatedNowMs(double nowMs) { nowMs_ = std::isfinite(nowMs) && nowMs >= 0.0 ? nowMs : 0.0; }
void NeuralChipTimebase::advanceMs(double deltaMs) { if (std::isfinite(deltaMs) && deltaMs > 0.0) nowMs_ += deltaMs; }

void NeuralChipTimebase::beginStep(double worldTimestampMs) {
    if (std::isfinite(worldTimestampMs) && worldTimestampMs > nowMs_) {
        nowMs_ = worldTimestampMs;
    } else if (policy_.advanceOnEveryStep) {
        nowMs_ += policy_.defaultStepMs;
    }
}

void NeuralChipTimebase::setPolicy(NeuralChipTimebasePolicy policy) { policy_ = policy; }
const NeuralChipTimebasePolicy& NeuralChipTimebase::policy() const { return policy_; }

} // namespace dnxr
