#include "RuntimeSignalStage.h"

#include "RuntimeStageServices.h"
#include "../devices/HostComputerBridge.h"
#include "../safety/IndependentWatchdog.h"
#include "../signal/FeatureVectorizer.h"
#include "../signal/SignalStreamProcessor.h"
#include "../neurotech/NeuralArtifactClassifier.h"
#include "../neurotech/NeuralSignalQualityGate.h"

#include <algorithm>

namespace dnxr {

const char* RuntimeSignalStage::name() {
    return "signal";
}

RuntimeStageStatus RuntimeSignalStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeSignalStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    std::vector<float> decrypted;
    if (input.encrypt && input.decrypt) {
        const auto encrypted = input.encrypt(frame.rawNeuralSignals);
        decrypted = input.decrypt(encrypted);
    }
    const std::vector<float>& transportFrame = decrypted.empty() ? frame.rawNeuralSignals : decrypted;
    const auto uplinked = input.uplink ? input.uplink(transportFrame) : transportFrame;
    frame.cleanSignals = input.denoise ? input.denoise(uplinked) : uplinked;
    frame.features = input.vectorize ? input.vectorize(frame.cleanSignals) : frame.cleanSignals;
    NeuralArtifactClassifier artifactClassifier;
    NeuralSignalQualityGate qualityGate;
    frame.artifactReport = artifactClassifier.classify(frame.cleanSignals);
    frame.repeatedArtifactCount = frame.artifactReport.artifactProbability > 0.7f ? frame.repeatedArtifactCount + 1 : 0;
    const float signalQuality = frame.cleanSignals.empty() ? 0.0f : std::clamp(1.0f - frame.artifactReport.artifactProbability, 0.0f, 1.0f);
    frame.signalGateDecision = qualityGate.evaluate(signalQuality, frame.artifactReport.signalDropout > 0.5f, frame.artifactReport.artifactProbability, frame.repeatedArtifactCount);
    if (input.heartbeatSignalProcessing) {
        input.heartbeatSignalProcessing();
    }
    if (frame.signalGateDecision.action == NeuralSignalGateAction::BlockStimulus) {
        frame.fullDiveNeuralFrame = {};
        frame.signalStatus = RuntimeStageStatus::emergency(frame.signalGateDecision.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = frame.signalGateDecision.reason;
        return frame;
    }
    if (frame.signalGateDecision.action == NeuralSignalGateAction::BlockIntent) {
        frame.features.clear();
    }
    if (frame.signalGateDecision.action == NeuralSignalGateAction::RealityAnchor ||
        frame.signalGateDecision.action == NeuralSignalGateAction::HoldLastSafeFrame ||
        frame.signalGateDecision.action == NeuralSignalGateAction::Recalibration) {
        frame.fullDiveNeuralFrame.realityAnchor.anchorRequired = true;
    }
    frame.signalStatus = validate(!frame.features.empty(), frame.signalGateDecision.intentAllowed ? "empty_feature_vector" : frame.signalGateDecision.reason);
    frame.continueCycle = frame.signalStatus.continueCycle;
    if (!frame.continueCycle) {
        frame.stopReason = frame.signalStatus.reason;
    }
    return frame;
}

RuntimePipelineFrame RuntimeSignalStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services) {
    if (!frame.continueCycle) {
        return frame;
    }

    const auto encrypted = services.packetProtector.protect(frame.rawNeuralSignals, services.neuralTransportSecret.value);
    const auto decrypted = services.packetProtector.unprotect(encrypted, services.neuralTransportSecret.value);
    const auto uplinked = services.bridge.uplink(decrypted.empty() ? frame.rawNeuralSignals : decrypted);
    frame.cleanSignals = services.dsp.denoise(uplinked);
    frame.features = services.vectorizer.vectorize(frame.cleanSignals);
    NeuralArtifactClassifier artifactClassifier;
    NeuralSignalQualityGate qualityGate;
    frame.artifactReport = artifactClassifier.classify(frame.cleanSignals);
    frame.repeatedArtifactCount = frame.artifactReport.artifactProbability > 0.7f ? frame.repeatedArtifactCount + 1 : 0;
    const float signalQuality = frame.cleanSignals.empty() ? 0.0f : std::clamp(1.0f - frame.artifactReport.artifactProbability, 0.0f, 1.0f);
    frame.signalGateDecision = qualityGate.evaluate(signalQuality, frame.artifactReport.signalDropout > 0.5f, frame.artifactReport.artifactProbability, frame.repeatedArtifactCount);
    services.watchdog.heartbeat("signal_processing");
    if (frame.signalGateDecision.action == NeuralSignalGateAction::BlockStimulus) {
        frame.fullDiveNeuralFrame = {};
        frame.signalStatus = RuntimeStageStatus::emergency(frame.signalGateDecision.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = frame.signalGateDecision.reason;
        return frame;
    }
    if (frame.signalGateDecision.action == NeuralSignalGateAction::BlockIntent) {
        frame.features.clear();
    }
    if (frame.signalGateDecision.action == NeuralSignalGateAction::RealityAnchor ||
        frame.signalGateDecision.action == NeuralSignalGateAction::HoldLastSafeFrame ||
        frame.signalGateDecision.action == NeuralSignalGateAction::Recalibration) {
        frame.fullDiveNeuralFrame.realityAnchor.anchorRequired = true;
    }

    frame.signalStatus = validate(!frame.features.empty(), frame.signalGateDecision.intentAllowed ? "empty_feature_vector" : frame.signalGateDecision.reason);
    frame.continueCycle = frame.signalStatus.continueCycle;
    if (!frame.continueCycle) {
        frame.stopReason = frame.signalStatus.reason;
    }
    return frame;
}

} // namespace dnxr
