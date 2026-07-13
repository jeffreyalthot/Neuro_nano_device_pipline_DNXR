#include "RuntimeMotorIntentStage.h"

#include "../full_dive/motor_chip/AvatarKinematicRigController.h"
#include "../full_dive/motor_chip/MotorIntentConfidenceGate.h"
#include "../full_dive/motor_chip/MotorIntentDecoder.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace dnxr {
namespace {

float clamp01(float value) {
    if (!std::isfinite(value)) {
        return 0.0f;
    }
    return std::clamp(value, 0.0f, 1.0f);
}

bool finiteSamples(const std::vector<float>& samples) {
    return std::all_of(samples.begin(), samples.end(), [](float value) { return std::isfinite(value); });
}

float acquisitionSignalQuality(const BciAcquisitionFrame& frame) {
    if (!frame.external || !frame.attested || !frame.authenticated || frame.samples.empty()) {
        return 0.0f;
    }
    float quality = 1.0f;
    if ((frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Dropout)) != 0U) quality -= 0.35f;
    if ((frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Overflow)) != 0U) quality -= 0.35f;
    if ((frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Corrupt)) != 0U) quality = 0.0f;
    if ((frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Replay)) != 0U) quality = 0.0f;
    if ((frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Stale)) != 0U) quality = 0.0f;
    if (frame.dropoutCount > 0U) quality -= std::min(0.5f, static_cast<float>(frame.dropoutCount) * 0.05f);
    if (frame.overflowCount > 0U) quality -= std::min(0.5f, static_cast<float>(frame.overflowCount) * 0.05f);
    return clamp01(quality);
}

std::uint64_t hashCommand(const MotorIntentCommand& command) {
    std::uint64_t h = 1469598103934665603ULL;
    auto mix = [&](std::uint64_t v) { h ^= v; h *= 1099511628211ULL; };
    mix(command.frameId);
    mix(static_cast<std::uint64_t>(command.confidence01 * 1000000.0f));
    mix(static_cast<std::uint64_t>(command.intent.moveForward01 * 1000000.0f));
    mix(static_cast<std::uint64_t>(command.intent.turnYaw01 * 1000000.0f));
    mix(static_cast<std::uint64_t>(command.intent.stopIntent01 * 1000000.0f));
    return h;
}

VirtualWorldMotorIngress toWorldMotorIntent(const NeuralMotorIntentVector& intent) {
    VirtualWorldMotorIngress out;
    out.moveForward01 = clamp01(intent.moveForward01);
    out.turnYaw01 = clamp01(intent.turnYaw01);
    out.postureBalance01 = clamp01(intent.postureBalance01);
    out.stopIntent01 = clamp01(intent.stopIntent01);
    out.confidence01 = clamp01(intent.confidence01);
    return out;
}

} // namespace

const char* RuntimeMotorIntentStage::name() { return "motor_intent"; }

CausalMotorIntentStageResult RuntimeMotorIntentStage::decode(const RuntimePipelineFrame& frame,
                                                             const CausalMotorIntentModelIdentity& model) {
    CausalMotorIntentStageResult result;
    result.provenance = "external_bci_acquisition_only";
    result.command.frameId = frame.cycleId;
    result.command.validFromFrameId = frame.cycleId;
    result.command.validUntilFrameId = frame.cycleId;
    result.command.timestampMs = frame.timestamp * 1000.0;
    result.command.provenance = result.provenance;
    result.command.safetyDisposition = MotorIntentSafetyDisposition::Hold;
    result.command.dispositionReason = "neutral_hold_default";
    result.command.intent.stopIntent01 = 1.0f;

    if (model.schemaVersion != 1U || model.modelDigest.empty() || !model.signedManifest || !model.deterministic) {
        result.reason = "motor_intent_model_manifest_invalid";
        result.command.dispositionReason = result.reason;
        result.command.commandHash = hashCommand(result.command);
        return result;
    }
    const auto& acquisition = frame.bciAcquisitionFrame;
    if (!acquisition.external || !acquisition.attested || !acquisition.authenticated || acquisition.sequence == 0U) {
        result.reason = "motor_intent_requires_valid_external_bci_acquisition";
        result.command.dispositionReason = result.reason;
        result.command.commandHash = hashCommand(result.command);
        return result;
    }
    if (!finiteSamples(acquisition.samples) || !finiteSamples(frame.cleanSignals) || !finiteSamples(frame.features)) {
        result.reason = "motor_intent_nonfinite_bci_input";
        result.command.dispositionReason = result.reason;
        result.command.commandHash = hashCommand(result.command);
        return result;
    }

    result.signalQuality01 = acquisitionSignalQuality(acquisition);
    MotorCortexReadFrame readFrame;
    const auto featureAt = [&](std::size_t index) { return index < frame.features.size() ? clamp01(frame.features[index]) : 0.0f; };
    readFrame.forward01 = featureAt(0);
    readFrame.backward01 = featureAt(1);
    readFrame.left01 = featureAt(2);
    readFrame.right01 = featureAt(3);
    readFrame.yaw01 = featureAt(4);
    readFrame.pitch01 = featureAt(5);
    readFrame.rightReach01 = featureAt(6);
    readFrame.leftReach01 = featureAt(7);
    readFrame.rightGrip01 = featureAt(8);
    readFrame.leftGrip01 = featureAt(9);
    readFrame.gait01 = featureAt(10);
    readFrame.balance01 = featureAt(11);
    readFrame.stop01 = featureAt(12);
    readFrame.tremor01 = featureAt(13);
    readFrame.spasm01 = featureAt(14);
    readFrame.fatigue01 = featureAt(15);
    readFrame.confidence01 = result.signalQuality01 * clamp01(frame.diagnostics.intentConfidence);
    NeuralMotorIntentVector decoded = MotorIntentDecoder{}.decode(readFrame);
    MotorIntentConfidenceDecision gateDecision{};
    decoded = MotorIntentConfidenceGate{}.apply(decoded, result.signalQuality01, &gateDecision);
    decoded.confidence01 = clamp01(std::min(decoded.confidence01, result.signalQuality01));

    result.confidence01 = decoded.confidence01;
    result.uncertainty01 = clamp01(1.0f - result.confidence01);
    result.command.confidence01 = result.confidence01;
    result.command.intent = decoded;
    result.command.rig = AvatarKinematicRigController{}.apply(decoded);

    if (gateDecision.lowConfidence || result.confidence01 < clamp01(model.minConfidence01)) {
        result.neutralHold = true;
        result.accepted = false;
        result.reason = gateDecision.reason && gateDecision.reason[0] != '\0' ? gateDecision.reason : "motor_intent_confidence_below_policy";
        result.command.safetyDisposition = MotorIntentSafetyDisposition::Hold;
        result.command.intent = NeuralMotorIntentVector{};
        result.command.intent.stopIntent01 = 1.0f;
        result.command.dispositionReason = result.reason;
        result.command.confidence01 = result.confidence01;
        result.command.commandHash = hashCommand(result.command);
        return result;
    }

    result.neutralHold = false;
    result.accepted = true;
    result.reason = "motor_intent_decoded_for_world_egress";
    result.command.safetyDisposition = gateDecision.safetyPoseRequired ? MotorIntentSafetyDisposition::SafetyPose : MotorIntentSafetyDisposition::Normal;
    result.command.dispositionReason = result.reason;
    result.command.commandHash = hashCommand(result.command);
    return result;
}

RuntimePipelineFrame RuntimeMotorIntentStage::execute(RuntimePipelineFrame frame,
                                                      IMotorWorldEgress* motorWorldEgress,
                                                      const CausalMotorIntentModelIdentity& model) {
    if (!frame.continueCycle) {
        return frame;
    }
    frame.motorIntentResult = decode(frame, model);
    frame.motorIntentCommand = frame.motorIntentResult.command;
    if (frame.motorIntentResult.accepted && motorWorldEgress != nullptr) {
        frame.motorIntentResult.submittedToWorld = motorWorldEgress->publishMotorIntent(
            frame.cycleId,
            frame.worldIngressFrame.metadata.sessionId,
            toWorldMotorIntent(frame.motorIntentResult.command.intent));
    }
    return frame;
}

} // namespace dnxr
