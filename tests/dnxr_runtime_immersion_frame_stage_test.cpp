#include "../src/runtime/RuntimeImmersionFrameStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.cycleId = 3;
    frame.cleanSignals = {0.1f, 0.2f};
    frame.calibratedFeatures = {0.8f};
    frame.cognitive.immersionIndex = 0.5f;

    bool traced = false;
    auto out = dnxr::RuntimeImmersionFrameStage::execute(frame, dnxr::RuntimeImmersionFrameStage::Input{
        [](const std::vector<float>&, const dnxr::CognitiveState&) {
            dnxr::XRFullBodyCommand command;
            command.balanceAssist = 0.25f;
            return command;
        },
        [](const dnxr::CognitiveState&, const dnxr::XRFullBodyCommand&, const dnxr::XRAdaptationFrame&, const dnxr::NeuroPipelineDiagnostics&) {
            dnxr::NeuralSensoryFrame sensory;
            sensory.visualPrimary = 0.2f;
            sensory.audioSpatial = 0.4f;
            return sensory;
        },
        [](const std::vector<float>& sensoryFrame, std::uint64_t, double, double) {
            dnxr::MultisensoryAlignmentReport report;
            report.alignedFrame = sensoryFrame;
            report.degradationApplied = true;
            report.recommendedIntensityScale = 0.75f;
            report.reason = "simulated_jitter";
            return report;
        },
        [&traced](std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&) {
            traced = true;
        }
    });

    assert(out.continueCycle);
    assert(out.bodyCommand.balanceAssist == 0.25f);
    assert(out.sensoryFrame.size() == 11);
    assert(out.sensoryFrame[0] > 0.28f && out.sensoryFrame[0] < 0.30f);
    assert(out.sensorySync.degradationApplied);
    assert(traced);

    auto unavailable = dnxr::RuntimeImmersionFrameStage::execute(frame, dnxr::RuntimeImmersionFrameStage::Input{});
    assert(!unavailable.continueCycle);
    assert(unavailable.stopReason == "immersion_frame_stage_unavailable");
    return 0;
}
