#include "RuntimeAcquisitionStage.h"

#include "RuntimeStageServices.h"
#include "../devices/NanoNodeInterface.h"
#include "../devices/XRHeadsetInterface.h"
#include "../hardware/NeuralChipHAL.h"
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

    if (input.pullBioTelemetry) {
        frame.telemetry = input.pullBioTelemetry();
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

    if (input.readNeuralNodeSignals) {
        frame.rawNeuralSignals = input.readNeuralNodeSignals();
    }
    if (input.readChipFrame) {
        frame.chipFrame = input.readChipFrame();
        if (!frame.chipFrame.channels.empty()) {
            frame.rawNeuralSignals = frame.chipFrame.channels;
        }
    }

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

    frame.telemetry = services.headset.pullBioTelemetry();
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

    frame.rawNeuralSignals = services.nodes.readNeuralNodeSignals();
    frame.chipFrame = services.neuralChipHal.readFrame();
    if (!frame.chipFrame.channels.empty()) {
        frame.rawNeuralSignals = frame.chipFrame.channels;
    }

    frame.acquisitionStatus = validate(!frame.rawNeuralSignals.empty(), "empty_neural_signal_frame");
    frame.continueCycle = frame.acquisitionStatus.continueCycle;
    frame.emergencyActive = frame.acquisitionStatus.emergencyActive;
    if (!frame.continueCycle) {
        frame.stopReason = frame.acquisitionStatus.reason;
    }
    return frame;
}

} // namespace dnxr
