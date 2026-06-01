#pragma once

#include <string>

namespace dnxr {

enum class RuntimeSafetyAction {
    AllowRender,
    DegradedFallback,
    StabilityGuard,
    EmergencyStop,
    Unknown
};

enum class SafetyMode {
    Nominal,
    SimulationSafe,
    Degraded,
    Emergency,
    Unknown
};

enum class IntentAction {
    Move,
    Look,
    Grab,
    Interact,
    Navigate,
    None,
    Unknown
};

enum class RuntimeHealth {
    Nominal,
    Throttle,
    Fallback,
    EmergencyStop,
    Unknown
};

enum class RenderQualityMode {
    FullQuality,
    AdaptiveSafe,
    DegradedFallback,
    Reprojection,
    Unknown
};

enum class SignalSource {
    Simulated,
    FileReplay,
    DeviceDisabled,
    Unknown
};

std::string toString(RuntimeSafetyAction value);
std::string toString(SafetyMode value);
std::string toString(IntentAction value);
std::string toString(RuntimeHealth value);
std::string toString(RenderQualityMode value);
std::string toString(SignalSource value);

RuntimeSafetyAction parseRuntimeSafetyAction(const std::string& value);
SafetyMode parseSafetyMode(const std::string& value);
IntentAction parseIntentAction(const std::string& value);
RuntimeHealth parseRuntimeHealth(const std::string& value);
RenderQualityMode parseRenderQualityMode(const std::string& value);
SignalSource parseSignalSource(const std::string& value);

} // namespace dnxr
