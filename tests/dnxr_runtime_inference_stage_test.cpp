#include "../src/runtime/RuntimeInferenceStage.h"

#include <cassert>
#include <limits>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.features = {0.1f};
    frame.cleanSignals = {0.2f};
    frame.telemetry = {72.0f};

    auto valid = dnxr::RuntimeInferenceStage::execute(frame, dnxr::RuntimeInferenceStage::Input{
        [](const std::vector<float>&, double) { return std::string{"focused"}; },
        [](const std::vector<float>&) { return dnxr::BiometricFrame{}; },
        [](const std::vector<float>&, const dnxr::BiometricFrame&) {
            dnxr::CognitiveState cognitive;
            cognitive.immersionIndex = 0.5f;
            cognitive.stress = 0.1f;
            cognitive.fatigue = 0.2f;
            return cognitive;
        },
        [](const dnxr::CognitiveState&, const dnxr::BiometricFrame&) { return dnxr::XRAdaptationFrame{}; },
        []() {
            dnxr::NeuroPipelineDiagnostics diagnostics;
            diagnostics.neuralSignalQuality = 0.9f;
            diagnostics.intentConfidence = 0.8f;
            diagnostics.decisionDrift = 0.1f;
            diagnostics.autonomyPreservation = 1.0f;
            return diagnostics;
        }
    });
    assert(valid.continueCycle);
    assert(valid.inferenceStatus.reason == "inference_ok");

    auto invalid = dnxr::RuntimeInferenceStage::execute(frame, dnxr::RuntimeInferenceStage::Input{
        [](const std::vector<float>&, double) { return std::string{"focused"}; },
        [](const std::vector<float>&) { return dnxr::BiometricFrame{}; },
        [](const std::vector<float>&, const dnxr::BiometricFrame&) {
            dnxr::CognitiveState cognitive;
            cognitive.immersionIndex = std::numeric_limits<float>::quiet_NaN();
            return cognitive;
        },
        [](const dnxr::CognitiveState&, const dnxr::BiometricFrame&) { return dnxr::XRAdaptationFrame{}; },
        []() { return dnxr::NeuroPipelineDiagnostics{}; }
    });
    assert(!invalid.continueCycle);
    assert(invalid.stopReason == "invalid_inference_output");

    auto missing = dnxr::RuntimeInferenceStage::execute(frame, dnxr::RuntimeInferenceStage::Input{});
    assert(!missing.continueCycle);
    assert(missing.stopReason == "inference_stage_unavailable");
    return 0;
}
