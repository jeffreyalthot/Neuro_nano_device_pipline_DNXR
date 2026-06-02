#include "../src/runtime/RuntimePipelineRunner.h"

#include <cassert>
#include <mutex>
#include <string>
#include <vector>

namespace {

dnxr::RuntimePipelineRunner::RunInput makeNominalInput(
    dnxr::SchedulerTickReport& tickReport,
    dnxr::NeuroRealtimeScheduler& scheduler,
    dnxr::RuntimeMetrics& metrics,
    dnxr::RuntimeTraceRecorder& traceRecorder,
    std::mutex& metricsMutex,
    std::mutex& traceMutex,
    std::uint64_t& publishedStates,
    std::uint64_t& replayedFrames,
    std::uint64_t& renderInputsBuilt,
    std::uint64_t& finalizedCycles,
    std::atomic<std::uint64_t>& cycleCounter,
    std::atomic<std::uint64_t>& droppedFrames) {
    return dnxr::RuntimePipelineRunner::RunInput{
        dnxr::RuntimeStageContext{cycleCounter.load(), 42.0, 0.011},
        dnxr::RuntimePipelineRunner::StageInputs{
            dnxr::RuntimeAcquisitionStage::Input{
                [] { return std::vector<float>{72.0f, 0.1f, 0.8f, 0.01f, 0.98f, 0.95f}; },
                [](const std::vector<float>&) { return true; },
                [] {},
                [] {},
                [] { return std::vector<float>{0.1f, 0.2f, 0.3f}; },
                [] { return dnxr::NeuralChipFrame{}; }},
            dnxr::RuntimeSignalStage::Input{
                [](const std::vector<float>& raw) { return std::vector<std::uint8_t>(raw.size(), 1U); },
                [](const std::vector<std::uint8_t>& encrypted) { return std::vector<float>(encrypted.size(), 0.25f); },
                [](const std::vector<float>& values) { return values; },
                [](const std::vector<float>& values) { return values; },
                [](const std::vector<float>& values) { return values; },
                [] {}},
            dnxr::RuntimeInferenceStage::Input{
                [](const std::vector<float>&, double) { return std::string{"focused"}; },
                [](const std::vector<float>& telemetry) {
                    dnxr::BiometricFrame bio;
                    bio.heartRate = telemetry.empty() ? 72.0f : telemetry.front();
                    bio.postureStability = 0.9f;
                    return bio;
                },
                [](const std::vector<float>&, const dnxr::BiometricFrame&) {
                    dnxr::CognitiveState cognitive;
                    cognitive.attention = 0.8f;
                    cognitive.immersionIndex = 0.5f;
                    cognitive.stress = 0.1f;
                    cognitive.fatigue = 0.1f;
                    return cognitive;
                },
                [](const dnxr::CognitiveState&, const dnxr::BiometricFrame&) { return dnxr::XRAdaptationFrame{}; },
                [] {
                    dnxr::NeuroPipelineDiagnostics diagnostics;
                    diagnostics.neuralSignalQuality = 0.95f;
                    diagnostics.intentConfidence = 0.90f;
                    diagnostics.autonomyPreservation = 1.0f;
                    diagnostics.decisionDrift = 0.01f;
                    return diagnostics;
                },
                [] {}},
            dnxr::RuntimeCalibrationStage::Input{
                [](const std::vector<float>& features, float, double) { return features; },
                {},
                {},
                [](const std::vector<float>&, float, float, float, double) { return dnxr::CalibrationDriftAssessment{}; },
                {},
                {},
                {}},
            dnxr::RuntimeImmersionFrameStage::Input{
                [](const std::vector<float>&, const dnxr::CognitiveState&) { return dnxr::XRFullBodyCommand{}; },
                [](const dnxr::CognitiveState&, const dnxr::XRFullBodyCommand&, const dnxr::XRAdaptationFrame&, const dnxr::NeuroPipelineDiagnostics&) {
                    dnxr::NeuralSensoryFrame frame;
                    frame.visualPrimary = 0.4f;
                    frame.audioSpatial = 0.3f;
                    return frame;
                },
                [](const std::vector<float>& frame, std::uint64_t, double, double) {
                    dnxr::MultisensoryAlignmentReport report;
                    report.alignedFrame = frame;
                    return report;
                },
                {},
                {}},
            dnxr::RuntimeSafetyStage::Input{
                [](const dnxr::SafetyGovernorInput&) {
                    dnxr::SafetyDecision decision;
                    decision.action = dnxr::SafetyAction::Continue;
                    decision.allowRender = true;
                    decision.intensityScale = 1.0f;
                    return decision;
                },
                {},
                {},
                [](const dnxr::RealityAnchorContext&, const dnxr::SafetyDecision&) {
                    dnxr::RealityAnchorPlan plan;
                    plan.mode = dnxr::RealityAnchorMode::Maintain;
                    return plan;
                },
                {},
                [](const std::vector<float>& frame, const dnxr::SafetyDecision&) { return frame; },
                [](const std::vector<float>& frame, const dnxr::RealityAnchorPlan&) { return frame; },
                [](const std::vector<float>& frame, float) { return frame; }},
            dnxr::RuntimeFullDiveStage::Input{
                [](const dnxr::FullDiveRuntimeInput&) {},
                [] { return dnxr::FullDiveRuntimeSnapshot{}; },
                [](const dnxr::FullDiveRuntimeSnapshot&, const dnxr::SafetyDecision& decision) { return decision; },
                {},
                {},
                {},
                {},
                dnxr::RuntimeMetricsSnapshot{}},
            dnxr::RuntimeStimulusStage::Input{
                [](std::uint64_t, const std::vector<float>&, const dnxr::SafetyDecision&, double) {
                    return dnxr::SafeNeuralStimulusCommand{};
                },
                [] {},
                [](const dnxr::SafeNeuralStimulusCommand&) { return true; },
                {},
                [](const std::vector<float>& frame) { return frame; },
                [](const std::vector<float>&) {}},
            dnxr::RuntimeCycleFinalizer::Input{
                scheduler,
                metrics,
                traceRecorder,
                metricsMutex,
                traceMutex,
                cycleCounter,
                droppedFrames,
                [&](const dnxr::RuntimeMetricsSnapshot&) { ++finalizedCycles; },
                {}}},
        tickReport,
        [&](dnxr::SchedulerTickReport&) {
            ++renderInputsBuilt;
            return dnxr::RuntimeRenderStage::Input{
                [](const std::vector<float>&) {},
                [] { return true; },
                {},
                [] {},
                {},
                {},
                {},
                [](const std::vector<float>&) {},
                {},
                {},
                1.0};
        },
        [&](const dnxr::RuntimePipelineFrame&) { ++publishedStates; },
        [&](const std::vector<float>&, double) { ++replayedFrames; },
        {},
        {},
        {},
        {},
        [&] { ++droppedFrames; },
        {}};
}

} // namespace

int main() {
    dnxr::RuntimePipelineRunner runner;
    dnxr::NeuroRealtimeScheduler scheduler(100.0);
    scheduler.registerTask({dnxr::RuntimeAcquisitionStage::name(), 100, 10.0, false});
    scheduler.registerTask({dnxr::RuntimeSignalStage::name(), 90, 10.0, false});
    scheduler.registerTask({dnxr::RuntimeInferenceStage::name(), 80, 10.0, true});
    scheduler.registerTask({dnxr::RuntimeRenderStage::name(), 70, 10.0, true});

    dnxr::RuntimeMetrics metrics(20.0, 16U);
    dnxr::RuntimeTraceRecorder traceRecorder;
    std::mutex metricsMutex;
    std::mutex traceMutex;
    std::atomic<std::uint64_t> cycleCounter{0U};
    std::atomic<std::uint64_t> droppedFrames{0U};
    std::uint64_t publishedStates = 0U;
    std::uint64_t replayedFrames = 0U;
    std::uint64_t renderInputsBuilt = 0U;
    std::uint64_t finalizedCycles = 0U;

    dnxr::SchedulerTickReport tickReport = scheduler.beginTick();
    auto input = makeNominalInput(
        tickReport,
        scheduler,
        metrics,
        traceRecorder,
        metricsMutex,
        traceMutex,
        publishedStates,
        replayedFrames,
        renderInputsBuilt,
        finalizedCycles,
        cycleCounter,
        droppedFrames);

    const dnxr::RuntimePipelineFrame frame = runner.run(input);
    assert(frame.continueCycle);
    assert(frame.stopReason == "nominal_runtime");
    assert(frame.sensoryFrame.size() == 11U);
    assert(publishedStates == 1U);
    assert(replayedFrames == 1U);
    assert(renderInputsBuilt == 1U);
    assert(finalizedCycles == 1U);
    assert(cycleCounter == 1U);
    assert(droppedFrames == 0U);

    return 0;
}
