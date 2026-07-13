#include "../src/runtime/IBciAcquisitionSource.h"

#include <cassert>
#include <cstdint>
#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace {

dnxr::BciAcquisitionFrame frame(std::uint64_t sequence,
                                std::int64_t hardwareNs,
                                const std::string& session = "session-a",
                                std::uint64_t topology = 77U) {
    dnxr::BciAcquisitionFrame f;
    f.deviceIdentity = "recorded_nonhuman_device_fixture";
    f.sessionId = session;
    f.sequence = sequence;
    f.hardwareTimestampNs = hardwareNs;
    f.hostTimestampNs = hardwareNs + 1000;
    f.clockDomain = dnxr::BciAcquisitionClockDomain::RecordedNonHumanFixture;
    f.sampleLayout.channelCount = 3U;
    f.sampleLayout.samplesPerChannel = 1U;
    f.sampleLayout.stride = 1U;
    f.channelTopologyDigest = topology;
    f.external = true;
    f.attested = true;
    f.authenticated = true;
    f.recordedNonHumanFixture = true;
    f.samples = {0.1f, 0.2f, 0.3f};
    f.packetDigest = dnxr::bciAcquisitionDigest(f);
    f.authenticationTag = f.packetDigest ^ 0xA77E57EDULL;
    return f;
}

class RecordedBciAcquisitionAdapter final : public dnxr::IBciAcquisitionSource {
public:
    explicit RecordedBciAcquisitionAdapter(std::vector<dnxr::BciAcquisitionFrame> frames)
        : frames_(std::move(frames)) {}

    dnxr::BciAcquisitionReadResult readBciAcquisition(const dnxr::RuntimeStageContext&) noexcept override {
        if (cursor_ >= frames_.size()) {
            return {{}, false, true, "recorded_bci_fixture_exhausted"};
        }
        return validator_.validate(frames_[cursor_++]);
    }

private:
    std::vector<dnxr::BciAcquisitionFrame> frames_;
    std::size_t cursor_ = 0U;
    dnxr::BciAcquisitionValidator validator_{};
};

void expectFailure(dnxr::BciAcquisitionFrame bad, const std::string& reason) {
    RecordedBciAcquisitionAdapter adapter({frame(1, 1000000000LL), bad});
    dnxr::RuntimeStageContext context{1, 1.0, 0.01};
    assert(adapter.readBciAcquisition(context).valid);
    auto out = adapter.readBciAcquisition(context);
    assert(!out.valid);
    assert(out.hold);
    assert(out.failureReason == reason);
}
} // namespace

int main() {
    {
        RecordedBciAcquisitionAdapter adapter({frame(1, 1000000000LL), frame(2, 1010000000LL)});
        dnxr::RuntimeStageContext context{1, 1.0, 0.01};
        assert(adapter.readBciAcquisition(context).valid);
        assert(adapter.readBciAcquisition(context).valid);
    }

    expectFailure(frame(1, 1010000000LL), "bci_acquisition_duplicate_sequence");
    expectFailure(frame(0, 1010000000LL), "bci_acquisition_replay_sequence");
    expectFailure(frame(3, 1010000000LL), "bci_acquisition_sequence_gap");

    auto dropout = frame(2, 1010000000LL);
    dropout.dropoutCount = 1U;
    dropout.packetDigest = dnxr::bciAcquisitionDigest(dropout);
    expectFailure(dropout, "bci_acquisition_dropout_detected");

    auto corrupt = frame(2, 1010000000LL);
    corrupt.packetDigest ^= 99U;
    expectFailure(corrupt, "bci_acquisition_packet_digest_mismatch");

    auto drift = frame(2, 1010000000LL);
    drift.hostTimestampNs = drift.hardwareTimestampNs + 300000000LL;
    drift.packetDigest = dnxr::bciAcquisitionDigest(drift);
    expectFailure(drift, "bci_acquisition_clock_skew_exceeded");

    expectFailure(frame(2, 1010000000LL, "session-b"), "bci_acquisition_session_mismatch");
    expectFailure(frame(2, 1010000000LL, "session-a", 88U), "bci_acquisition_topology_mismatch");

    auto badLength = frame(2, 1010000000LL);
    badLength.samples.pop_back();
    badLength.packetDigest = dnxr::bciAcquisitionDigest(badLength);
    expectFailure(badLength, "bci_acquisition_sample_length_mismatch");

    std::atomic<unsigned> okThreads{0U};
    auto worker = [&okThreads](std::uint64_t base) {
        RecordedBciAcquisitionAdapter adapter({frame(base, static_cast<std::int64_t>(base) * 1000000000LL),
                                               frame(base + 1U, static_cast<std::int64_t>(base) * 1000000000LL + 10000000LL)});
        dnxr::RuntimeStageContext context{base, static_cast<double>(base), 0.01};
        if (adapter.readBciAcquisition(context).valid && adapter.readBciAcquisition(context).valid) {
            ++okThreads;
        }
    };
    std::thread t1(worker, 10U);
    std::thread t2(worker, 20U);
    t1.join();
    t2.join();
    assert(okThreads.load() == 2U);

    dnxr::BciAcquisitionRingBuffer<2> ring;
    assert(ring.push(frame(1, 1000000000LL)));
    assert(ring.push(frame(2, 1010000000LL)));
    assert(!ring.push(frame(3, 1020000000LL)));
    assert(ring.droppedCount() == 1U);
    dnxr::BciAcquisitionFrame popped;
    assert(ring.pop(popped));
    assert(popped.sequence == 2U);

    return 0;
}
