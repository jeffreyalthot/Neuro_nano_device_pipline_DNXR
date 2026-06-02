#include "../src/runtime/RuntimeImmersionFrameStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.cycleId = 99;
    frame.cleanSignals = {0.1f};
    frame.calibratedFeatures = {0.5f};

    bool traceRecorded = false;
    auto out = dnxr::RuntimeImmersionFrameStage::execute(frame, dnxr::RuntimeImmersionFrameStage::Input{
        [](const std::vector<float>&, const dnxr::CognitiveState&) { return dnxr::XRFullBodyCommand{}; },
        [](const dnxr::CognitiveState&, const dnxr::XRFullBodyCommand&, const dnxr::XRAdaptationFrame&, const dnxr::NeuroPipelineDiagnostics&) {
            dnxr::NeuralSensoryFrame sensory;
            sensory.visualPrimary = 1.0f;
            return sensory;
        },
        [](const std::vector<float>& sensoryFrame, std::uint64_t, double, double) {
            dnxr::MultisensoryAlignmentReport report;
            report.alignedFrame = sensoryFrame;
            for (float& channel : report.alignedFrame) channel *= 0.6f;
            report.degradationApplied = true;
            report.maxObservedPhaseErrorMs = 8.0;
            report.recommendedIntensityScale = 0.6f;
            report.reason = "simulated_phase_jitter";
            return report;
        },
        [&traceRecorded](std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&) {
            traceRecorded = true;
        }
    });

    assert(out.continueCycle);
    assert(out.sensorySync.degradationApplied);
    assert(out.sensorySync.maxObservedPhaseErrorMs == 8.0);
    assert(traceRecorded);
    assert(out.sensoryFrame[0] < 0.7f);
    return 0;
}
