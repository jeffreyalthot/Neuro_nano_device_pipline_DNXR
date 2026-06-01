#include "FullDiveSafetyBridge.h"

#include <algorithm>

namespace dnxr {
namespace {

SafetyAction toSafetyAction(FullDiveInterventionLevel level) {
    switch (level) {
    case FullDiveInterventionLevel::None: return SafetyAction::Continue;
    case FullDiveInterventionLevel::ReduceIntensity: return SafetyAction::ReduceIntensity;
    case FullDiveInterventionLevel::Decompress: return SafetyAction::Decompress;
    case FullDiveInterventionLevel::RealityAnchor: return SafetyAction::RealityAnchor;
    case FullDiveInterventionLevel::EmergencyStop: return SafetyAction::EmergencyStop;
    case FullDiveInterventionLevel::Lockout: return SafetyAction::Lockout;
    }
    return SafetyAction::EmergencyStop;
}

bool isMoreSevere(FullDiveInterventionLevel candidate, FullDiveInterventionLevel current) {
    return static_cast<int>(candidate) > static_cast<int>(current);
}

void raise(
    FullDiveRuntimeSnapshot& snapshot,
    FullDiveInterventionLevel level,
    float severity,
    const std::string& reason,
    const std::string& traceTag) {

    if (isMoreSevere(level, snapshot.intervention) || severity > snapshot.severity) {
        snapshot.intervention = level;
        snapshot.severity = std::clamp(severity, 0.0f, 1.0f);
        snapshot.reason = reason;
        snapshot.traceTag = traceTag;
    }
}

std::string appendFullDiveReason(const std::string& base, const std::string& reason) {
    if (reason.empty()) {
        return base;
    }
    if (base.empty()) {
        return "full_dive:" + reason;
    }
    return base + "|full_dive:" + reason;
}

} // namespace

FullDiveSafetyBridge::FullDiveSafetyBridge(FullDiveInterventionMemory memory)
    : memory_(memory) {}

FullDiveRuntimeSnapshot FullDiveSafetyBridge::evaluateInstant(const FullDiveSynchronizationEngine& engine) const {
    FullDiveRuntimeSnapshot snapshot;
    snapshot.state = engine.getState();

    if (snapshot.state.emergencyExitTriggered) {
        raise(snapshot, FullDiveInterventionLevel::EmergencyStop, 1.0f,
              "full_dive_emergency_exit", "full_dive:emergency_exit");
    }

    if (snapshot.state.neuralOverloadDetected) {
        const float severity = std::clamp(1.0f - snapshot.state.fullDiveStability, 0.0f, 1.0f);
        raise(snapshot,
              severity > 0.80f ? FullDiveInterventionLevel::EmergencyStop : FullDiveInterventionLevel::Decompress,
              severity,
              "full_dive_neural_overload",
              "full_dive:neural_overload");
    } else if (snapshot.state.fullDiveStability < 0.55f) {
        raise(snapshot, FullDiveInterventionLevel::ReduceIntensity, 0.55f - snapshot.state.fullDiveStability,
              "full_dive_stability_margin_low", "full_dive:reduce_intensity");
    }

    if (snapshot.state.consciousnessDesynchronization) {
        const float temporalSeverity = std::clamp(1.0f - snapshot.state.temporalCoherence, 0.0f, 1.0f);
        raise(snapshot,
              temporalSeverity > 0.75f ? FullDiveInterventionLevel::EmergencyStop : FullDiveInterventionLevel::RealityAnchor,
              temporalSeverity,
              "full_dive_consciousness_desynchronization",
              "full_dive:reality_anchor");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    snapshot.memory = memory_;
    return snapshot;
}

FullDiveRuntimeSnapshot FullDiveSafetyBridge::observeAndEscalate(const FullDiveRuntimeSnapshot& instantSnapshot) {
    return observeAndEscalate(instantSnapshot, std::chrono::steady_clock::now());
}

FullDiveRuntimeSnapshot FullDiveSafetyBridge::observeAndEscalate(
    const FullDiveRuntimeSnapshot& instantSnapshot,
    std::chrono::steady_clock::time_point now) {
    FullDiveRuntimeSnapshot snapshot = instantSnapshot;
    std::lock_guard<std::mutex> lock(mutex_);

    if (snapshot.safetyInterventionRequired()) {
        const bool cooldownActive = lastInterventionAt_ != std::chrono::steady_clock::time_point{} &&
            std::chrono::duration<double>(now - lastInterventionAt_).count() < memory_.cooldownSeconds;
        if (cooldownActive && snapshot.intervention == FullDiveInterventionLevel::ReduceIntensity) {
            snapshot.reason = "full_dive_reduce_intensity_cooldown";
            snapshot.traceTag = "full_dive:reduce_intensity_cooldown";
        }

        rememberLocked(snapshot.intervention, now);
        lastInterventionAt_ = now;
    }

    std::uint32_t anchorsInWindow = countWindowLocked(FullDiveInterventionLevel::RealityAnchor, now);
    std::uint32_t emergenciesInSession = memory_.emergencyStopCount;
    const bool repeatedVestibularCollapse = snapshot.state.vestibularStability < 0.20f && anchorsInWindow >= 2U;

    if (snapshot.intervention == FullDiveInterventionLevel::RealityAnchor && anchorsInWindow >= 3U) {
        raise(snapshot, FullDiveInterventionLevel::EmergencyStop, std::max(snapshot.severity, 0.95f),
              "full_dive_repeated_anchor_failure_emergency_stop", "full_dive:repeated_anchor_emergency");
        rememberLocked(FullDiveInterventionLevel::EmergencyStop, now);
        emergenciesInSession = memory_.emergencyStopCount;
        anchorsInWindow = countWindowLocked(FullDiveInterventionLevel::RealityAnchor, now);
    }

    if ((snapshot.intervention == FullDiveInterventionLevel::EmergencyStop && emergenciesInSession >= 2U) ||
        (repeatedVestibularCollapse && emergenciesInSession >= 1U)) {
        raise(snapshot, FullDiveInterventionLevel::Lockout, 1.0f,
              "full_dive_repeated_incident_lockout", "full_dive:lockout");
        rememberLocked(FullDiveInterventionLevel::Lockout, now);
    }

    snapshot.memory = memory_;
    return snapshot;
}

FullDiveRuntimeSnapshot FullDiveSafetyBridge::evaluateAndRemember(const FullDiveSynchronizationEngine& engine) {
    return observeAndEscalate(evaluateInstant(engine));
}

FullDiveRuntimeSnapshot FullDiveSafetyBridge::evaluate(const FullDiveSynchronizationEngine& engine) {
    return evaluateAndRemember(engine);
}

void FullDiveSafetyBridge::apply(const FullDiveRuntimeSnapshot& snapshot, SafetyStateMachine& stateMachine) const {
    if (!snapshot.safetyInterventionRequired()) {
        return;
    }
    if (snapshot.intervention == FullDiveInterventionLevel::Lockout) {
        stateMachine.lockout(snapshot.reason);
        return;
    }
    if (snapshot.intervention == FullDiveInterventionLevel::EmergencyStop) {
        stateMachine.latchEmergency(snapshot.reason);
        return;
    }

    SafetyDecision decision;
    decision.action = toSafetyAction(snapshot.intervention);
    decision.reason = snapshot.reason;
    decision.riskScore = snapshot.severity;
    decision.intensityScale = std::clamp(1.0f - snapshot.severity, 0.0f, 1.0f);
    decision.allowRender = snapshot.intervention != FullDiveInterventionLevel::EmergencyStop &&
                           snapshot.intervention != FullDiveInterventionLevel::Lockout;
    decision.requiresEmergencyStop = false;
    stateMachine.observeSafetyDecision(decision);
}

SafetyDecision FullDiveSafetyBridge::mergeIntoDecision(
    const FullDiveRuntimeSnapshot& snapshot,
    const SafetyDecision& baseDecision) const {

    SafetyDecision fused = baseDecision;
    if (!snapshot.safetyInterventionRequired()) {
        return fused;
    }

    fused.action = toSafetyAction(snapshot.intervention);
    fused.riskScore = std::max(fused.riskScore, std::clamp(snapshot.severity, 0.0f, 1.0f));
    fused.reason = appendFullDiveReason(fused.reason, snapshot.reason);
    fused.auditRecord = appendFullDiveReason(fused.auditRecord, snapshot.traceTag);

    switch (snapshot.intervention) {
    case FullDiveInterventionLevel::None:
        break;
    case FullDiveInterventionLevel::ReduceIntensity:
        fused.intensityScale *= std::clamp(1.0f - snapshot.severity, 0.25f, 1.0f);
        break;
    case FullDiveInterventionLevel::Decompress:
        fused.intensityScale *= std::clamp(1.0f - (snapshot.severity * 0.5f), 0.35f, 1.0f);
        break;
    case FullDiveInterventionLevel::RealityAnchor:
        fused.intensityScale *= std::clamp(1.0f - (snapshot.severity * 0.35f), 0.45f, 1.0f);
        break;
    case FullDiveInterventionLevel::EmergencyStop:
        fused.allowRender = false;
        fused.requiresEmergencyStop = true;
        fused.intensityScale = 0.0f;
        break;
    case FullDiveInterventionLevel::Lockout:
        fused.allowRender = false;
        fused.requiresEmergencyStop = true;
        fused.intensityScale = 0.0f;
        break;
    }

    fused.intensityScale = std::clamp(fused.intensityScale, 0.0f, 1.0f);
    return fused;
}

FullDiveInterventionMemory FullDiveSafetyBridge::memory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memory_;
}

void FullDiveSafetyBridge::resetMemory() {
    std::lock_guard<std::mutex> lock(mutex_);
    memory_ = FullDiveInterventionMemory{};
    incidents_.clear();
    lastInterventionAt_ = {};
}

void FullDiveSafetyBridge::rememberLocked(
    FullDiveInterventionLevel level,
    std::chrono::steady_clock::time_point now) {

    incidents_.push_back({level, now});
    const auto window = std::chrono::duration<double>(memory_.windowSeconds);
    while (!incidents_.empty() && now - incidents_.front().timestamp > window) {
        incidents_.pop_front();
    }

    switch (level) {
    case FullDiveInterventionLevel::None:
        break;
    case FullDiveInterventionLevel::ReduceIntensity:
        ++memory_.reduceIntensityCount;
        break;
    case FullDiveInterventionLevel::Decompress:
        ++memory_.decompressionCount;
        break;
    case FullDiveInterventionLevel::RealityAnchor:
        ++memory_.realityAnchorCount;
        break;
    case FullDiveInterventionLevel::EmergencyStop:
        ++memory_.emergencyStopCount;
        break;
    case FullDiveInterventionLevel::Lockout:
        ++memory_.lockoutCount;
        break;
    }
}

std::uint32_t FullDiveSafetyBridge::countWindowLocked(
    FullDiveInterventionLevel level,
    std::chrono::steady_clock::time_point now) const {

    const auto window = std::chrono::duration<double>(memory_.windowSeconds);
    return static_cast<std::uint32_t>(std::count_if(
        incidents_.begin(),
        incidents_.end(),
        [&](const InterventionIncident& incident) {
            return incident.level == level && now - incident.timestamp <= window;
        }));
}

} // namespace dnxr
