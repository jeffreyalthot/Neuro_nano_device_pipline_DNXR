#include "neural_chip/EmergencyLatchController.h"

namespace dnxr {

EmergencyLatchController::EmergencyLatchController(NeuralChipEmergencyLatch& latch, EmergencyResetAuditTrail& audit, PipelineResumeController& resume)
    : latch_(latch), audit_(audit), resume_(resume) {}

bool EmergencyLatchController::resetWithAudit(const EmergencyLatchResetRequest& request, double timestampMs) {
    EmergencyResetAuditRecord record;
    record.sessionId = request.sessionId;
    record.operatorId = request.operatorId;
    record.reason = request.reason;
    record.timestampMs = timestampMs;
    record.simulationOperatorConfirmed = request.simulationOperatorConfirmed;
    record.safetyChecklistConfirmed = request.safetyChecklistConfirmed;
    record.resetAccepted = latch_.resetWithOperatorAuthorization(request);
    audit_.record(record);
    if (record.resetAccepted) resume_.beginControlledResume();
    return record.resetAccepted;
}

} // namespace dnxr
