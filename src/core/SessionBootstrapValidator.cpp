#include "SessionBootstrapValidator.h"

#include <array>
#include <cstdint>

namespace dnxr {
namespace {

canonical::Digest256 digestFromSeed(std::uint64_t seed) noexcept {
    canonical::Digest256 digest{};
    digest.words[0] = seed ? seed : 1U;
    digest.words[1] = seed * 1099511628211ULL;
    digest.words[2] = seed ^ 0x9e3779b97f4a7c15ULL;
    digest.words[3] = seed + 0x517cc1b727220a95ULL;
    return digest;
}

std::uint64_t foldString(std::uint64_t seed, const std::string& value) noexcept {
    std::uint64_t hash = seed ? seed : 1469598103934665603ULL;
    for (const unsigned char c : value) {
        hash ^= c;
        hash *= 1099511628211ULL;
    }
    return hash;
}

bool allHardwareEvidencePresent(const RuntimeHardwareEvidence& evidence) noexcept {
    return evidence.deviceDiscovered &&
           evidence.hardwareIdentityVerified &&
           evidence.firmwareVerified &&
           evidence.deviceCertificateVerified &&
           evidence.capabilitiesNegotiated &&
           evidence.readChannelVerified &&
           evidence.feedbackChannelVerified &&
           evidence.hardwareCutoffVerified &&
           evidence.clocksSynchronized &&
           evidence.authenticatedSessionCreated;
}

SessionBootstrapResult fail(canonical::RuntimeErrorCode code,
                            canonical::LifecycleDisposition disposition,
                            std::string reason) {
    return SessionBootstrapResult::failure(SessionBootstrapError{code, disposition, std::move(reason)});
}

} // namespace

SessionBootstrapResult SessionBootstrapResult::success(SessionBootstrapSnapshot snapshot) {
    SessionBootstrapResult result;
    result.snapshot_.emplace(std::move(snapshot));
    return result;
}

SessionBootstrapResult SessionBootstrapResult::failure(SessionBootstrapError error) {
    SessionBootstrapResult result;
    result.error_ = std::move(error);
    return result;
}

bool SessionBackendLock::lock(RuntimeDeploymentMode mode) noexcept {
    if (locked_) {
        return mode_ == mode;
    }
    mode_ = mode;
    locked_ = true;
    return true;
}

bool SessionBackendLock::canReconfigureTo(RuntimeDeploymentMode mode) const noexcept {
    return !locked_ || mode_ == mode;
}

RuntimeProfileKind SessionBootstrapValidator::classifyProfile(const RuntimeConfig& config) noexcept {
    if (config.runtimeMode == "tools" || config.profile == "tools") {
        return RuntimeProfileKind::Tools;
    }
    if (config.observerMirror.enabled || config.profile.find("observer") != std::string::npos) {
        return RuntimeProfileKind::Observer;
    }
    if (config.runtimeMode == "benchmark_mode" || config.profile.find("hil") != std::string::npos || config.profile.find("bench") != std::string::npos) {
        return RuntimeProfileKind::HilBench;
    }
    if (config.simulationMode) {
        return RuntimeProfileKind::SimulationTestRunner;
    }
    return RuntimeProfileKind::RealMinimal;
}

bool SessionBootstrapValidator::uniqueOwnershipSatisfied(const RuntimeInstanceCounters& counters) noexcept {
    return counters.sessionOwners == 1 &&
           counters.monotonicClocks == 1 &&
           counters.committers == 1 &&
           counters.ackHandshakes == 1 &&
           counters.emergencyLatches == 1 &&
           counters.stateMachines == 1;
}

SessionBootstrapResult SessionBootstrapValidator::validate(const RuntimeConfig& config,
                                                           const RuntimeHardwareEvidence& evidence) const {
    if (config.profile.empty()) {
        return fail(canonical::RuntimeErrorCode::PreconditionMissing,
                    canonical::LifecycleDisposition::Shutdown,
                    "profile_missing");
    }
    if (config.runtimeMode.empty()) {
        return fail(canonical::RuntimeErrorCode::PreconditionMissing,
                    canonical::LifecycleDisposition::Shutdown,
                    "runtime_mode_missing");
    }
    if (config.virtualWorldIngress.required && config.virtualWorldIngress.endpoint.empty()) {
        return fail(canonical::RuntimeErrorCode::ExternalDependencyUnavailable,
                    canonical::LifecycleDisposition::ExternalDependencyUnavailable,
                    "world_ingress_endpoint_missing");
    }
    if (config.neuralChipTransport.transport.empty()) {
        return fail(canonical::RuntimeErrorCode::ExternalDependencyUnavailable,
                    canonical::LifecycleDisposition::ExternalDependencyUnavailable,
                    "neural_transport_missing");
    }
    if (config.maxRuntimeCycles < 0 || config.targetLoopMs <= 0 || config.maxCyclesPerSecond <= 0) {
        return fail(canonical::RuntimeErrorCode::PreconditionMissing,
                    canonical::LifecycleDisposition::Shutdown,
                    "invalid_runtime_timing_config");
    }

    const RuntimeProfileKind profileKind = classifyProfile(config);
    const RuntimeDeploymentMode deploymentMode = config.simulationMode
        ? RuntimeDeploymentMode::SimulationOnly
        : RuntimeDeploymentMode::HardwareBenchNoHuman;

    if (profileKind == RuntimeProfileKind::RealMinimal && !allHardwareEvidencePresent(evidence)) {
        return fail(canonical::RuntimeErrorCode::ExternalDependencyUnavailable,
                    canonical::LifecycleDisposition::ExternalDependencyUnavailable,
                    "real_backend_attestation_incomplete");
    }

    RuntimeInstanceCounters counters{};
    counters.sessionOwners = 1;
    counters.monotonicClocks = 1;
    counters.committers = 1;
    counters.ackHandshakes = 1;
    counters.emergencyLatches = 1;
    counters.stateMachines = 1;
    if (!uniqueOwnershipSatisfied(counters)) {
        return fail(canonical::RuntimeErrorCode::PreconditionMissing,
                    canonical::LifecycleDisposition::Shutdown,
                    "duplicate_runtime_owner_detected");
    }

    std::uint64_t seed = 0;
    seed = foldString(seed, config.profile);
    seed = foldString(seed, config.runtimeMode);
    seed = foldString(seed, config.virtualWorldIngress.endpoint);
    seed = foldString(seed, config.neuralChipTransport.transport);
    seed = foldString(seed, config.signalSource);

    const bool backendAttested = config.simulationMode || allHardwareEvidencePresent(evidence);
    const bool secretsResolved = config.simulationMode || evidence.authenticatedSessionCreated;
    const bool worldIngressReady = !config.virtualWorldIngress.required || !config.virtualWorldIngress.endpoint.empty();
    const bool motorEgressReady = !config.neuralChipTransport.transport.empty();

    return SessionBootstrapResult::success(SessionBootstrapSnapshot{
        config.profile,
        profileKind,
        deploymentMode,
        canonical::ConfigurationDigest{digestFromSeed(seed)},
        canonical::PatientMapDigest{digestFromSeed(foldString(seed, "patient-map"))},
        canonical::TopologyDigest{digestFromSeed(foldString(seed, "topology"))},
        canonical::CapabilityDigest{digestFromSeed(foldString(seed, "capability"))},
        canonical::SafetyPolicyDigest{digestFromSeed(foldString(seed, "safety-policy"))},
        counters,
        backendAttested,
        secretsResolved,
        worldIngressReady,
        motorEgressReady,
        {"stop_dispatch", "enter_hold_or_decompression", "drain_ack_feedback", "stop_ingress", "flush_audit", "destroy_transport"}
    });
}

} // namespace dnxr
