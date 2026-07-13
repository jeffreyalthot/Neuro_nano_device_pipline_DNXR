#pragma once

#include "RuntimePathLock.h"
#include "RuntimePipelineRunner.h"
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

struct CanonicalBciCycleTransaction {
    CanonicalBciFrameStamp stamp{};
    RuntimePathDecision pathDecision{};
    CanonicalWorldFrame world{};
    CanonicalBciAcquisitionFrame acquisition{};
    CanonicalBciCommandFrame command{};
    CanonicalBciFeedbackFrame feedback{};
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

    RuntimePipelineFrame runCycle(RuntimePipelineRunner::RuntimePipelineRunInputRef input,
                                  RuntimePathDecision pathDecision);

    const CanonicalPipelineMetrics& metrics() const;
    const CanonicalBciCycleTransaction& lastTransaction() const;
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

    RuntimePipelineRunner compatibilityAdapter_{};
    CanonicalWorldIngressBundle worldIngress_{};
    CanonicalBciIngressBundle bciIngress_{};
    CanonicalSafetyLifecycleBundle safetyLifecycle_{};
    CanonicalObservabilityBundle observability_{};
    CanonicalPipelineMetrics metrics_{};
    CanonicalBciCycleTransaction lastTransaction_{};
    bool cycleInProgress_ = false;
    bool hasLastFrameId_ = false;
    std::uint64_t lastFrameId_ = 0;
};

} // namespace dnxr
