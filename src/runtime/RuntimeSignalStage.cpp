#include "RuntimeSignalStage.h"

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
    if (input.heartbeatSignalProcessing) {
        input.heartbeatSignalProcessing();
    }
    frame.signalStatus = validate(!frame.features.empty(), "empty_feature_vector");
    frame.continueCycle = frame.signalStatus.continueCycle;
    if (!frame.continueCycle) {
        frame.stopReason = frame.signalStatus.reason;
    }
    return frame;
}

} // namespace dnxr
