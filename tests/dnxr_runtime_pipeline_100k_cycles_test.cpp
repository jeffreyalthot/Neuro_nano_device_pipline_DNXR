#include "../src/runtime/RuntimeCalibrationStage.h"
#include "../src/runtime/RuntimeImmersionFrameStage.h"
#include "../src/runtime/RuntimeSafetyStage.h"

#include <cassert>

int main() {
    for (std::uint64_t i = 0; i < 100000; ++i) {
        dnxr::RuntimePipelineFrame frame;
        frame.cycleId = i;
        frame.timestamp = static_cast<double>(i) * 0.011;
        frame.features = {0.2f, 0.4f, 0.6f};
        frame.cleanSignals = {0.1f, 0.2f, 0.3f};
        frame.normalizedTelemetryRisk = {0.05f};
        frame.symbolicState = "simulated_focus";
        frame.diagnostics.neuralSignalQuality = 0.95f;
        frame.diagnostics.intentConfidence = 0.90f;
        frame.diagnostics.autonomyPreservation = 1.0f;
        frame.biometric.postureStability = 0.9f;
        frame.cognitive.attention = 0.8f;
        frame.cognitive.immersionIndex = 0.4f;

        frame = dnxr::RuntimeCalibrationStage::execute(frame, dnxr::RuntimeCalibrationStage::Input{
            [](const std::vector<float>& features, float sensitivity, double) {
                auto out = features;
                for (float& value : out) value *= sensitivity;
                return out;
            },
            {}, {},
            [](const std::vector<float>&, float, float, float, double) { return dnxr::CalibrationDriftAssessment{}; }
        });
        assert(frame.continueCycle);

        frame = dnxr::RuntimeImmersionFrameStage::execute(frame, dnxr::RuntimeImmersionFrameStage::Input{
            [](const std::vector<float>&, const dnxr::CognitiveState&) { return dnxr::XRFullBodyCommand{}; },
            [](const dnxr::CognitiveState&, const dnxr::XRFullBodyCommand&, const dnxr::XRAdaptationFrame&, const dnxr::NeuroPipelineDiagnostics&) {
                dnxr::NeuralSensoryFrame sensory;
                sensory.visualPrimary = 0.3f;
                sensory.audioSpatial = 0.2f;
                return sensory;
            },
            [](const std::vector<float>& sensoryFrame, std::uint64_t, double, double) {
                dnxr::MultisensoryAlignmentReport report;
                report.alignedFrame = sensoryFrame;
                return report;
            }
        });
        assert(frame.continueCycle);
        assert(frame.sensoryFrame.size() == 11);
    }
    return 0;
}
