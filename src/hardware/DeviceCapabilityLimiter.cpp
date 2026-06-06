#include "DeviceCapabilityLimiter.h"

namespace dnxr {
namespace {
template <typename Layer>
void suppress(Layer& layer) { layer.channels.clear(); }
}

DeviceCapabilityLimitResult DeviceCapabilityLimiter::apply(const FullDiveNeuralFrame& frame,
                                                           const ChipCapabilities& capabilities,
                                                           float runtimeRateHz) const {
    DeviceCapabilityLimitResult result;
    result.frame = frame;
    if (!capabilities.supportsHardwareCutoff) {
        result.allowed = false;
        result.reason = "hardware_cutoff_missing";
        result.frame.safety.emergencyCutoffAvailable = false;
        return result;
    }
    if (!capabilities.supportsClosedLoopFeedback) {
        result.allowed = false;
        result.reason = "closed_loop_feedback_not_supported";
        return result;
    }
    if (capabilities.maxSafeRateHz > 0.0f && runtimeRateHz > capabilities.maxSafeRateHz) {
        result.allowed = false;
        result.reason = "runtime_rate_exceeds_device_capability";
        return result;
    }
    if (!capabilities.canVisual) suppress(result.frame.visual);
    if (!capabilities.canAuditory) suppress(result.frame.auditory);
    if (!capabilities.canHaptic) suppress(result.frame.somatosensory);
    if (!capabilities.canProprioceptive) suppress(result.frame.proprioception);
    if (!capabilities.canVestibular) suppress(result.frame.vestibular);
    if (!capabilities.canOlfactory) suppress(result.frame.smellTaste.olfactory);
    if (!capabilities.canGustatory) suppress(result.frame.smellTaste.gustatory);
    return result;
}

} // namespace dnxr
