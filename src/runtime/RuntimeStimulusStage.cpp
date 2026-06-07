#include "RuntimeStimulusStage.h"

#include "RuntimeStageServices.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/NanoNodeInterface.h"
#include "../hardware/NeuralChipHAL.h"
#include "../hardware/DeviceCapabilityLimiter.h"
#include "../hardware/NeuralChipSafetySupervisor.h"
#include "../full_dive/NeuralStimulusTranslator.h"
#include "../safety/FullDiveSafetySupervisor.h"
#include "../safety/IndependentStimulusCutoff.h"
#include "../safety/NoPainGuarantee.h"
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

    FullDiveSafetySupervisor safetySupervisor;
    const auto safetyDecision = safetySupervisor.evaluate(frame.fullDiveNeuralFrame);
    if (!safetyDecision.allowed && safetyDecision.requiresEmergencyStop) {
        services.neuralChipHal.emergencyStop(safetyDecision.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = safetyDecision.reason;
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    DeviceCapabilityLimiter capabilityLimiter;
    ChipCapabilityNegotiation negotiation;
    const ChipCapabilities clinicalCapabilities = negotiation.clampForClinical(services.neuralChipHal.capabilities());
    if (!services.neuralChipHal.hardwareCutoffAvailable() || !services.neuralChipHal.closedLoopFeedbackAvailable()) {
        services.neuralChipHal.emergencyStop("neural_chip_required_safety_capability_missing");
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "neural_chip_required_safety_capability_missing";
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }
    const float runtimeRateHz = frame.deltaTimeSeconds > 0.0 ? static_cast<float>(1.0 / frame.deltaTimeSeconds) : 90.0f;
    auto limited = capabilityLimiter.apply(frame.fullDiveNeuralFrame, clinicalCapabilities, runtimeRateHz);
    if (!limited.allowed) {
        services.neuralChipHal.emergencyStop(limited.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = limited.reason;
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }
    frame.fullDiveNeuralFrame = limited.frame;

    NeuralStimulusTranslator translator;
    auto translation = translator.translate(frame.fullDiveNeuralFrame);
    frame.stimulusCommand = translation.command;
    services.watchdog.heartbeat("stimulus");

    NoPainGuarantee noPain;
    const auto painDecision = noPain.evaluate(PainSafetyInput{});
    if (!painDecision.safe) {
        IndependentStimulusCutoff cutoff;
        const auto cut = cutoff.zeroAllOutputs(frame.stimulusCommand, painDecision.reason);
        frame.stimulusCommand = cut.command;
        services.neuralChipHal.emergencyStop(painDecision.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = painDecision.reason;
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    if (!translation.valid || !frame.stimulusCommand.allowStimulusDispatch) {
        frame.stimulusStatus = RuntimeStageStatus::degraded(translation.reason);
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

    const auto ack = services.neuralChipHal.lastAck();
    NeuralChipSafetySupervisor chipSupervisor;
    const auto chipDecision = chipSupervisor.evaluate(ack, services.neuralChipHal.capabilities(), services.neuralChipHal.deviceIdentity(), frame.stimulusCommand.sequenceId, &clinicalCapabilities);
    if (!chipDecision.allowed) {
        if (chipDecision.requiresEmergencyStop) {
            services.neuralChipHal.emergencyStop(chipDecision.reason);
        }
        frame.continueCycle = false;
        frame.emergencyActive = chipDecision.requiresEmergencyStop;
        frame.stopReason = chipDecision.reason;
        frame.stimulusStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    const auto down = services.bridge.downlink(frame.sensoryFrame);
    services.nodes.writeNeuralStimulus(down);
    frame.stimulusStatus = validate(true, "stimulus_stage_unhealthy");
    return frame;
}
} // namespace dnxr
