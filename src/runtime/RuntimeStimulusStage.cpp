#include "RuntimeStimulusStage.h"

#include "RuntimeStageServices.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/NanoNodeInterface.h"
#include "../hardware/NeuralChipHAL.h"
#include "../hardware/DeviceCapabilityLimiter.h"
#include "../hardware/NeuralChipSafetySupervisor.h"
#include "../full_dive/NeuralStimulusTranslator.h"
#include "../full_dive/spike_bridge/PPPGrammarFrameBuilder.h"
#include "../safety/FullDiveSafetySupervisor.h"
#include "../safety/IndependentStimulusCutoff.h"
#include "../safety/NoPainGuarantee.h"
#include "../safety/IndependentWatchdog.h"
#include "../safety/NeuralStimulusSafetyEnvelope.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

namespace {

void addCommandToPacket(ModalityVectorPacket& packet,
                        const NeuroChannelCommand& command) {
    if (packet.channelCount < packet.channels01.size()) {
        packet.channels01[packet.channelCount++] =
            spikeClamp01(command.intensity);
    }
    packet.confidence01 =
        std::max(packet.confidence01, spikeClamp01(command.confidence));
    packet.risk01 =
        std::max(packet.risk01,
                 spikeClamp01(command.intensity - command.safetyLimit));
    packet.priority01 = std::max(packet.priority01, spikeClamp01(command.intensity));
}

ModalityVectorPacket makeRuntimeGrammarPacket(
    std::uint64_t frameId,
    double timestampMs,
    SpikeModality modality,
    const char* sourceEncoder) {
    ModalityVectorPacket packet;
    packet.frameId = frameId;
    packet.timestampMs = timestampMs;
    packet.modality = modality;
    packet.sourceEncoder = sourceEncoder;
    packet.enabled = true;
    packet.confidence01 = 0.0f;
    packet.sceneBinding01 = 1.0f;
    packet.patientMapConfidence01 = 1.0f;
    packet.latencyBudgetMs = 16.0f;
    packet.sensoryLike = modality != SpikeModality::Motor &&
                         modality != SpikeModality::RealityAnchor;
    packet.motorLike = modality == SpikeModality::Motor;
    return packet;
}

VectorBundleFrame buildRuntimeGrammarVectorBundle(
    const RuntimePipelineFrame& frame) {
    VectorBundleFrame bundle;
    bundle.frameId = frame.fullDiveNeuralFrame.sequenceId != 0
                         ? frame.fullDiveNeuralFrame.sequenceId
                         : frame.cycleId;
    bundle.timestampMs = frame.timestamp * 1000.0;
    bundle.modalityCount = DNXR_SPIKE_BRIDGE_MODALITY_COUNT;
    bundle.modalities = {
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Visual,
                                 "RuntimeFullDiveStage.visual"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Auditory,
                                 "RuntimeFullDiveStage.auditory"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Touch,
                                 "RuntimeFullDiveStage.somatosensory"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Proprioception,
                                 "RuntimeFullDiveStage.proprioception"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Vestibular,
                                 "RuntimeFullDiveStage.vestibular"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Motor,
                                 "RuntimeStimulusStage.avatar_motor"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Olfactory,
                                 "RuntimeFullDiveStage.olfactory"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::Gustatory,
                                 "RuntimeFullDiveStage.gustatory"),
        makeRuntimeGrammarPacket(bundle.frameId, bundle.timestampMs,
                                 SpikeModality::RealityAnchor,
                                 "RuntimeFullDiveStage.reality_anchor")};

    for (const auto& command : frame.fullDiveNeuralFrame.visual.channels) {
        addCommandToPacket(bundle.modalities[0], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.auditory.channels) {
        addCommandToPacket(bundle.modalities[1], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.somatosensory.channels) {
        addCommandToPacket(bundle.modalities[2], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.proprioception.channels) {
        addCommandToPacket(bundle.modalities[3], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.vestibular.channels) {
        addCommandToPacket(bundle.modalities[4], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.smellTaste.olfactory.channels) {
        addCommandToPacket(bundle.modalities[6], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.smellTaste.gustatory.channels) {
        addCommandToPacket(bundle.modalities[7], command);
    }
    for (const auto& command : frame.fullDiveNeuralFrame.realityAnchor.channels) {
        addCommandToPacket(bundle.modalities[8], command);
    }

    float risk = spikeClamp01(frame.fullDiveNeuralFrame.safety.aggregateRisk);
    float coherence = 1.0f;
    for (std::size_t index = 0; index < bundle.modalityCount; ++index) {
        auto& packet = bundle.modalities[index];
        if (packet.channelCount == 0) {
            packet.confidence01 = packet.modality == SpikeModality::RealityAnchor
                                      ? frame.fullDiveNeuralFrame.realityAnchor.anchorIntensity
                                      : 1.0f;
        }
        packet.risk01 = std::max(packet.risk01, risk);
        coherence = std::min(coherence, packet.confidence01);
        sanitize(packet);
    }
    bundle.aggregateRisk01 = risk;
    bundle.globalCoherence01 = coherence;
    bundle.realityAnchor01 = spikeClamp01(
        frame.fullDiveNeuralFrame.realityAnchor.anchorRequired
            ? frame.fullDiveNeuralFrame.realityAnchor.anchorIntensity
            : 0.0f);
    sanitize(bundle);
    return bundle;
}

std::size_t effectiveStimulusChannelLimit(const ChipCapabilities& capabilities) {
    std::size_t limit = DNXR_MAX_STIMULUS_CHANNELS;
    if (capabilities.electrodeCount > 0) {
        limit = std::min(limit, capabilities.electrodeCount);
    }
    if (capabilities.wireCount > 0) {
        limit = std::min(limit, capabilities.wireCount *
                                    DNXR_NEURAL_CHIP_CHANNELS_PER_WIRE);
    }
    if (capabilities.maxStimulusChannels > 0) {
        limit = std::min(limit, capabilities.maxStimulusChannels);
    }
    return std::max<std::size_t>(1, limit);
}

void adaptCommandToChipTopology(SafeNeuralStimulusCommandFixed& command,
                                const ChipCapabilities& capabilities) {
    const std::size_t channelLimit = effectiveStimulusChannelLimit(capabilities);
    if (command.boundedChannels.size() <= channelLimit) {
        return;
    }

    FixedVector<float, DNXR_MAX_STIMULUS_CHANNELS> limited;
    const std::size_t copyCount = std::min(channelLimit, command.boundedChannels.size());
    for (std::size_t index = 0; index < copyCount; ++index) {
        limited.push_back(command.boundedChannels[index]);
    }
    command.boundedChannels = limited;
    command.aggregateSensoryLoad = 0.0f;
    for (float channel : command.boundedChannels) {
        command.aggregateSensoryLoad =
            std::max(command.aggregateSensoryLoad, std::abs(channel));
    }
    command.coverageReason = "adapted_to_neural_chip_electrode_wire_topology";
}

} // namespace

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

    frame.stimulusCommand = toFixedCommand(input.buildCommand(
        frame.cycleId,
        frame.sensoryFrame,
        frame.fusedSafetyDecision,
        std::max(0.0, frame.deltaTimeSeconds)));
    if (input.heartbeatStimulus) {
        input.heartbeatStimulus();
    }

    if (!frame.stimulusCommand.allowStimulusDispatch) {
        frame.stimulusStatus = RuntimeStageStatus::degraded("stimulus_dispatch_suppressed");
        return frame;
    }

    if (!input.writeSafeStimulus(toLegacyCommand(frame.stimulusCommand))) {
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
    const float runtimeRateHz = frame.deltaTimeSeconds > 0.0 ? static_cast<float>(1.0 / frame.deltaTimeSeconds) : DNXR_NEURAL_CHIP_TARGET_FRAME_RATE_HZ;
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

    PPPGrammarFrameBuilder grammarBuilder;
    const auto ackBeforeDispatch = services.neuralChipHal.lastAck();
    NeuralChipSessionState grammarSession;
    grammarSession.realityAnchorStability01 =
        frame.fullDiveNeuralFrame.realityAnchor.anchorIntensity;
    grammarSession.sessionRisk01 =
        frame.fullDiveNeuralFrame.safety.aggregateRisk;
    grammarSession.emergencyStopLatched = frame.emergencyActive;
    frame.neuralChipGrammarFrame = grammarBuilder.build(
        buildRuntimeGrammarVectorBundle(frame), grammarSession,
        ackBeforeDispatch);
    frame.neuralChipGrammarFrame.hardwareElectrodeCount =
        clinicalCapabilities.electrodeCount;
    frame.neuralChipGrammarFrame.hardwareWireCount =
        clinicalCapabilities.wireCount;
    frame.neuralChipGrammarFrame.hardwareStimulusChannelLimit =
        effectiveStimulusChannelLimit(clinicalCapabilities);

    NeuralStimulusTranslator translator;
    auto translation = translator.translate(frame.fullDiveNeuralFrame);
    frame.stimulusCommand = translation.command;
    adaptCommandToChipTopology(frame.stimulusCommand, clinicalCapabilities);
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

#if DNXR_HUMAN_USE_BUILD
    static_assert(true, "modern RuntimeStimulusStage must not dispatch through legacy neural nodes");
#endif
    frame.stimulusStatus = validate(true, "stimulus_stage_unhealthy");
    return frame;
}
} // namespace dnxr
