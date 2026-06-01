#include "RuntimeTypes.h"

namespace dnxr {

std::string toString(RuntimeSafetyAction value) {
    switch (value) {
        case RuntimeSafetyAction::AllowRender: return "allow_render";
        case RuntimeSafetyAction::DegradedFallback: return "degraded_fallback";
        case RuntimeSafetyAction::StabilityGuard: return "stability_guard";
        case RuntimeSafetyAction::EmergencyStop: return "emergency_stop";
        case RuntimeSafetyAction::Unknown: return "unknown";
    }
    return "unknown";
}

std::string toString(SafetyMode value) {
    switch (value) {
        case SafetyMode::Nominal: return "nominal";
        case SafetyMode::SimulationSafe: return "simulation_safe";
        case SafetyMode::Degraded: return "degraded";
        case SafetyMode::Emergency: return "emergency";
        case SafetyMode::Unknown: return "unknown";
    }
    return "unknown";
}

std::string toString(IntentAction value) {
    switch (value) {
        case IntentAction::Move: return "xr.move";
        case IntentAction::Look: return "xr.look";
        case IntentAction::Grab: return "xr.grab";
        case IntentAction::Interact: return "xr.interact";
        case IntentAction::Navigate: return "xr.navigate";
        case IntentAction::None: return "none";
        case IntentAction::Unknown: return "unknown";
    }
    return "unknown";
}

std::string toString(RuntimeHealth value) {
    switch (value) {
        case RuntimeHealth::Nominal: return "nominal";
        case RuntimeHealth::Throttle: return "throttle";
        case RuntimeHealth::Fallback: return "fallback";
        case RuntimeHealth::EmergencyStop: return "emergency_stop";
        case RuntimeHealth::Unknown: return "unknown";
    }
    return "unknown";
}

std::string toString(RenderQualityMode value) {
    switch (value) {
        case RenderQualityMode::FullQuality: return "full_quality";
        case RenderQualityMode::AdaptiveSafe: return "adaptive_safe";
        case RenderQualityMode::DegradedFallback: return "degraded_fallback";
        case RenderQualityMode::Reprojection: return "reprojection";
        case RenderQualityMode::Unknown: return "unknown";
    }
    return "unknown";
}

std::string toString(SignalSource value) {
    switch (value) {
        case SignalSource::Simulated: return "simulated";
        case SignalSource::FileReplay: return "file_replay";
        case SignalSource::DeviceDisabled: return "device_disabled";
        case SignalSource::Unknown: return "unknown";
    }
    return "unknown";
}

RuntimeSafetyAction parseRuntimeSafetyAction(const std::string& value) {
    if (value == "allow_render") return RuntimeSafetyAction::AllowRender;
    if (value == "degraded_fallback") return RuntimeSafetyAction::DegradedFallback;
    if (value == "stability_guard") return RuntimeSafetyAction::StabilityGuard;
    if (value == "emergency_stop") return RuntimeSafetyAction::EmergencyStop;
    return RuntimeSafetyAction::Unknown;
}

SafetyMode parseSafetyMode(const std::string& value) {
    if (value == "nominal") return SafetyMode::Nominal;
    if (value == "simulation_safe") return SafetyMode::SimulationSafe;
    if (value == "degraded") return SafetyMode::Degraded;
    if (value == "emergency") return SafetyMode::Emergency;
    return SafetyMode::Unknown;
}

IntentAction parseIntentAction(const std::string& value) {
    if (value == "xr.move") return IntentAction::Move;
    if (value == "xr.look") return IntentAction::Look;
    if (value == "xr.grab") return IntentAction::Grab;
    if (value == "xr.interact") return IntentAction::Interact;
    if (value == "xr.navigate") return IntentAction::Navigate;
    if (value == "none") return IntentAction::None;
    return IntentAction::Unknown;
}

RuntimeHealth parseRuntimeHealth(const std::string& value) {
    if (value == "nominal") return RuntimeHealth::Nominal;
    if (value == "throttle") return RuntimeHealth::Throttle;
    if (value == "fallback") return RuntimeHealth::Fallback;
    if (value == "emergency_stop") return RuntimeHealth::EmergencyStop;
    return RuntimeHealth::Unknown;
}

RenderQualityMode parseRenderQualityMode(const std::string& value) {
    if (value == "full_quality") return RenderQualityMode::FullQuality;
    if (value == "adaptive_safe") return RenderQualityMode::AdaptiveSafe;
    if (value == "degraded_fallback") return RenderQualityMode::DegradedFallback;
    if (value == "reprojection") return RenderQualityMode::Reprojection;
    return RenderQualityMode::Unknown;
}

SignalSource parseSignalSource(const std::string& value) {
    if (value == "simulated") return SignalSource::Simulated;
    if (value == "file_replay") return SignalSource::FileReplay;
    if (value == "device_disabled") return SignalSource::DeviceDisabled;
    return SignalSource::Unknown;
}

} // namespace dnxr
