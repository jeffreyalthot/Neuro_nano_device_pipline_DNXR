#include "../src/runtime/CanonicalBciRuntimeContract.h"

#include <cassert>
#include <type_traits>

using namespace dnxr::canonical;

namespace {

Digest256 digestWord(std::uint64_t word) {
    Digest256 digest{};
    digest.words[0] = word;
    return digest;
}

TimeEvidence timeEvidence() {
    return TimeEvidence{MonotonicNs{100}, DeadlineNs{MonotonicNs{200}}, AgeNs{DurationNs{5}}, SkewNs{0}, BudgetNs{DurationNs{50}}};
}

struct TransportProbe {
    PhaseResult<SubmitReceipt> submit(PreparedCommit&& commit) noexcept {
        const auto frame = commit.safety().spikePlan.encoded.calibration.motor.inference.signal.acquisition.world.frameId;
        const auto session = commit.safety().spikePlan.encoded.calibration.motor.inference.signal.acquisition.world.session.sessionId;
        return PhaseResult<SubmitReceipt>::success(SubmitReceipt{session, frame, commit.commitId(), commit.packetNonce(), commit.transactionDigest()});
    }
};

} // namespace

static_assert(!CanSubmitPayload<TransportProbe, EncodedModalities&&>::value,
              "transport must not submit encoded modalities directly");
static_assert(CanSubmitPayload<TransportProbe, PreparedCommit&&>::value,
              "transport must submit only a prepared commit token");
static_assert(!std::is_copy_constructible<PreparedCommit>::value,
              "commit token must be move-only so it cannot be reused after submit");
static_assert(!std::is_assignable<SessionContext&, SessionContext>::value,
              "session context must be immutable once opened");
static_assert(!std::is_convertible<std::uint64_t, DurationNs>::value,
              "duration units must reject implicit integer conversion");

int main() {
    const SessionContext session{SessionId{7},
                                 ConfigurationDigest{digestWord(1)},
                                 PatientMapDigest{digestWord(2)},
                                 TopologyDigest{digestWord(3)},
                                 CapabilityDigest{digestWord(4)},
                                 SafetyPolicyDigest{digestWord(5)},
                                 MonotonicNs{10}};
    const WorldIngressFrame world{session, FrameId{11}, WorldSequence{12}, timeEvidence(), TransactionDigest{digestWord(13)}};
    const BciAcquisitionFrame acquisition{world, AcquisitionSequence{14}, timeEvidence(), TransactionDigest{digestWord(15)}};
    const ConditionedSignal conditioned{acquisition, TransactionDigest{digestWord(16)}};
    const InferenceResult inference{conditioned, TransactionDigest{digestWord(17)}};
    const MotorIntent motor{inference, TransactionDigest{digestWord(18)}};
    const CalibrationSnapshotRef calibration{motor, PatientMapDigest{digestWord(19)}, CapabilityDigest{digestWord(20)}};
    const EncodedModalities encoded{calibration, TransactionDigest{digestWord(21)}};
    const AlignedSpikePlan spikePlan{encoded, TopologyDigest{digestWord(22)}, TransactionDigest{digestWord(23)}};
    const SafetyDecision safety{spikePlan, SafetyPolicyDigest{digestWord(24)}, true};

    auto prepared = prepareCommit(safety, CommitId{25}, PacketNonce{26}, TransactionDigest{digestWord(27)});
    assert(prepared.ok());

    TransportProbe transport;
    auto submitted = transport.submit(prepared.take());
    assert(submitted.ok());
    assert(submitted.value().commitId.value == 25);

    const ExternalAck ack{submitted.value(), MonotonicNs{300}, TransactionDigest{digestWord(28)}};
    const auto correlated = correlateAck(submitted.value(), ack);
    assert(correlated.ok());

    const SubmitReceipt wrongReceipt{SessionId{7}, FrameId{11}, CommitId{999}, PacketNonce{26}, TransactionDigest{digestWord(27)}};
    const auto rejectedAck = correlateAck(wrongReceipt, ack);
    assert(!rejectedAck.ok());
    assert(rejectedAck.error().code == RuntimeErrorCode::CommitIdMismatch);

    const ExternalFeedback feedback{ack, MonotonicNs{350}, TransactionDigest{digestWord(29)}};
    const CorrectionDecision correction{feedback, TransactionDigest{digestWord(30)}};
    const auto closed = closeWithFeedback(correction, true, TransactionDigest{digestWord(31)});
    assert(closed.ok());

    const ExternalFeedback missingFeedback{ack, MonotonicNs{360}, TransactionDigest{Digest256{}}};
    const CorrectionDecision missingCorrection{missingFeedback, TransactionDigest{digestWord(32)}};
    const auto rejectedClose = closeWithFeedback(missingCorrection, true, TransactionDigest{digestWord(33)});
    assert(!rejectedClose.ok());
    assert(rejectedClose.error().code == RuntimeErrorCode::FeedbackRequired);

    const SafetyDecision denied{spikePlan, SafetyPolicyDigest{digestWord(24)}, false};
    const auto deniedPrepare = prepareCommit(denied, CommitId{25}, PacketNonce{26}, TransactionDigest{digestWord(27)});
    assert(!deniedPrepare.ok());
    assert(deniedPrepare.error().disposition == LifecycleDisposition::Hold);

    return 0;
}
