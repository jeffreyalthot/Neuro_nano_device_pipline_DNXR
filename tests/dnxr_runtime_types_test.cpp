#include "../src/runtime/RuntimeTypes.h"

#include <cassert>

int main() {
    assert(dnxr::toString(dnxr::RuntimeSafetyAction::AllowRender) == "allow_render");
    assert(dnxr::parseRuntimeSafetyAction("degraded_fallback") == dnxr::RuntimeSafetyAction::DegradedFallback);
    assert(dnxr::parseRuntimeSafetyAction("typo") == dnxr::RuntimeSafetyAction::Unknown);

    assert(dnxr::toString(dnxr::IntentAction::Move) == "xr.move");
    assert(dnxr::parseIntentAction("xr.grab") == dnxr::IntentAction::Grab);
    assert(dnxr::parseIntentAction("xr.fly") == dnxr::IntentAction::Unknown);

    assert(dnxr::toString(dnxr::RuntimeHealth::Fallback) == "fallback");
    assert(dnxr::parseRuntimeHealth("emergency_stop") == dnxr::RuntimeHealth::EmergencyStop);

    assert(dnxr::toString(dnxr::RenderQualityMode::AdaptiveSafe) == "adaptive_safe");
    assert(dnxr::parseRenderQualityMode("full_quality") == dnxr::RenderQualityMode::FullQuality);

    assert(dnxr::toString(dnxr::SignalSource::FileReplay) == "file_replay");
    assert(dnxr::parseSignalSource("device_disabled") == dnxr::SignalSource::DeviceDisabled);
    assert(dnxr::parseSignalSource("hardware") == dnxr::SignalSource::Unknown);

    assert(dnxr::toString(dnxr::SafetyMode::SimulationSafe) == "simulation_safe");
    assert(dnxr::parseSafetyMode("nominal") == dnxr::SafetyMode::Nominal);

    return 0;
}
