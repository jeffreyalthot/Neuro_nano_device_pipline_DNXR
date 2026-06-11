#include "neural_chip/NeuralChipEmergencyLatch.h"

namespace dnxr {

void NeuralChipEmergencyLatch::trigger(const char* reason) { latched_ = true; reason_ = reason ? reason : "emergency latch"; }
bool NeuralChipEmergencyLatch::latched() const { return latched_; }
const char* NeuralChipEmergencyLatch::reason() const { return reason_; }
void NeuralChipEmergencyLatch::resetWithOperatorAuthorization() { latched_ = false; reason_ = ""; }

} // namespace dnxr
