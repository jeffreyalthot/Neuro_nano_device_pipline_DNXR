#include "../src/runtime/RuntimeCalibrationStage.h"
#include "../src/runtime/RuntimeImmersionFrameStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.features = std::vector<float>(64, 0.2f);
    frame.cleanSignals = std::vector<float>(64, 0.1f);
    frame.diagnostics.neuralSignalQuality = 1.0f;
    frame.diagnostics.intentConfidence = 1.0f;

    std::size_t maxSensoryCapacity = 0;
    for (std::uint64_t i = 0; i < 25000; ++i) {
        frame.cycleId = i;
        frame.timestamp = static_cast<double>(i) * 0.016;
        frame = dnxr::RuntimeCalibrationStage::execute(frame, dnxr::RuntimeCalibrationStage::Input{
            [](const std::vector<float>& features, float, double) { return features; },
            {}, {},
            [](const std::vector<float>&, float, float, float, double) { return dnxr::CalibrationDriftAssessment{}; }
        });
        frame = dnxr::RuntimeImmersionFrameStage::execute(frame, dnxr::RuntimeImmersionFrameStage::Input{
            [](const std::vector<float>&, const dnxr::CognitiveState&) { return dnxr::XRFullBodyCommand{}; },
            [](const dnxr::CognitiveState&, const dnxr::XRFullBodyCommand&, const dnxr::XRAdaptationFrame&, const dnxr::NeuroPipelineDiagnostics&) {
                dnxr::NeuralSensoryFrame sensory;
                sensory.visualPrimary = 0.5f;
                return sensory;
            },
            [](const std::vector<float>& sensoryFrame, std::uint64_t, double, double) {
                dnxr::MultisensoryAlignmentReport report;
                report.alignedFrame = sensoryFrame;
                return report;
            }
        });
        maxSensoryCapacity = std::max(maxSensoryCapacity, frame.sensoryFrame.capacity());
        assert(frame.sensoryFrame.size() == 11);
        assert(maxSensoryCapacity <= 32);
    }
    return 0;
}
