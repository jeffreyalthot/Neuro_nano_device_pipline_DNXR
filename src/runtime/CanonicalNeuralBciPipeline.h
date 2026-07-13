#pragma once

#include "RuntimePathLock.h"
#include "RuntimePipelineRunInput.h"
#include "RuntimeRealtimeTelemetry.h"
#include "neural_chip/AckHoldFrameFactory.h"
#include "neural_chip/EmergencyLatchController.h"
#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralChipPipelineDecisionTrace.h"
#include "neural_chip/NeuralChipPipelineStateGate.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/NeuralChipSessionDigest.h"
#include "neural_chip/PipelineResumeController.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace dnxr {

enum class CanonicalBciSchemaValidity {
    Unchecked,
    Valid,
    Invalid,
    SanitizedHold
};

enum class CanonicalBciSafetyDecision {
    Undecided,
    Hold,
    DispatchAllowed,
    EmergencyStop
};

enum class CanonicalBciProvenanceKind {
    VirtualWorldEngine,
    NeuralChipAcquisition,
    IndependentBiometricSource,
    CanonicalPipelineStage,
    NeuralChipFeedback,
    ObserverDiagnostic
};

struct CanonicalBciFrameStamp {
    std::uint64_t frameId = 0;
    std::uint64_t schemaVersion = 1;
    std::uint64_t monotonicTimestampNs = 0;
    CanonicalBciProvenanceKind provenance = CanonicalBciProvenanceKind::CanonicalPipelineStage;
    CanonicalBciSchemaValidity validity = CanonicalBciSchemaValidity::Unchecked;
    CanonicalBciSafetyDecision safetyDecision = CanonicalBciSafetyDecision::Undecided;
    const char* validityReason = "unchecked";
};

struct CanonicalWorldFrame {
    CanonicalBciFrameStamp stamp{};
    std::vector<float> sceneSemantics;
    std::vector<float> audioEnvelope;
    std::vector<float> contactFields;
    std::vector<float> poseAndMotion;
    std::string digest;
};

struct CanonicalBciAcquisitionFrame {
    CanonicalBciFrameStamp stamp{};
    std::vector<float> rawNeuralSignals;
    std::vector<float> independentBiometrics;
};

struct CanonicalBciCommandFrame {
    CanonicalBciFrameStamp stamp{};
    std::vector<float> boundedStimulusChannels;
    bool patientBindingVerified = false;
    bool capabilitiesVerified = false;
    bool safetyEnvelopeVerified = false;
    bool lifecyclePolicyVerified = false;
    bool ackPolicyVerified = false;
    bool feedbackCapabilityVerified = false;
    bool chemicalGateVerified = false;
};

struct CanonicalBciFeedbackFrame {
    CanonicalBciFrameStamp stamp{};
    bool ackReceived = false;
    bool autoAck = false;
    std::vector<float> chipTelemetry;
    std::vector<float> closedLoopFeedback;
};

struct CanonicalBciStageTiming {
    double queueWaitMs = 0.0;
    double executionMs = 0.0;
    double externalWaitMs = 0.0;
    double deadlineSlackMs = 0.0;
    double totalMs = 0.0;
};

struct CanonicalBciCycleTransaction {
    CanonicalBciFrameStamp stamp{};
    RuntimePathDecision pathDecision{};
    CanonicalWorldFrame world{};
    CanonicalBciAcquisitionFrame acquisition{};
    CanonicalBciCommandFrame command{};
    CanonicalBciFeedbackFrame feedback{};
    std::array<CanonicalBciStageTiming, 22> stageTimings{};
    std::uint64_t traceDigest = 1469598103934665603ULL;
    bool preparedCommitOnly = false;
};

enum class CanonicalBciStage : std::uint8_t {
    BootAndAttestation,
    AcquireWorld,
    SanitizeWorld,
    AcquireBci,
    SignalProcessing,
    DecodeIntentMotorCognition,
    UpdateAvatarWorld,
    CalibrateAndBindPatient,
    SensoryEncoding,
    EncoderOutputContracts,
    NormalizeAndAlign,
    SpikeMappingAndPpp,
    IsolationPolicyCapabilityDose,
    PhaseLock,
    GlobalSafety,
    PrepareCommit,
    DispatchTransport,
    Ack,
    Feedback,
    Correction,
    PostCorrectionSafety,
    FinalizeAudit
};

struct CanonicalBciStageDescriptor {
    CanonicalBciStage id;
    const char* inputs;
    const char* outputs;
    std::uint64_t budgetNs;
    const char* errorDisposition;
    const char* owner;
};

inline constexpr std::array<CanonicalBciStage, 22> kCanonicalBciStageOrder{
    CanonicalBciStage::BootAndAttestation,
    CanonicalBciStage::AcquireWorld,
    CanonicalBciStage::SanitizeWorld,
    CanonicalBciStage::AcquireBci,
    CanonicalBciStage::SignalProcessing,
    CanonicalBciStage::DecodeIntentMotorCognition,
    CanonicalBciStage::UpdateAvatarWorld,
    CanonicalBciStage::CalibrateAndBindPatient,
    CanonicalBciStage::SensoryEncoding,
    CanonicalBciStage::EncoderOutputContracts,
    CanonicalBciStage::NormalizeAndAlign,
    CanonicalBciStage::SpikeMappingAndPpp,
    CanonicalBciStage::IsolationPolicyCapabilityDose,
    CanonicalBciStage::PhaseLock,
    CanonicalBciStage::GlobalSafety,
    CanonicalBciStage::PrepareCommit,
    CanonicalBciStage::DispatchTransport,
    CanonicalBciStage::Ack,
    CanonicalBciStage::Feedback,
    CanonicalBciStage::Correction,
    CanonicalBciStage::PostCorrectionSafety,
    CanonicalBciStage::FinalizeAudit};

inline constexpr std::array<CanonicalBciStageDescriptor, 22> kCanonicalBciStageRegistry{{
    {CanonicalBciStage::BootAndAttestation, "session evidence", "attested cycle", 250000ULL, "shutdown", "CanonicalNeuralBciPipeline"},
    {CanonicalBciStage::AcquireWorld, "world source", "world frame", 700000ULL, "hold", "CanonicalNeuralBciPipeline"},
    {CanonicalBciStage::SanitizeWorld, "world frame", "sanitized world", 500000ULL, "hold", "CanonicalNeuralBciPipeline"},
    {CanonicalBciStage::AcquireBci, "chip acquisition", "neural acquisition", 1200000ULL, "hold", "RuntimeAcquisitionStageAdapter"},
    {CanonicalBciStage::SignalProcessing, "neural acquisition", "conditioned signal", 1200000ULL, "hold", "RuntimeSignalStageAdapter"},
    {CanonicalBciStage::DecodeIntentMotorCognition, "conditioned signal", "inference", 1200000ULL, "hold", "RuntimeInferenceStageAdapter"},
    {CanonicalBciStage::UpdateAvatarWorld, "inference", "motor intent", 500000ULL, "hold", "RuntimeControllerMotorAdapter"},
    {CanonicalBciStage::CalibrateAndBindPatient, "motor intent", "calibration", 1000000ULL, "hold", "RuntimeCalibrationStageAdapter"},
    {CanonicalBciStage::SensoryEncoding, "calibration", "encoded modalities", 1200000ULL, "hold", "RuntimeImmersionFrameStageAdapter"},
    {CanonicalBciStage::EncoderOutputContracts, "encoded modalities", "contracted modalities", 500000ULL, "hold", "CanonicalNeuralBciPipeline"},
    {CanonicalBciStage::NormalizeAndAlign, "contracted modalities", "aligned modalities", 700000ULL, "hold", "RuntimeFullDiveStageAdapter"},
    {CanonicalBciStage::SpikeMappingAndPpp, "aligned modalities", "spike plan", 1000000ULL, "hold", "RuntimeFullDiveStageAdapter"},
    {CanonicalBciStage::IsolationPolicyCapabilityDose, "spike plan", "bounded dose plan", 900000ULL, "hold", "RuntimeSafetyStageAdapter"},
    {CanonicalBciStage::PhaseLock, "bounded dose plan", "phase locked plan", 500000ULL, "hold", "RuntimeModalityPhaseLockStageAdapter"},
    {CanonicalBciStage::GlobalSafety, "phase locked plan", "safety decision", 900000ULL, "emergency", "RuntimeSafetyStageAdapter"},
    {CanonicalBciStage::PrepareCommit, "safety decision", "prepared commit", 300000ULL, "hold", "CanonicalNeuralBciPipeline"},
    {CanonicalBciStage::DispatchTransport, "prepared commit", "closed transport gate", 100000ULL, "external_dependency_unavailable", "FutureCertifiedTransport"},
    {CanonicalBciStage::Ack, "submit receipt", "external ack", 100000ULL, "external_dependency_unavailable", "FutureCertifiedTransport"},
    {CanonicalBciStage::Feedback, "external ack", "external feedback", 100000ULL, "external_dependency_unavailable", "FutureCertifiedTransport"},
    {CanonicalBciStage::Correction, "external feedback", "correction", 500000ULL, "hold", "RuntimePerceptionCorrectionStageAdapter"},
    {CanonicalBciStage::PostCorrectionSafety, "correction", "post safety", 500000ULL, "emergency", "RuntimePostCorrectionSafetyStageAdapter"},
    {CanonicalBciStage::FinalizeAudit, "closed or held tx", "audit record", 300000ULL, "shutdown", "CanonicalNeuralBciPipeline"}
}};

class IVirtualWorldFrameSource {
public:
    virtual ~IVirtualWorldFrameSource() = default;
    virtual CanonicalWorldFrame acquireWorldFrame(std::uint64_t expectedFrameId) = 0;
};

class IIndependentBiometricSource {
public:
    virtual ~IIndependentBiometricSource() = default;
    virtual std::vector<float> acquireBiometrics(std::uint64_t frameId) = 0;
};

class IReadOnlyObserverDiagnostics {
public:
    virtual ~IReadOnlyObserverDiagnostics() = default;
    virtual void observeCommittedCycle(const CanonicalBciCycleTransaction& transaction) = 0;
};

struct CanonicalWorldIngressBundle {
    IVirtualWorldFrameSource* worldSource = nullptr;
};

struct CanonicalBciIngressBundle {
    IIndependentBiometricSource* independentBiometrics = nullptr;
};

struct CanonicalSafetyLifecycleBundle {
    NeuralChipBus bus{};
    FrameLifecycleGuard lifecycleGuard{};
    NeuralChipAckHandshake ackHandshake{};
    NeuralChipPipelineStateMachine stateMachine{};
    NeuralChipPipelineStateGate stateGate{};
    AckHoldFrameFactory ackHoldFactory{};
    SafeChipFrameHoldBuffer holdBuffer{};
    PipelineResumeController resumeController{};
    NeuralChipEmergencyLatch emergencyLatch{};
    EmergencyResetAuditTrail emergencyResetAudit{};
    EmergencyLatchController emergencyLatchController{emergencyLatch, emergencyResetAudit, resumeController};
    NeuralChipFrameCommitter committer{bus, lifecycleGuard, holdBuffer};
    NeuralChipReplayRecorder replayRecorder{};
    NeuralChipSessionDigest sessionDigest{};
    NeuralChipPipelineDecisionTrace decisionTrace{};
};

struct CanonicalObservabilityBundle {
    IReadOnlyObserverDiagnostics* observer = nullptr;
};

struct CanonicalPipelineMetrics {
    std::uint64_t cyclesStarted = 0;
    std::uint64_t cyclesCompleted = 0;
    std::uint64_t delegatedCompatibilityCycles = 0;
    std::uint64_t commitsPrepared = 0;
    std::uint64_t dispatchesObserved = 0;
    std::uint64_t metricRecords = 0;
    std::uint64_t rejectedDoubleStarts = 0;
    std::uint64_t rejectedFrameIdRegressions = 0;
    std::uint64_t rejectedAdapterFallbacks = 0;
};

class CanonicalNeuralBciPipeline {
public:
    static constexpr std::uint64_t kSchemaVersion = 1;

    CanonicalNeuralBciPipeline();
    ~CanonicalNeuralBciPipeline();

    CanonicalNeuralBciPipeline(const CanonicalNeuralBciPipeline&) = delete;
    CanonicalNeuralBciPipeline& operator=(const CanonicalNeuralBciPipeline&) = delete;

    RuntimePipelineFrame runCycle(RuntimePipelineRunInputRef input,
                                  RuntimePathDecision pathDecision);

    const CanonicalPipelineMetrics& metrics() const;
    const CanonicalBciCycleTransaction& lastTransaction() const;
    RuntimeRealtimeSnapshot realtimeSnapshot() const;
    NeuralChipPipelineState globalState() const;
    bool cycleInProgress() const;
    static std::uint64_t activeInstanceCount();

    static bool stageOrderIsCanonical(const std::vector<CanonicalBciStage>& candidate) {
        if (candidate.size() != kCanonicalBciStageOrder.size()) {
            return false;
        }
        for (std::size_t i = 0; i < kCanonicalBciStageOrder.size(); ++i) {
            if (candidate[i] != kCanonicalBciStageOrder[i]) {
                return false;
            }
        }
        return true;
    }

    static bool stageRegistryIsValid();
    static bool stageCoverageAccepts(const std::vector<CanonicalBciStage>& observed);

    static bool dispatchPreconditionsSatisfied(const CanonicalBciCycleTransaction& transaction) {
        return transaction.world.stamp.validity == CanonicalBciSchemaValidity::Valid &&
               transaction.command.patientBindingVerified &&
               transaction.command.capabilitiesVerified &&
               transaction.command.safetyEnvelopeVerified &&
               transaction.command.lifecyclePolicyVerified &&
               transaction.command.ackPolicyVerified &&
               transaction.command.feedbackCapabilityVerified &&
               !transaction.feedback.autoAck;
    }

private:
    CanonicalBciFrameStamp makeStamp(std::uint64_t frameId,
                                     std::uint64_t monotonicTimestampNs,
                                     CanonicalBciProvenanceKind provenance,
                                     CanonicalBciSchemaValidity validity,
                                     CanonicalBciSafetyDecision safety,
                                     const char* reason) const;
    void beginCycle(std::uint64_t frameId, std::uint64_t monotonicTimestampNs, RuntimePathDecision pathDecision);
    void finishCycle(const RuntimePipelineFrame& frame);

    CanonicalWorldIngressBundle worldIngress_{};
    CanonicalBciIngressBundle bciIngress_{};
    CanonicalSafetyLifecycleBundle safetyLifecycle_{};
    CanonicalObservabilityBundle observability_{};
    CanonicalPipelineMetrics metrics_{};
    RuntimeRealtimeTelemetry realtimeTelemetry_{};
    CanonicalBciCycleTransaction lastTransaction_{};
    bool cycleInProgress_ = false;
    std::bitset<22> currentStageCoverage_{};
    bool hasLastFrameId_ = false;
    std::uint64_t lastFrameId_ = 0;
};

} // namespace dnxr
