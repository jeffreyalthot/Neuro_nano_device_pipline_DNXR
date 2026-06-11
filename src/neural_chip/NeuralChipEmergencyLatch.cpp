#include "neural_chip/NeuralChipEmergencyLatch.h"

namespace dnxr {

bool NeuralChipEmergencyLatch::present(const char* value) { return value && value[0] != '\0'; }
void NeuralChipEmergencyLatch::trigger(const char* reason) { latched_ = true; reason_ = reason ? reason : "emergency latch"; }
bool NeuralChipEmergencyLatch::latched() const { return latched_; }
const char* NeuralChipEmergencyLatch::reason() const { return reason_; }
bool NeuralChipEmergencyLatch::resetWithOperatorAuthorization(const EmergencyLatchResetRequest& request) {
    if (!present(request.operatorId) || !present(request.reason) || !request.simulationOperatorConfirmed || !request.safetyChecklistConfirmed) {
        return false;
    }
    latched_ = false;
    reason_ = "";
    return true;
}

} // namespace dnxr
