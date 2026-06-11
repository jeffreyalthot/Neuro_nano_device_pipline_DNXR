#include "neural_chip/PipelineSafetyCaseReport.h"

namespace dnxr {

PipelineSafetyCaseReport PipelineSafetyCaseReportBuilder::build(const NeuralChipReplayRecorder& recorder,
                                                                bool noDirectBusAccessPassed,
                                                                bool noElectricalParamsPassed,
                                                                bool noHumanReadyClaimsPassed,
                                                                bool traceCoveragePassed) const {
    PipelineSafetyCaseReport r;
    r.noDirectBusAccessPassed = noDirectBusAccessPassed;
    r.noElectricalParamsPassed = noElectricalParamsPassed;
    r.noHumanReadyClaimsPassed = noHumanReadyClaimsPassed;
    r.traceCoveragePassed = traceCoveragePassed;
    for (const auto& record : recorder.records()) {
        ++r.totalFrames;
        r.replayDigestPresent = r.replayDigestPresent || record.sessionDigestHash != 0;
        if (record.pipelineState == NeuralChipPipelineState::ControlledResume) ++r.controlledResumeFrames;
        switch (record.pipelinePathKind) {
            case PipelinePathKind::NormalCommit: ++r.normalCommits; break;
            case PipelinePathKind::AckMissingHold: ++r.ackHolds; break;
            case PipelinePathKind::WorldFrameReject: ++r.worldRejects; break;
            case PipelinePathKind::EarlyEncoderReject: ++r.encoderRejects; break;
            case PipelinePathKind::CapabilityHold: ++r.capabilityHolds; break;
            case PipelinePathKind::RealityAnchorHold: ++r.realityAnchorHolds; break;
            case PipelinePathKind::LifecycleReject: ++r.lifecycleRejects; break;
            case PipelinePathKind::ValidatorReject: ++r.validatorRejects; break;
            case PipelinePathKind::EmergencyLatchHold: ++r.emergencyLatches; break;
            case PipelinePathKind::ResumeHold:
            case PipelinePathKind::SessionSupervisorHold: break;
        }
    }
    r.summary = "Pipeline safety case: frames=" + std::to_string(r.totalFrames) + ", normal=" + std::to_string(r.normalCommits) + ", holds=" + std::to_string(r.ackHolds + r.emergencyLatches);
    return r;
}

} // namespace dnxr
