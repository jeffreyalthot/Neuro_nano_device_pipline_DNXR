#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace dnxr::canonical {

struct MonotonicNs {
    std::uint64_t value;
    explicit constexpr MonotonicNs(std::uint64_t ns = 0) noexcept : value(ns) {}
};

struct DurationNs {
    std::uint64_t value;
    explicit constexpr DurationNs(std::uint64_t ns = 0) noexcept : value(ns) {}
};

struct DeadlineNs {
    MonotonicNs at;
    explicit constexpr DeadlineNs(MonotonicNs value = MonotonicNs{}) noexcept : at(value) {}
};

struct AgeNs {
    DurationNs value;
    explicit constexpr AgeNs(DurationNs duration = DurationNs{}) noexcept : value(duration) {}
};

struct SkewNs {
    std::int64_t value;
    explicit constexpr SkewNs(std::int64_t ns = 0) noexcept : value(ns) {}
};

struct BudgetNs {
    DurationNs value;
    explicit constexpr BudgetNs(DurationNs duration = DurationNs{}) noexcept : value(duration) {}
};

struct SessionId { std::uint64_t value; explicit constexpr SessionId(std::uint64_t v = 0) noexcept : value(v) {} };
struct FrameId { std::uint64_t value; explicit constexpr FrameId(std::uint64_t v = 0) noexcept : value(v) {} };
struct WorldSequence { std::uint64_t value; explicit constexpr WorldSequence(std::uint64_t v = 0) noexcept : value(v) {} };
struct AcquisitionSequence { std::uint64_t value; explicit constexpr AcquisitionSequence(std::uint64_t v = 0) noexcept : value(v) {} };
struct CommitId { std::uint64_t value; explicit constexpr CommitId(std::uint64_t v = 0) noexcept : value(v) {} };
struct PacketNonce { std::uint64_t value; explicit constexpr PacketNonce(std::uint64_t v = 0) noexcept : value(v) {} };

struct Digest256 {
    std::uint64_t words[4]{};
    constexpr bool present() const noexcept { return words[0] != 0 || words[1] != 0 || words[2] != 0 || words[3] != 0; }
};

struct ConfigurationDigest { Digest256 value; };
struct PatientMapDigest { Digest256 value; };
struct TopologyDigest { Digest256 value; };
struct CapabilityDigest { Digest256 value; };
struct SafetyPolicyDigest { Digest256 value; };
struct TransactionDigest { Digest256 value; };

struct TimeEvidence {
    const MonotonicNs sourceTimestamp;
    const DeadlineNs deadline;
    const AgeNs age;
    const SkewNs skew;
    const BudgetNs budget;
};

class IMonotonicClock {
public:
    virtual ~IMonotonicClock() = default;
    virtual MonotonicNs now() noexcept = 0;
};

enum class RuntimeErrorCode {
    None,
    InvalidPhase,
    StaleFrame,
    DigestMismatch,
    PreconditionMissing,
    CommitIdMismatch,
    FeedbackRequired,
    RetryForbidden,
    ExternalDependencyUnavailable,
    TransportRejected,
    SafetyHold,
    DecompressionRequired,
    EmergencyStop,
    ShutdownRequired
};

enum class LifecycleDisposition {
    Continue,
    RetryAllowed,
    RetryForbidden,
    Hold,
    Decompression,
    Emergency,
    Shutdown,
    ExternalDependencyUnavailable
};

struct RuntimeError {
    RuntimeErrorCode code = RuntimeErrorCode::None;
    LifecycleDisposition disposition = LifecycleDisposition::Continue;
    const char* phase = "none";
    const char* reason = "ok";
};

template <typename T>
class PhaseResult {
public:
    static PhaseResult success(T value) noexcept { return PhaseResult(std::move(value)); }
    static PhaseResult failure(RuntimeError error) noexcept { return PhaseResult(error); }

    bool ok() const noexcept { return ok_; }
    const T& value() const noexcept { return *value_; }
    T&& take() noexcept { return std::move(*value_); }
    RuntimeError error() const noexcept { return error_; }

private:
    explicit PhaseResult(T value) noexcept : ok_(true), value_(std::move(value)) {}
    explicit PhaseResult(RuntimeError error) noexcept : ok_(false), error_(error) {}

    bool ok_ = false;
    std::optional<T> value_{};
    RuntimeError error_{};
};

struct SessionContext {
    const SessionId sessionId;
    const ConfigurationDigest configurationDigest;
    const PatientMapDigest patientMapDigest;
    const TopologyDigest topologyDigest;
    const CapabilityDigest capabilityDigest;
    const SafetyPolicyDigest safetyPolicyDigest;
    const MonotonicNs openedAt;
};

struct WorldIngressFrame {
    const SessionContext session;
    const FrameId frameId;
    const WorldSequence worldSequence;
    const TimeEvidence time;
    const TransactionDigest provenanceDigest;
};

struct BciAcquisitionFrame {
    const WorldIngressFrame world;
    const AcquisitionSequence acquisitionSequence;
    const TimeEvidence acquisitionTime;
    const TransactionDigest acquisitionDigest;
};

struct ConditionedSignal { const BciAcquisitionFrame acquisition; const TransactionDigest signalDigest; };
struct InferenceResult { const ConditionedSignal signal; const TransactionDigest inferenceDigest; };
struct MotorIntent { const InferenceResult inference; const TransactionDigest motorDigest; };
struct CalibrationSnapshotRef { const MotorIntent motor; const PatientMapDigest patientMapDigest; const CapabilityDigest capabilityDigest; };
struct EncodedModalities { const CalibrationSnapshotRef calibration; const TransactionDigest modalityDigest; };
struct AlignedSpikePlan { const EncodedModalities encoded; const TopologyDigest topologyDigest; const TransactionDigest spikeDigest; };
struct SafetyDecision { const AlignedSpikePlan spikePlan; const SafetyPolicyDigest safetyPolicyDigest; const bool allowDispatch; };

class PreparedCommit {
public:
    PreparedCommit(const SafetyDecision& safety, CommitId commitId, PacketNonce packetNonce, TransactionDigest digest) noexcept
        : safety_(safety), commitId_(commitId), packetNonce_(packetNonce), transactionDigest_(digest) {}
    PreparedCommit(const PreparedCommit&) = delete;
    PreparedCommit& operator=(const PreparedCommit&) = delete;
    PreparedCommit(PreparedCommit&&) noexcept = default;
    PreparedCommit& operator=(PreparedCommit&&) noexcept = delete;

    const SafetyDecision& safety() const noexcept { return safety_; }
    CommitId commitId() const noexcept { return commitId_; }
    PacketNonce packetNonce() const noexcept { return packetNonce_; }
    TransactionDigest transactionDigest() const noexcept { return transactionDigest_; }

private:
    const SafetyDecision& safety_;
    CommitId commitId_;
    PacketNonce packetNonce_;
    TransactionDigest transactionDigest_;
};

struct SubmitReceipt {
    const SessionId sessionId;
    const FrameId frameId;
    const CommitId commitId;
    const PacketNonce packetNonce;
    const TransactionDigest transactionDigest;
};

struct ExternalAck {
    const SubmitReceipt submitted;
    const MonotonicNs receivedAt;
    const TransactionDigest ackDigest;
};

struct ExternalFeedback {
    const ExternalAck ack;
    const MonotonicNs receivedAt;
    const TransactionDigest feedbackDigest;
};

struct CorrectionDecision { const ExternalFeedback feedback; const TransactionDigest correctionDigest; };
struct ClosedTransaction { const CorrectionDecision correction; const TransactionDigest transactionDigest; };

class IWorldIngress { public: virtual ~IWorldIngress() = default; virtual PhaseResult<WorldIngressFrame> readWorld(const SessionContext&, FrameId) noexcept = 0; };
class IBciAcquisition { public: virtual ~IBciAcquisition() = default; virtual PhaseResult<BciAcquisitionFrame> acquire(const WorldIngressFrame&) noexcept = 0; };
class ISensoryEncoding { public: virtual ~ISensoryEncoding() = default; virtual PhaseResult<EncodedModalities> encode(const CalibrationSnapshotRef&) noexcept = 0; };
class ISpikeMapping { public: virtual ~ISpikeMapping() = default; virtual PhaseResult<AlignedSpikePlan> mapSpikes(const EncodedModalities&) noexcept = 0; };
class ISafetyAuthority { public: virtual ~ISafetyAuthority() = default; virtual PhaseResult<SafetyDecision> evaluate(const AlignedSpikePlan&) noexcept = 0; };
class INeuralChipTransport { public: virtual ~INeuralChipTransport() = default; virtual PhaseResult<SubmitReceipt> submit(PreparedCommit&&) noexcept = 0; };
class IFeedbackReader { public: virtual ~IFeedbackReader() = default; virtual PhaseResult<ExternalAck> readAck(const SubmitReceipt&) noexcept = 0; virtual PhaseResult<ExternalFeedback> readFeedback(const ExternalAck&) noexcept = 0; };
class IMotorWorldEgress { public: virtual ~IMotorWorldEgress() = default; virtual PhaseResult<MotorIntent> publishMotorIntent(const InferenceResult&) noexcept = 0; };
class IAuditSink { public: virtual ~IAuditSink() = default; virtual void recordClosedTransaction(const ClosedTransaction&) noexcept = 0; virtual void recordError(RuntimeError) noexcept = 0; };
class IObserverSnapshotSink { public: virtual ~IObserverSnapshotSink() = default; virtual void publishReadOnlySnapshot(const ClosedTransaction&) noexcept = 0; };

struct RuntimeDependencies {
    IMonotonicClock& clock;
    IWorldIngress& worldIngress;
    IBciAcquisition& bciAcquisition;
    ISensoryEncoding& sensoryEncoding;
    ISpikeMapping& spikeMapping;
    ISafetyAuthority& safetyAuthority;
    INeuralChipTransport& transport;
    IFeedbackReader& feedbackReader;
    IMotorWorldEgress& motorWorldEgress;
    IAuditSink& auditSink;
    IObserverSnapshotSink* observerSnapshotSink = nullptr;
};

struct OpenSessionRequest {
    ConfigurationDigest configurationDigest;
    PatientMapDigest patientMapDigest;
    TopologyDigest topologyDigest;
    CapabilityDigest capabilityDigest;
    SafetyPolicyDigest safetyPolicyDigest;
};

struct StepRequest { FrameId frameId; bool feedbackRequired = true; };
struct StepResult { PhaseResult<ClosedTransaction> closed; };

class CanonicalNeuralBciRuntime {
public:
    virtual ~CanonicalNeuralBciRuntime() = default;
    virtual PhaseResult<SessionContext> openSession(const OpenSessionRequest&) noexcept = 0;
    virtual StepResult step(const StepRequest&) noexcept = 0;
    virtual RuntimeError requestHold(const char* reason) noexcept = 0;
    virtual RuntimeError requestEmergencyStop(const char* reason) noexcept = 0;
    virtual RuntimeError closeSession() noexcept = 0;
};

inline bool requiredArtifactDigestsPresent(const SafetyDecision& safety) noexcept {
    const auto& session = safety.spikePlan.encoded.calibration.motor.inference.signal.acquisition.world.session;
    return session.patientMapDigest.value.present() &&
           session.topologyDigest.value.present() &&
           session.capabilityDigest.value.present() &&
           safety.spikePlan.encoded.calibration.patientMapDigest.value.present() &&
           safety.spikePlan.encoded.calibration.capabilityDigest.value.present() &&
           safety.spikePlan.topologyDigest.value.present();
}

inline PhaseResult<PreparedCommit> prepareCommit(const SafetyDecision& safety,
                                                 CommitId commitId,
                                                 PacketNonce packetNonce,
                                                 TransactionDigest digest) noexcept {
    if (!safety.allowDispatch) {
        return PhaseResult<PreparedCommit>::failure({RuntimeErrorCode::PreconditionMissing,
                                                     LifecycleDisposition::Hold,
                                                     "PreparedCommit",
                                                     "safety_denied_dispatch"});
    }
    if (!requiredArtifactDigestsPresent(safety)) {
        return PhaseResult<PreparedCommit>::failure({RuntimeErrorCode::PreconditionMissing,
                                                     LifecycleDisposition::Hold,
                                                     "PreparedCommit",
                                                     "missing_required_artifact_digest"});
    }
    if (commitId.value == 0 || packetNonce.value == 0 || !digest.value.present()) {
        return PhaseResult<PreparedCommit>::failure({RuntimeErrorCode::PreconditionMissing,
                                                     LifecycleDisposition::Hold,
                                                     "PreparedCommit",
                                                     "missing_commit_identity"});
    }
    return PhaseResult<PreparedCommit>::success(PreparedCommit{safety, commitId, packetNonce, digest});
}

inline PhaseResult<ExternalAck> correlateAck(const SubmitReceipt& receipt, const ExternalAck& ack) noexcept {
    if (receipt.sessionId.value != ack.submitted.sessionId.value ||
        receipt.frameId.value != ack.submitted.frameId.value ||
        receipt.commitId.value != ack.submitted.commitId.value ||
        receipt.packetNonce.value != ack.submitted.packetNonce.value) {
        return PhaseResult<ExternalAck>::failure({RuntimeErrorCode::CommitIdMismatch,
                                                  LifecycleDisposition::Emergency,
                                                  "ExternalAck",
                                                  "ack_correlation_mismatch"});
    }
    return PhaseResult<ExternalAck>::success(ack);
}

inline PhaseResult<ClosedTransaction> closeWithFeedback(const CorrectionDecision& correction,
                                                        bool feedbackRequired,
                                                        TransactionDigest digest) noexcept {
    if (feedbackRequired && !correction.feedback.feedbackDigest.value.present()) {
        return PhaseResult<ClosedTransaction>::failure({RuntimeErrorCode::FeedbackRequired,
                                                        LifecycleDisposition::Hold,
                                                        "ClosedTransaction",
                                                        "external_feedback_required"});
    }
    if (!digest.value.present()) {
        return PhaseResult<ClosedTransaction>::failure({RuntimeErrorCode::PreconditionMissing,
                                                        LifecycleDisposition::Hold,
                                                        "ClosedTransaction",
                                                        "missing_transaction_digest"});
    }
    return PhaseResult<ClosedTransaction>::success(ClosedTransaction{correction, digest});
}

template <typename Transport, typename Payload, typename = void>
struct CanSubmitPayload : std::false_type {};

template <typename Transport, typename Payload>
struct CanSubmitPayload<Transport, Payload, std::void_t<decltype(std::declval<Transport&>().submit(std::declval<Payload>()))>> : std::true_type {};

static_assert(!std::is_convertible<std::uint64_t, MonotonicNs>::value, "time units must be explicit");
static_assert(!std::is_copy_constructible<PreparedCommit>::value, "prepared commits are single-use move-only tokens");
static_assert(!std::is_assignable<SessionContext&, SessionContext>::value, "session snapshots are immutable after construction");

} // namespace dnxr::canonical
