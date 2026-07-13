#pragma once

#include "RuntimeCompositionRoot.h"
#include "RuntimeDeploymentMode.h"
#include "../config/RuntimeConfig.h"
#include "../runtime/CanonicalBciRuntimeContract.h"

#include <optional>
#include <string>
#include <vector>

namespace dnxr {

enum class RuntimeProfileKind {
    RealMinimal,
    SimulationTestRunner,
    HilBench,
    Observer,
    Tools
};

struct RuntimeInstanceCounters {
    int sessionOwners = 0;
    int monotonicClocks = 0;
    int committers = 0;
    int ackHandshakes = 0;
    int emergencyLatches = 0;
    int stateMachines = 0;
};

struct SessionBootstrapSnapshot {
    const std::string profile;
    const RuntimeProfileKind profileKind;
    const RuntimeDeploymentMode deploymentMode;
    const canonical::ConfigurationDigest configurationDigest;
    const canonical::PatientMapDigest patientMapDigest;
    const canonical::TopologyDigest topologyDigest;
    const canonical::CapabilityDigest capabilityDigest;
    const canonical::SafetyPolicyDigest safetyPolicyDigest;
    const RuntimeInstanceCounters instanceCounters;
    const bool backendAttested;
    const bool secretsResolved;
    const bool worldIngressReady;
    const bool motorEgressReady;
    const std::vector<std::string> destructionOrder;
};

struct SessionBootstrapError {
    canonical::RuntimeErrorCode code = canonical::RuntimeErrorCode::None;
    canonical::LifecycleDisposition disposition = canonical::LifecycleDisposition::Continue;
    std::string reason;
};

class SessionBootstrapResult {
public:
    static SessionBootstrapResult success(SessionBootstrapSnapshot snapshot);
    static SessionBootstrapResult failure(SessionBootstrapError error);

    bool ok() const noexcept { return snapshot_.has_value(); }
    const SessionBootstrapSnapshot& snapshot() const { return *snapshot_; }
    const SessionBootstrapError& error() const { return error_; }

private:
    std::optional<SessionBootstrapSnapshot> snapshot_;
    SessionBootstrapError error_{};
};

class SessionBackendLock {
public:
    bool lock(RuntimeDeploymentMode mode) noexcept;
    bool canReconfigureTo(RuntimeDeploymentMode mode) const noexcept;
    RuntimeDeploymentMode mode() const noexcept { return mode_; }
    bool locked() const noexcept { return locked_; }

private:
    bool locked_ = false;
    RuntimeDeploymentMode mode_ = RuntimeDeploymentMode::SimulationOnly;
};

class SessionBootstrapValidator {
public:
    SessionBootstrapResult validate(const RuntimeConfig& config,
                                    const RuntimeHardwareEvidence& evidence = RuntimeHardwareEvidence{}) const;
    static bool uniqueOwnershipSatisfied(const RuntimeInstanceCounters& counters) noexcept;
    static RuntimeProfileKind classifyProfile(const RuntimeConfig& config) noexcept;
};

} // namespace dnxr
