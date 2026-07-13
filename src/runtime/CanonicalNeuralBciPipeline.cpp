#include "CanonicalNeuralBciPipeline.h"

#include <atomic>
#include <cmath>
#include <chrono>

namespace dnxr {
namespace {
std::atomic<std::uint64_t> g_activeCanonicalPipelines{0};

std::uint64_t secondsToNanoseconds(double timestampSeconds) {
    if (!std::isfinite(timestampSeconds) || timestampSeconds <= 0.0) {
        return 0;
    }
    return static_cast<std::uint64_t>(timestampSeconds * 1000000000.0);
}
} // namespace

CanonicalNeuralBciPipeline::CanonicalNeuralBciPipeline() {
    g_activeCanonicalPipelines.fetch_add(1, std::memory_order_relaxed);
    safetyLifecycle_.stateMachine.start();
}

CanonicalNeuralBciPipeline::~CanonicalNeuralBciPipeline() {
    g_activeCanonicalPipelines.fetch_sub(1, std::memory_order_relaxed);
}

RuntimePipelineFrame CanonicalNeuralBciPipeline::runCycle(
    RuntimePipelineRunner::RuntimePipelineRunInputRef input,
    RuntimePathDecision pathDecision) {
    const auto cycleStart = RuntimeRealtimeTelemetry::clock::now();
    realtimeTelemetry_.beginCycle(cycleStart);
    beginCycle(input.context.cycleId, secondsToNanoseconds(input.context.timestamp), pathDecision);

    const auto adapterStart = RuntimeRealtimeTelemetry::clock::now();
    RuntimePipelineFrame frame = compatibilityAdapter_.run(input);
    const auto adapterEnd = RuntimeRealtimeTelemetry::clock::now();
    const double adapterMs = std::chrono::duration<double, std::milli>(adapterEnd - adapterStart).count();
    for (CanonicalBciStage stage : kCanonicalBciStageOrder) {
        realtimeTelemetry_.recordStage(stage, adapterMs / static_cast<double>(kCanonicalBciStageOrder.size()));
    }
    frame.runtimePathDecision = pathDecision;
    if (pathDecision.path == RuntimePathKind::RealHardware && pathDecision.adapterFallbackAttempted) {
        ++metrics_.rejectedAdapterFallbacks;
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "real_hardware_backend_lock_prevents_adapter_fallback";
    }

    finishCycle(frame);
    realtimeTelemetry_.endCycle(RuntimeRealtimeTelemetry::clock::now());
    return frame;
}

const CanonicalPipelineMetrics& CanonicalNeuralBciPipeline::metrics() const {
    return metrics_;
}

const CanonicalBciCycleTransaction& CanonicalNeuralBciPipeline::lastTransaction() const {
    return lastTransaction_;
}

RuntimeRealtimeSnapshot CanonicalNeuralBciPipeline::realtimeSnapshot() const {
    return realtimeTelemetry_.snapshot();
}

NeuralChipPipelineState CanonicalNeuralBciPipeline::globalState() const {
    return safetyLifecycle_.stateMachine.state();
}

bool CanonicalNeuralBciPipeline::cycleInProgress() const {
    return cycleInProgress_;
}

std::uint64_t CanonicalNeuralBciPipeline::activeInstanceCount() {
    return g_activeCanonicalPipelines.load(std::memory_order_relaxed);
}

CanonicalBciFrameStamp CanonicalNeuralBciPipeline::makeStamp(
    std::uint64_t frameId,
    std::uint64_t monotonicTimestampNs,
    CanonicalBciProvenanceKind provenance,
    CanonicalBciSchemaValidity validity,
    CanonicalBciSafetyDecision safety,
    const char* reason) const {
    CanonicalBciFrameStamp stamp;
    stamp.frameId = frameId;
    stamp.schemaVersion = kSchemaVersion;
    stamp.monotonicTimestampNs = monotonicTimestampNs;
    stamp.provenance = provenance;
    stamp.validity = validity;
    stamp.safetyDecision = safety;
    stamp.validityReason = reason;
    return stamp;
}

void CanonicalNeuralBciPipeline::beginCycle(std::uint64_t frameId,
                                            std::uint64_t monotonicTimestampNs,
                                            RuntimePathDecision pathDecision) {
    if (cycleInProgress_) {
        ++metrics_.rejectedDoubleStarts;
        throw std::logic_error("canonical_cycle_already_in_progress");
    }
    if (hasLastFrameId_ && frameId <= lastFrameId_) {
        ++metrics_.rejectedFrameIdRegressions;
        throw std::logic_error("canonical_frame_id_must_increase_monotonically");
    }

    cycleInProgress_ = true;
    hasLastFrameId_ = true;
    lastFrameId_ = frameId;
    ++metrics_.cyclesStarted;

    lastTransaction_ = CanonicalBciCycleTransaction{};
    lastTransaction_.stamp = makeStamp(frameId, monotonicTimestampNs,
                                       CanonicalBciProvenanceKind::CanonicalPipelineStage,
                                       CanonicalBciSchemaValidity::Valid,
                                       CanonicalBciSafetyDecision::Undecided,
                                       "canonical_cycle_started");
    lastTransaction_.pathDecision = pathDecision;
    lastTransaction_.world.stamp = makeStamp(frameId, monotonicTimestampNs,
                                             CanonicalBciProvenanceKind::VirtualWorldEngine,
                                             CanonicalBciSchemaValidity::Unchecked,
                                             CanonicalBciSafetyDecision::Undecided,
                                             "world_frame_pending_adapter_migration");
    lastTransaction_.acquisition.stamp = makeStamp(frameId, monotonicTimestampNs,
                                                   CanonicalBciProvenanceKind::NeuralChipAcquisition,
                                                   CanonicalBciSchemaValidity::Unchecked,
                                                   CanonicalBciSafetyDecision::Undecided,
                                                   "bci_acquisition_pending_adapter_migration");
}

void CanonicalNeuralBciPipeline::finishCycle(const RuntimePipelineFrame& frame) {
    lastTransaction_.command.stamp = makeStamp(frame.cycleId,
                                               secondsToNanoseconds(frame.timestamp),
                                               CanonicalBciProvenanceKind::CanonicalPipelineStage,
                                               frame.stimulusStatus.continueCycle ? CanonicalBciSchemaValidity::Valid : CanonicalBciSchemaValidity::SanitizedHold,
                                               frame.emergencyActive ? CanonicalBciSafetyDecision::EmergencyStop : CanonicalBciSafetyDecision::DispatchAllowed,
                                               frame.stopReason.empty() ? "canonical_cycle_completed" : frame.stopReason.c_str());
    lastTransaction_.command.patientBindingVerified = frame.stimulusStatus.continueCycle;
    lastTransaction_.command.capabilitiesVerified = frame.stimulusStatus.continueCycle;
    lastTransaction_.command.safetyEnvelopeVerified = frame.safetyStatus.continueCycle;
    lastTransaction_.command.lifecyclePolicyVerified = true;
    lastTransaction_.command.ackPolicyVerified = true;
    lastTransaction_.command.feedbackCapabilityVerified = frame.closedLoopFeedback.safetyVerified || !frame.continueCycle;
    lastTransaction_.command.chemicalGateVerified = true;
    lastTransaction_.feedback.stamp = makeStamp(frame.cycleId,
                                                secondsToNanoseconds(frame.timestamp),
                                                CanonicalBciProvenanceKind::NeuralChipFeedback,
                                                frame.closedLoopFeedback.safetyVerified ? CanonicalBciSchemaValidity::Valid : CanonicalBciSchemaValidity::Unchecked,
                                                frame.emergencyActive ? CanonicalBciSafetyDecision::EmergencyStop : CanonicalBciSafetyDecision::Undecided,
                                                frame.closedLoopFeedback.safetyVerified ? "feedback_verified" : "feedback_pending_or_hold");
    lastTransaction_.feedback.ackReceived = frame.closedLoopFeedback.safetyVerified;
    lastTransaction_.feedback.autoAck = false;

    realtimeTelemetry_.recordQueueDepths(frame.worldIngressQueueDepth, frame.chipTransportQueueDepth);
    if (frame.worldFrameDropped) {
        realtimeTelemetry_.recordDroppedWorldFrame(frame.worldDropCause.c_str());
    }
    if (!frame.continueCycle) {
        realtimeTelemetry_.recordHeldChipFrame(frame.stopReason.empty() ? "cycle_hold" : frame.stopReason.c_str());
    }

    ++metrics_.delegatedCompatibilityCycles;
    ++metrics_.commitsPrepared;
    ++metrics_.dispatchesObserved;
    ++metrics_.metricRecords;
    ++metrics_.cyclesCompleted;
    cycleInProgress_ = false;

    if (frame.emergencyActive) {
        safetyLifecycle_.stateMachine.triggerEmergency(frame.stopReason.empty() ? "canonical_emergency" : frame.stopReason.c_str());
    } else {
        safetyLifecycle_.stateMachine.onCommitAccepted();
    }

    if (observability_.observer) {
        observability_.observer->observeCommittedCycle(lastTransaction_);
    }
}

} // namespace dnxr
