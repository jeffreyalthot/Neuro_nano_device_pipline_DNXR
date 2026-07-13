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
        [&readCount](const dnxr::RuntimeStageContext&) {
            ++readCount;
            dnxr::BciAcquisitionReadResult result;
            result.valid = true;
            result.hold = false;
            result.frame.deviceIdentity = "test_external_bci_device";
            result.frame.sessionId = "unit-session";
            result.frame.sequence = 10;
            result.frame.hardwareTimestampNs = 1000000000LL;
            result.frame.hostTimestampNs = 1000001000LL;
            result.frame.clockDomain = dnxr::BciAcquisitionClockDomain::RecordedNonHumanFixture;
            result.frame.sampleLayout.channelCount = 3U;
            result.frame.channelTopologyDigest = 77U;
            result.frame.external = true;
            result.frame.attested = true;
            result.frame.authenticated = true;
            result.frame.recordedNonHumanFixture = true;
            result.frame.samples = {0.1f, 0.2f, 0.3f};
            result.frame.packetDigest = dnxr::bciAcquisitionDigest(result.frame);
            result.frame.authenticationTag = 1U;
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
        [](const dnxr::RuntimeStageContext&) {
            dnxr::BciAcquisitionReadResult result;
            result.valid = false;
            result.hold = true;
            result.failureReason = "bci_acquisition_duplicate_sequence";
            return result;
        }});
    assert(!replayed.continueCycle);
    assert(replayed.stopReason == "bci_acquisition_duplicate_sequence");

    auto noBiometric = dnxr::RuntimeAcquisitionStage::execute(context, dnxr::RuntimeAcquisitionStage::Input{
        {},
        [](const std::vector<float>& telemetry) { return telemetry.empty(); },
        {},
        {},
        [](const dnxr::RuntimeStageContext&) {
            dnxr::BciAcquisitionReadResult result;
            result.valid = true;
            result.hold = false;
            result.frame.deviceIdentity = "test_external_bci_device";
            result.frame.sessionId = "unit-session";
            result.frame.sequence = 11;
            result.frame.hardwareTimestampNs = 1100000000LL;
            result.frame.hostTimestampNs = 1100001000LL;
            result.frame.clockDomain = dnxr::BciAcquisitionClockDomain::RecordedNonHumanFixture;
            result.frame.sampleLayout.channelCount = 1U;
            result.frame.channelTopologyDigest = 77U;
            result.frame.external = true;
            result.frame.attested = true;
            result.frame.authenticated = true;
            result.frame.recordedNonHumanFixture = true;
            result.frame.samples = {0.4f};
            result.frame.packetDigest = dnxr::bciAcquisitionDigest(result.frame);
            result.frame.authenticationTag = 1U;
            return result;
        }});
    assert(noBiometric.continueCycle);
    assert(noBiometric.telemetry.empty());
    return 0;
}
