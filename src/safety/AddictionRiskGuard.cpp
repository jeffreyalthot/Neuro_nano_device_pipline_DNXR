#include "AddictionRiskGuard.h"

#include <algorithm>

namespace dnxr {

namespace {
float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
}

AddictionRiskReport AddictionRiskGuard::evaluate(float sceneInstability,
                                                 float emotionalValence,
                                                 float stress,
                                                 float sessionMinutes) const {
    AddictionRiskReport r;
    r.instabilityExposure = clamp01(sceneInstability);
    r.emotionalVolatility = clamp01((1.0f - emotionalValence) * 0.55f + stress * 0.45f);
    const float durationFactor = clamp01(sessionMinutes / 60.0f);
    r.addictionRisk = clamp01(r.instabilityExposure * 0.45f + r.emotionalVolatility * 0.35f + durationFactor * 0.20f);

    if (r.addictionRisk > 0.65f) {
        r.mitigationActive = true;
        r.policyAction = "stabilize_stimulus_and_force_micro_break";
    } else if (r.addictionRisk > 0.45f) {
        r.policyAction = "reduce_instability_and_cap_narrative_pressure";
    } else {
        r.policyAction = "nominal";
    }

    return r;
}

} // namespace dnxr
