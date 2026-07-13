#include "RuntimeAcquisitionStage.h"

#include "RuntimeStageServices.h"
#include "../safety/IndependentWatchdog.h"
#include "../safety/PhysiologicalGuard.h"

namespace dnxr {

const char* RuntimeAcquisitionStage::name() {
    return "acquisition";
}

RuntimeStageStatus RuntimeAcquisitionStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeAcquisitionStage::execute(const RuntimeStageContext& context, const Input& input) {
    RuntimePipelineFrame frame;
    frame.cycleId = context.cycleId;
    frame.timestamp = context.timestamp;
    frame.deltaTimeSeconds = context.deltaTimeSeconds;

    BiometricSample biometricSample;
    biometricSample.timestampSeconds = context.timestamp;
    biometricSample.provenance = "no_biometric_callback_configured";
    biometricSample.validity = BiometricValidity::Unavailable;
    if (input.readBiometricSample) {
        biometricSample = input.readBiometricSample(context.timestamp);
    }
    if (biometricSample.usable()) {
        frame.telemetry = biometricSample.channels;
    }
    if (input.heartbeatRuntimeCycle) {
        input.heartbeatRuntimeCycle();
    }
    if (input.heartbeatAcquisition) {
        input.heartbeatAcquisition();
    }
    if (input.enforcePhysiologicalGuard && !input.enforcePhysiologicalGuard(frame.telemetry)) {
        frame.acquisitionStatus = RuntimeStageStatus::emergency("physiological_guard_triggered");
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = frame.acquisitionStatus.reason;
        return frame;
    }

    BciAcquisitionReadResult bciRead;
    if (input.readCanonicalBciFrame) {
        bciRead = input.readCanonicalBciFrame(context);
    }
    if (!bciRead.valid) {
        frame.acquisitionStatus = RuntimeStageStatus::degraded(
            bciRead.failureReason.empty() ? "bci_acquisition_source_unavailable" : bciRead.failureReason);
        frame.continueCycle = false;
        frame.stopReason = frame.acquisitionStatus.reason;
        return frame;
    }
    frame.bciAcquisitionFrame = bciRead.frame;
    frame.chipFrame.sequenceId = bciRead.frame.sequence;
    frame.chipFrame.monotonicTimestampMs = bciRead.frame.hardwareTimestampNs / 1000000LL;
    frame.chipFrame.channels = bciRead.frame.samples;
    frame.chipFrame.signalQuality = 1.0f;
    frame.chipFrame.simulation = bciRead.frame.recordedNonHumanFixture;
    frame.acquisitionChipFrame = frame.chipFrame;
    frame.rawNeuralSignals = bciRead.frame.samples;

    frame.acquisitionStatus = validate(!frame.rawNeuralSignals.empty(), "empty_neural_signal_frame");
    frame.continueCycle = frame.acquisitionStatus.continueCycle;
    frame.emergencyActive = frame.acquisitionStatus.emergencyActive;
    if (!frame.continueCycle) {
        frame.stopReason = frame.acquisitionStatus.reason;
    }
    return frame;
}

RuntimePipelineFrame RuntimeAcquisitionStage::execute(const RuntimeStageContext& context, RuntimeStageServices& services) {
    RuntimePipelineFrame frame;
    frame.cycleId = context.cycleId;
    frame.timestamp = context.timestamp;
    frame.deltaTimeSeconds = context.deltaTimeSeconds;

    const BiometricSample biometricSample = services.biometricSource.readBiometricSample(context.timestamp);
    if (biometricSample.usable()) {
        frame.telemetry = biometricSample.channels;
    }
    services.watchdog.heartbeat("runtime_cycle");
    services.watchdog.heartbeat("acquisition");
    std::vector<float> guardTelemetry = frame.telemetry;
    if (guardTelemetry.size() > 1U) {
        guardTelemetry[1] = 12.0f + guardTelemetry[1] * 8.0f;
    }
    if (!services.guard.enforce(guardTelemetry)) {
        frame.acquisitionStatus = RuntimeStageStatus::emergency("physiological_guard_triggered");
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = frame.acquisitionStatus.reason;
        return frame;
    }

    const BciAcquisitionReadResult bciRead = services.bciAcquisitionSource.readBciAcquisition(context);
    if (!bciRead.valid) {
        frame.acquisitionStatus = RuntimeStageStatus::degraded(
            bciRead.failureReason.empty() ? "bci_acquisition_source_unavailable" : bciRead.failureReason);
        frame.continueCycle = false;
        frame.stopReason = frame.acquisitionStatus.reason;
        return frame;
    }
    frame.bciAcquisitionFrame = bciRead.frame;
    frame.chipFrame.sequenceId = bciRead.frame.sequence;
    frame.chipFrame.monotonicTimestampMs = bciRead.frame.hardwareTimestampNs / 1000000LL;
    frame.chipFrame.channels = bciRead.frame.samples;
    frame.chipFrame.signalQuality = 1.0f;
    frame.chipFrame.simulation = bciRead.frame.recordedNonHumanFixture;
    frame.acquisitionChipFrame = frame.chipFrame;
    frame.rawNeuralSignals = bciRead.frame.samples;

    frame.acquisitionStatus = validate(!frame.rawNeuralSignals.empty(), "empty_neural_signal_frame");
    frame.continueCycle = frame.acquisitionStatus.continueCycle;
    frame.emergencyActive = frame.acquisitionStatus.emergencyActive;
    if (!frame.continueCycle) {
        frame.stopReason = frame.acquisitionStatus.reason;
    }
    return frame;
}

} // namespace dnxr
