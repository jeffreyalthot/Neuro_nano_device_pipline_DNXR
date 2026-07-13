#pragma once

#include "RuntimeCompositionRoot.h"
#include "SessionBootstrapValidator.h"

#include <memory>
#include <optional>

namespace dnxr {

struct CanonicalRuntimeHostGraph {
    RuntimeComposition composition;
    SessionBootstrapSnapshot snapshot;
};

class CanonicalRuntimeHost {
public:
    static SessionBootstrapResult validateSession(const RuntimeConfig& config,
                                                  const RuntimeHardwareEvidence& evidence = RuntimeHardwareEvidence{});
    static std::optional<CanonicalRuntimeHost> create(RuntimeConfig config,
                                                      RuntimeComposition composition,
                                                      SessionBootstrapSnapshot snapshot) noexcept;

    CanonicalRuntimeHost(CanonicalRuntimeHost&&) noexcept = default;
    CanonicalRuntimeHost& operator=(CanonicalRuntimeHost&&) noexcept = delete;
    CanonicalRuntimeHost(const CanonicalRuntimeHost&) = delete;
    CanonicalRuntimeHost& operator=(const CanonicalRuntimeHost&) = delete;
    ~CanonicalRuntimeHost() = default;

    const SessionBootstrapSnapshot& snapshot() const noexcept { return graph_.snapshot; }
    const RuntimeComposition& composition() const noexcept { return graph_.composition; }
    const SessionBackendLock& backendLock() const noexcept { return backendLock_; }
    bool canReconfigureBackend(RuntimeDeploymentMode mode) const noexcept { return backendLock_.canReconfigureTo(mode); }
    bool uniqueOwnershipSatisfied() const noexcept;

private:
    explicit CanonicalRuntimeHost(CanonicalRuntimeHostGraph graph, SessionBackendLock lock) noexcept;

    CanonicalRuntimeHostGraph graph_;
    SessionBackendLock backendLock_;
};

} // namespace dnxr
