#include "neural_chip/PipelineStepMetrics.h"

namespace dnxr {

void updatePipelineStepMetrics(PipelineStepMetrics& metrics, PipelineStepOutcome outcome) {
    ++metrics.totalSteps;
    switch (outcome) {
        case PipelineStepOutcome::NormalAccepted: ++metrics.normalAccepted; break;
        case PipelineStepOutcome::WaitingForAck: ++metrics.waitingForAck; break;
        case PipelineStepOutcome::HoldCommitted: ++metrics.holdCommitted; break;
        case PipelineStepOutcome::Rejected: ++metrics.rejected; break;
        case PipelineStepOutcome::EmergencyLatched: ++metrics.emergencyLatched; break;
        case PipelineStepOutcome::TraceInvalid: ++metrics.rejected; break;
        case PipelineStepOutcome::Shutdown: ++metrics.shutdown; break;
        case PipelineStepOutcome::NoOp: break;
    }
    const float denom = metrics.totalSteps == 0 ? 1.0f : static_cast<float>(metrics.totalSteps);
    metrics.acceptanceRatio01 = static_cast<float>(metrics.normalAccepted) / denom;
    metrics.holdRatio01 = static_cast<float>(metrics.holdCommitted) / denom;
    metrics.waitingRatio01 = static_cast<float>(metrics.waitingForAck) / denom;
}

} // namespace dnxr
