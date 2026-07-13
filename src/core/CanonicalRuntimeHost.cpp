#include "CanonicalRuntimeHost.h"

namespace dnxr {

SessionBootstrapResult CanonicalRuntimeHost::validateSession(const RuntimeConfig& config,
                                                             const RuntimeHardwareEvidence& evidence) {
    return SessionBootstrapValidator{}.validate(config, evidence);
}

std::optional<CanonicalRuntimeHost> CanonicalRuntimeHost::create(RuntimeConfig,
                                                                 RuntimeComposition composition,
                                                                 SessionBootstrapSnapshot snapshot) noexcept {
    SessionBackendLock lock;
    if (!lock.lock(snapshot.deploymentMode)) {
        return std::nullopt;
    }
    if (!SessionBootstrapValidator::uniqueOwnershipSatisfied(snapshot.instanceCounters)) {
        return std::nullopt;
    }
    return CanonicalRuntimeHost(CanonicalRuntimeHostGraph{std::move(composition), std::move(snapshot)}, lock);
}

CanonicalRuntimeHost::CanonicalRuntimeHost(CanonicalRuntimeHostGraph graph, SessionBackendLock lock) noexcept
    : graph_(std::move(graph)), backendLock_(lock) {}

bool CanonicalRuntimeHost::uniqueOwnershipSatisfied() const noexcept {
    return SessionBootstrapValidator::uniqueOwnershipSatisfied(graph_.snapshot.instanceCounters);
}

} // namespace dnxr
