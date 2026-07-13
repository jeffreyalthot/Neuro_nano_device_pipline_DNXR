#include "../src/runtime/RuntimeAcquisitionStage.h"

#include <cassert>
#include <vector>

int main() {
    assert(dnxr::RuntimeAcquisitionStage::validate(true, "empty").continueCycle);
    assert(!dnxr::RuntimeAcquisitionStage::validate(false, "empty").continueCycle);

    int readCount = 0;
    dnxr::RuntimeStageContext context{7, 100.0, 0.02};
    auto frame = dnxr::RuntimeAcquisitionStage::execute(context, dnxr::RuntimeAcquisitionStage::Input{
        [](double ts) {
            dnxr::BiometricSample sample;
            sample.timestampSeconds = ts;
            sample.provenance = "certified_test_monitor";
            sample.quality = 1.0f;
            sample.validity = dnxr::BiometricValidity::Valid;
            sample.channels = {72.0f, 0.2f};
            return sample;
        },
        [](const std::vector<float>& telemetry) { return telemetry.size() == 2U; },
        [] {},
        [] {},
        [&readCount](double) {
            ++readCount;
            dnxr::NeuralSignalReadResult result;
            result.frame.sequenceId = 10;
            result.frame.monotonicTimestampMs = 1000;
            result.frame.signalQuality = 0.9f;
            result.frame.channels = {0.1f, 0.2f, 0.3f};
            result.provenance = "test_hal";
            result.valid = true;
            result.failureReason.clear();
            return result;
        }});
    assert(frame.continueCycle);
    assert(readCount == 1);
    assert(frame.chipFrame.sequenceId == 10);
    assert(frame.acquisitionChipFrame.sequenceId == 10);
    assert(frame.rawNeuralSignals.size() == 3U);

    auto replayed = dnxr::RuntimeAcquisitionStage::execute(context, dnxr::RuntimeAcquisitionStage::Input{
        {},
        {},
        {},
        {},
        [](double) {
            dnxr::NeuralSignalReadResult result;
            result.valid = false;
            result.duplicateSequence = true;
            result.failureReason = "duplicate_or_replayed_neural_frame_sequence";
            return result;
        }});
    assert(!replayed.continueCycle);
    assert(replayed.stopReason == "duplicate_or_replayed_neural_frame_sequence");

    auto noBiometric = dnxr::RuntimeAcquisitionStage::execute(context, dnxr::RuntimeAcquisitionStage::Input{
        {},
        [](const std::vector<float>& telemetry) { return telemetry.empty(); },
        {},
        {},
        [](double) {
            dnxr::NeuralSignalReadResult result;
            result.frame.sequenceId = 11;
            result.frame.monotonicTimestampMs = 1100;
            result.frame.signalQuality = 0.8f;
            result.frame.channels = {0.4f};
            result.valid = true;
            return result;
        }});
    assert(noBiometric.continueCycle);
    assert(noBiometric.telemetry.empty());
    return 0;
}
