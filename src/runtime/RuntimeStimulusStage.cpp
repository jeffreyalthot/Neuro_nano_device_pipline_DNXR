#include "RuntimeStimulusStage.h"

#include "RuntimeStageServices.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/NanoNodeInterface.h"
#include "../hardware/NeuralChipHAL.h"
#include "../safety/IndependentWatchdog.h"
#include "../safety/NeuralStimulusSafetyEnvelope.h"

#include <algorithm>

namespace dnxr {

const char* RuntimeStimulusStage::name() {
    return "stimulus";
}

RuntimeStageStatus RuntimeStimulusStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeStimulusStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }
    frame.stimulusStatus = validate(
        input.buildCommand && input.writeSafeStimulus && input.downlink && input.writeNeuralStimulus,
        "stimulus_stage_unavailable");
    if (!frame.stimulusStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.stimulusStatus.reason;
        return frame;
    }

    frame.stimulusCommand = input.buildCommand(
        frame.cycleId,
        frame.sensoryFrame,
        frame.fusedSafetyDecision,
        std::max(0.0, frame.deltaTimeSeconds));
    if (input.heartbeatStimulus) {
        input.heartbeatStimulus();
    }

    if (!frame.stimulusCommand.allowStimulusDispatch) {
        frame.stimulusStatus = RuntimeStageStatus::degraded("stimulus_dispatch_suppressed");
        return frame;
    }

    if (!input.writeSafeStimulus(frame.stimulusCommand)) {
        if (input.emergencyStopHardware) {
            input.emergencyStopHardware("unsafe_stimulus_rejected");
        }
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "unsafe_stimulus_rejected";
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    const auto down = input.downlink(frame.sensoryFrame);
    input.writeNeuralStimulus(down);
    return frame;
}

RuntimePipelineFrame RuntimeStimulusStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.stimulusCommand = services.stimulusEnvelope.buildCommand(
        frame.cycleId,
        frame.sensoryFrame,
        frame.fusedSafetyDecision,
        std::max(0.0, frame.deltaTimeSeconds));
    services.watchdog.heartbeat("stimulus");

    if (!frame.stimulusCommand.allowStimulusDispatch) {
        frame.stimulusStatus = RuntimeStageStatus::degraded("stimulus_dispatch_suppressed");
        return frame;
    }

    if (!services.neuralChipHal.writeSafeStimulus(frame.stimulusCommand)) {
        services.neuralChipHal.emergencyStop("unsafe_stimulus_rejected");
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "unsafe_stimulus_rejected";
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    const auto down = services.bridge.downlink(frame.sensoryFrame);
    services.nodes.writeNeuralStimulus(down);
    frame.stimulusStatus = validate(true, "stimulus_stage_unhealthy");
    return frame;
}

} // namespace dnxr
