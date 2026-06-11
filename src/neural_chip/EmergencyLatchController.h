#pragma once

#include "neural_chip/EmergencyResetAuditTrail.h"
#include "neural_chip/NeuralChipEmergencyLatch.h"
#include "neural_chip/PipelineResumeController.h"

namespace dnxr {

class EmergencyLatchController {
public:
    EmergencyLatchController(NeuralChipEmergencyLatch& latch, EmergencyResetAuditTrail& audit, PipelineResumeController& resume);
    bool resetWithAudit(const EmergencyLatchResetRequest& request, double timestampMs);

private:
    NeuralChipEmergencyLatch& latch_;
    EmergencyResetAuditTrail& audit_;
    PipelineResumeController& resume_;
};

} // namespace dnxr
