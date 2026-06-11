#include "neural_chip/NeuralChipTimebase.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

double NeuralChipTimebase::nowMs() const { return nowMs_; }
void NeuralChipTimebase::setSimulatedNowMs(double nowMs) { nowMs_ = std::isfinite(nowMs) && nowMs >= 0.0 ? nowMs : 0.0; }
void NeuralChipTimebase::advanceMs(double deltaMs) { if (std::isfinite(deltaMs) && deltaMs > 0.0) nowMs_ += deltaMs; }

} // namespace dnxr
