#include "CanonicalNeuralBciPipeline.h"

#include "RuntimeBodyPoseExtractionStage.h"
#include "RuntimeBodySchemaStage.h"
#include "RuntimeFiveSenseCoherenceStage.h"
#include "RuntimePerceptionCorrectionStage.h"
#include "RuntimeCycleFinalizer.h"
#include "RuntimeModalityPhaseLockStage.h"
#include "RuntimePostCorrectionSafetyStage.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>

namespace dnxr {
namespace {
std::atomic<std::uint64_t> g_activeCanonicalPipelines{0};

std::uint64_t secondsToNanoseconds(double timestampSeconds) {
    if (!std::isfinite(timestampSeconds) || timestampSeconds <= 0.0) {
        return 0;
    }
    return static_cast<std::uint64_t>(timestampSeconds * 1000000000.0);
}

std::size_t stageIndex(CanonicalBciStage stage) noexcept {
    return static_cast<std::size_t>(stage);
}

const char* stageName(CanonicalBciStage stage) noexcept {
    switch (stage) {
    case CanonicalBciStage::BootAndAttestation: return "BootAndAttestation";
    case CanonicalBciStage::AcquireWorld: return "AcquireWorld";
    case CanonicalBciStage::SanitizeWorld: return "SanitizeWorld";
    case CanonicalBciStage::AcquireBci: return RuntimeAcquisitionStage::name();
    case CanonicalBciStage::SignalProcessing: return RuntimeSignalStage::name();
    case CanonicalBciStage::DecodeIntentMotorCognition: return RuntimeInferenceStage::name();
    case CanonicalBciStage::UpdateAvatarWorld: return "UpdateAvatarWorld";
    case CanonicalBciStage::CalibrateAndBindPatient: return RuntimeCalibrationStage::name();
    case CanonicalBciStage::SensoryEncoding: return RuntimeImmersionFrameStage::name();
    case CanonicalBciStage::EncoderOutputContracts: return "EncoderOutputContracts";
    case CanonicalBciStage::NormalizeAndAlign: return RuntimeFullDiveStage::name();
    case CanonicalBciStage::SpikeMappingAndPpp: return "SpikeMappingAndPpp";
    case CanonicalBciStage::IsolationPolicyCapabilityDose: return "IsolationPolicyCapabilityDose";
    case CanonicalBciStage::PhaseLock: return RuntimeModalityPhaseLockStage::name();
    case CanonicalBciStage::GlobalSafety: return RuntimeSafetyStage::name();
    case CanonicalBciStage::PrepareCommit: return "PrepareCommit";
    case CanonicalBciStage::DispatchTransport: return "DispatchTransport";
    case CanonicalBciStage::Ack: return "Ack";
    case CanonicalBciStage::Feedback: return "Feedback";
    case CanonicalBciStage::Correction: return RuntimePerceptionCorrectionStage::name();
    case CanonicalBciStage::PostCorrectionSafety: return RuntimePostCorrectionSafetyStage::name();
    case CanonicalBciStage::FinalizeAudit: return "FinalizeAudit";
    }
    return "unknown";
}

std::uint64_t digestMix(std::uint64_t seed, const char* text, std::uint64_t value) noexcept {
    constexpr std::uint64_t prime = 1099511628211ULL;
    std::uint64_t h = seed;
    for (const char* p = text; p && *p; ++p) {
        h ^= static_cast<unsigned char>(*p);
        h *= prime;
    }
    h ^= value;
    h *= prime;
    return h;
}
} // namespace

CanonicalNeuralBciPipeline::CanonicalNeuralBciPipeline() {
    if (!stageRegistryIsValid()) {
        throw std::logic_error("canonical_stage_registry_invalid");
    }
    g_activeCanonicalPipelines.fetch_add(1, std::memory_order_relaxed);
    safetyLifecycle_.stateMachine.start();
}

CanonicalNeuralBciPipeline::~CanonicalNeuralBciPipeline() {
    g_activeCanonicalPipelines.fetch_sub(1, std::memory_order_relaxed);
}

bool CanonicalNeuralBciPipeline::stageRegistryIsValid() {
    std::bitset<22> seen;
    for (std::size_t i = 0; i < kCanonicalBciStageRegistry.size(); ++i) {
        const auto& descriptor = kCanonicalBciStageRegistry[i];
        const auto index = stageIndex(descriptor.id);
        if (index >= kCanonicalBciStageRegistry.size() || seen.test(index)) {
            return false;
        }
        if (descriptor.id != kCanonicalBciStageOrder[i] || descriptor.budgetNs == 0U ||
            std::strstr(descriptor.owner, "Render") != nullptr || std::strstr(descriptor.owner, "XRRenderer") != nullptr) {
            return false;
        }
        seen.set(index);
    }
    return seen.count() == kCanonicalBciStageRegistry.size();
}

bool CanonicalNeuralBciPipeline::stageCoverageAccepts(const std::vector<CanonicalBciStage>& observed) {
    if (observed.size() > kCanonicalBciStageOrder.size()) {
        return false;
    }
    std::bitset<22> seen;
    for (std::size_t i = 0; i < observed.size(); ++i) {
        const auto index = stageIndex(observed[i]);
        if (index >= kCanonicalBciStageOrder.size() || seen.test(index) || observed[i] != kCanonicalBciStageOrder[i]) {
            return false;
        }
        seen.set(index);
    }
    return true;
}

RuntimePipelineFrame CanonicalNeuralBciPipeline::runCycle(
    RuntimePipelineRunInputRef input,
    RuntimePathDecision pathDecision) {
    const auto cycleStart = RuntimeRealtimeTelemetry::clock::now();
    realtimeTelemetry_.beginCycle(cycleStart);
    beginCycle(input.context.cycleId, secondsToNanoseconds(input.context.timestamp), pathDecision);

    RuntimePipelineFrame frame;
    frame.cycleId = input.context.cycleId;
    frame.timestamp = input.context.timestamp;
    frame.deltaTimeSeconds = input.context.deltaTimeSeconds;
    frame.runtimePathDecision = pathDecision;

    auto failClosed = [&](const char* reason, bool emergency = false) {
        frame.continueCycle = false;
        frame.emergencyActive = emergency;
        frame.stopReason = reason;
    };

    auto invoke = [&](CanonicalBciStage stage, auto&& body) {
        const std::size_t index = stageIndex(stage);
        if (index >= currentStageCoverage_.size() || currentStageCoverage_.test(index) || stage != kCanonicalBciStageOrder[index]) {
            failClosed("canonical_stage_reentrancy_or_order_violation", true);
            return;
        }
        const auto stageStart = RuntimeRealtimeTelemetry::clock::now();
        currentStageCoverage_.set(index);
        body();
        const auto stageEnd = RuntimeRealtimeTelemetry::clock::now();
        const double elapsedMs = std::chrono::duration<double, std::milli>(stageEnd - stageStart).count();
        const double budgetMs = static_cast<double>(kCanonicalBciStageRegistry[index].budgetNs) / 1000000.0;
        lastTransaction_.stageTimings[index].executionMs = elapsedMs;
        lastTransaction_.stageTimings[index].deadlineSlackMs = budgetMs - elapsedMs;
        lastTransaction_.stageTimings[index].totalMs = elapsedMs;
        realtimeTelemetry_.recordStage(stage, elapsedMs);
        lastTransaction_.traceDigest = digestMix(lastTransaction_.traceDigest, stageName(stage), frame.cycleId + frame.rawNeuralSignals.size() + frame.cleanSignals.size() + frame.features.size());
    };

    invoke(CanonicalBciStage::BootAndAttestation, [&] {
        if (pathDecision.path == RuntimePathKind::RealHardware && pathDecision.adapterFallbackAttempted) {
            ++metrics_.rejectedAdapterFallbacks;
            failClosed("real_hardware_backend_lock_prevents_adapter_fallback", true);
        }
    });
    invoke(CanonicalBciStage::AcquireWorld, [&] {
        const auto worldRead = input.services.worldIngress.readWorldFrame("runtime-simulation-session", frame.timestamp * 1000.0);
        frame.worldIngressFrame = worldRead.snapshot;
        frame.canonicalWorldFrame = worldRead.world;
        frame.worldFrameDropped = !worldRead.accepted;
        frame.worldDropCause = worldRead.reason;
        lastTransaction_.world.stamp = makeStamp(frame.cycleId, secondsToNanoseconds(frame.timestamp), CanonicalBciProvenanceKind::VirtualWorldEngine, worldRead.accepted ? CanonicalBciSchemaValidity::Valid : CanonicalBciSchemaValidity::SanitizedHold, worldRead.accepted ? CanonicalBciSafetyDecision::Undecided : CanonicalBciSafetyDecision::Hold, worldRead.reason.c_str());
        lastTransaction_.world.digest = worldRead.accepted ? canonicalVirtualWorldDigest(worldRead.snapshot) : worldRead.reason;
        if (!worldRead.accepted) {
            failClosed(worldRead.reason.c_str(), false);
        }
    });
    if (frame.continueCycle) invoke(CanonicalBciStage::SanitizeWorld, [&] {
        lastTransaction_.world.stamp.validity = CanonicalBciSchemaValidity::Valid;
    });
    if (frame.continueCycle) invoke(CanonicalBciStage::AcquireBci, [&] { frame = RuntimeAcquisitionStage::execute(input.context, input.services); });
    if (frame.continueCycle) invoke(CanonicalBciStage::SignalProcessing, [&] { frame = RuntimeSignalStage::execute(std::move(frame), input.services); });
    if (frame.continueCycle) invoke(CanonicalBciStage::DecodeIntentMotorCognition, [&] { frame.normalizedTelemetryRisk = frame.telemetry; frame = RuntimeInferenceStage::execute(std::move(frame), input.services); });
    if (frame.continueCycle) invoke(CanonicalBciStage::UpdateAvatarWorld, [&] { if (input.sinks.publishPipelineState) input.sinks.publishPipelineState(frame); });
    if (frame.continueCycle) invoke(CanonicalBciStage::CalibrateAndBindPatient, [&] { frame = RuntimeCalibrationStage::execute(std::move(frame), input.services); });
    if (frame.continueCycle) invoke(CanonicalBciStage::SensoryEncoding, [&] { frame = RuntimeImmersionFrameStage::execute(std::move(frame), input.services); });
    if (frame.continueCycle) invoke(CanonicalBciStage::EncoderOutputContracts, [&] { if (input.sinks.recordReplayFrame) input.sinks.recordReplayFrame(frame.cleanSignals, input.context.timestamp); frame.acquisitionChipFrame = frame.chipFrame; });
    if (frame.continueCycle) invoke(CanonicalBciStage::NormalizeAndAlign, [&] { frame = RuntimeFullDiveStage::execute(std::move(frame), input.services, input.context, input.runtimeMetricsSnapshot); });
    if (frame.continueCycle) invoke(CanonicalBciStage::SpikeMappingAndPpp, [&] { frame = RuntimeFiveSenseCoherenceStage::execute(std::move(frame)); if (frame.continueCycle) frame = RuntimeBodyPoseExtractionStage::execute(std::move(frame), input.services); if (frame.continueCycle) frame = RuntimeBodySchemaStage::execute(std::move(frame), frame.perceivedBodyPose, frame.avatarBodyPose); });
    if (frame.continueCycle) invoke(CanonicalBciStage::IsolationPolicyCapabilityDose, [&] { frame.safetyStatus = RuntimeStageStatus::ok("isolation_policy_capability_dose"); });
    if (frame.continueCycle) invoke(CanonicalBciStage::PhaseLock, [&] { frame = RuntimeModalityPhaseLockStage::execute(std::move(frame)); });
    if (frame.continueCycle) invoke(CanonicalBciStage::GlobalSafety, [&] { frame = RuntimeSafetyStage::execute(std::move(frame), input.services, input.context); });
    if (frame.continueCycle) invoke(CanonicalBciStage::PrepareCommit, [&] { frame.stimulusStatus = RuntimeStimulusStage::validate(true, "stimulus_stage_unavailable"); lastTransaction_.preparedCommitOnly = frame.stimulusStatus.continueCycle; ++metrics_.commitsPrepared; });
    if (frame.continueCycle) invoke(CanonicalBciStage::DispatchTransport, [&] { failClosed("certified_transport_not_bound_prepared_commit_held", false); });

    if (!frame.continueCycle) {
        RuntimeCycleFinalizer::finish(input.tickReport, frame, input.services, input.sinks, input.finalizer, RuntimeCycleFinalizer::Options{false, frame.emergencyActive, frame.stopReason, false});
    }

    finishCycle(frame);
    realtimeTelemetry_.endCycle(RuntimeRealtimeTelemetry::clock::now());
    return frame;
}

const CanonicalPipelineMetrics& CanonicalNeuralBciPipeline::metrics() const { return metrics_; }
const CanonicalBciCycleTransaction& CanonicalNeuralBciPipeline::lastTransaction() const { return lastTransaction_; }
RuntimeRealtimeSnapshot CanonicalNeuralBciPipeline::realtimeSnapshot() const { return realtimeTelemetry_.snapshot(); }
NeuralChipPipelineState CanonicalNeuralBciPipeline::globalState() const { return safetyLifecycle_.stateMachine.state(); }
bool CanonicalNeuralBciPipeline::cycleInProgress() const { return cycleInProgress_; }
std::uint64_t CanonicalNeuralBciPipeline::activeInstanceCount() { return g_activeCanonicalPipelines.load(std::memory_order_relaxed); }

CanonicalBciFrameStamp CanonicalNeuralBciPipeline::makeStamp(std::uint64_t frameId, std::uint64_t monotonicTimestampNs, CanonicalBciProvenanceKind provenance, CanonicalBciSchemaValidity validity, CanonicalBciSafetyDecision safety, const char* reason) const {
    CanonicalBciFrameStamp stamp;
    stamp.frameId = frameId; stamp.schemaVersion = kSchemaVersion; stamp.monotonicTimestampNs = monotonicTimestampNs; stamp.provenance = provenance; stamp.validity = validity; stamp.safetyDecision = safety; stamp.validityReason = reason; return stamp;
}

void CanonicalNeuralBciPipeline::beginCycle(std::uint64_t frameId, std::uint64_t monotonicTimestampNs, RuntimePathDecision pathDecision) {
    if (cycleInProgress_) { ++metrics_.rejectedDoubleStarts; throw std::logic_error("canonical_cycle_already_in_progress"); }
    if (hasLastFrameId_ && frameId <= lastFrameId_) { ++metrics_.rejectedFrameIdRegressions; throw std::logic_error("canonical_frame_id_must_increase_monotonically"); }
    cycleInProgress_ = true; currentStageCoverage_.reset(); hasLastFrameId_ = true; lastFrameId_ = frameId; ++metrics_.cyclesStarted;
    lastTransaction_ = CanonicalBciCycleTransaction{};
    lastTransaction_.stamp = makeStamp(frameId, monotonicTimestampNs, CanonicalBciProvenanceKind::CanonicalPipelineStage, CanonicalBciSchemaValidity::Valid, CanonicalBciSafetyDecision::Undecided, "canonical_cycle_started");
    lastTransaction_.pathDecision = pathDecision;
    lastTransaction_.world.stamp = makeStamp(frameId, monotonicTimestampNs, CanonicalBciProvenanceKind::VirtualWorldEngine, CanonicalBciSchemaValidity::Unchecked, CanonicalBciSafetyDecision::Undecided, "world_frame_pending");
    lastTransaction_.acquisition.stamp = makeStamp(frameId, monotonicTimestampNs, CanonicalBciProvenanceKind::NeuralChipAcquisition, CanonicalBciSchemaValidity::Unchecked, CanonicalBciSafetyDecision::Undecided, "bci_acquisition_pending");
}

void CanonicalNeuralBciPipeline::finishCycle(const RuntimePipelineFrame& frame) {
    lastTransaction_.command.stamp = makeStamp(frame.cycleId, secondsToNanoseconds(frame.timestamp), CanonicalBciProvenanceKind::CanonicalPipelineStage, frame.stimulusStatus.continueCycle ? CanonicalBciSchemaValidity::Valid : CanonicalBciSchemaValidity::SanitizedHold, frame.emergencyActive ? CanonicalBciSafetyDecision::EmergencyStop : CanonicalBciSafetyDecision::Hold, frame.stopReason.empty() ? "canonical_cycle_completed" : frame.stopReason.c_str());
    lastTransaction_.command.patientBindingVerified = frame.stimulusStatus.continueCycle;
    lastTransaction_.command.capabilitiesVerified = frame.stimulusStatus.continueCycle;
    lastTransaction_.command.safetyEnvelopeVerified = frame.safetyStatus.continueCycle;
    lastTransaction_.command.lifecyclePolicyVerified = true;
    lastTransaction_.command.ackPolicyVerified = false;
    lastTransaction_.command.feedbackCapabilityVerified = false;
    lastTransaction_.command.chemicalGateVerified = true;
    lastTransaction_.feedback.stamp = makeStamp(frame.cycleId, secondsToNanoseconds(frame.timestamp), CanonicalBciProvenanceKind::NeuralChipFeedback, CanonicalBciSchemaValidity::Unchecked, frame.emergencyActive ? CanonicalBciSafetyDecision::EmergencyStop : CanonicalBciSafetyDecision::Hold, "feedback_not_read_until_certified_transport_prompt");
    lastTransaction_.feedback.ackReceived = false;
    lastTransaction_.feedback.autoAck = false;

    realtimeTelemetry_.recordQueueDepths(frame.worldIngressQueueDepth, frame.chipTransportQueueDepth);
    if (frame.worldFrameDropped) realtimeTelemetry_.recordDroppedWorldFrame(frame.worldDropCause.c_str());
    if (!frame.continueCycle) realtimeTelemetry_.recordHeldChipFrame(frame.stopReason.empty() ? "cycle_hold" : frame.stopReason.c_str());

    ++metrics_.metricRecords; ++metrics_.cyclesCompleted; cycleInProgress_ = false;
    if (frame.emergencyActive) safetyLifecycle_.stateMachine.triggerEmergency(frame.stopReason.empty() ? "canonical_emergency" : frame.stopReason.c_str());
    if (observability_.observer) observability_.observer->observeCommittedCycle(lastTransaction_);
}

} // namespace dnxr
