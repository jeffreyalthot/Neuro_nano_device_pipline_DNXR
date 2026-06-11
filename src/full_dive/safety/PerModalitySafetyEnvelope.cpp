#include "full_dive/safety/PerModalitySafetyEnvelope.h"

#include <algorithm>

namespace dnxr {

float PerModalitySafetyEnvelope::clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float PerModalitySafetyEnvelope::capActive(float value, float ceiling) {
    return std::min(clamp01(value), clamp01(ceiling));
}

FullDiveChipFrame PerModalitySafetyEnvelope::apply(const FullDiveChipFrame& frame) const {
    FullDiveChipFrame out = frame;
    const float ceiling = out.safety.activeCeiling01 <= 0.0f ? DNXR_NEURAL_CHIP_ACTIVE_CEILING_01 : out.safety.activeCeiling01;

    out.visual.globalLuminance01 = capActive(out.visual.globalLuminance01, ceiling);
    out.visual.contrast01 = capActive(out.visual.contrast01, ceiling);
    out.visual.edgeMap01 = capActive(out.visual.edgeMap01, ceiling);
    out.visual.motionFlow01 = capActive(out.visual.motionFlow01, ceiling);
    out.visual.depthNear01 = capActive(out.visual.depthNear01, ceiling);
    out.visual.depthMid01 = capActive(out.visual.depthMid01, ceiling);
    out.visual.depthFar01 = capActive(out.visual.depthFar01, ceiling);
    out.visual.objectSaliency01 = capActive(out.visual.objectSaliency01, ceiling);
    out.visual.navigationCorridor01 = capActive(out.visual.navigationCorridor01, ceiling);
    out.visual.hazardMap01 = capActive(out.visual.hazardMap01, ceiling);
    out.visual.avatarBodyVisibility01 = capActive(out.visual.avatarBodyVisibility01, ceiling);
    out.visual.textSymbolPriority01 = capActive(out.visual.textSymbolPriority01, ceiling);
    out.visual.phospheneDensity01 = capActive(out.visual.phospheneDensity01, ceiling);
    out.visual.phospheneStability01 = capActive(out.visual.phospheneStability01, ceiling);
    out.visual.flickerRisk01 = clamp01(out.visual.flickerRisk01);
    out.visual.visualFatigueRisk01 = clamp01(out.visual.visualFatigueRisk01);

    out.auditory.loudness01 = capActive(out.auditory.loudness01, ceiling);
    out.auditory.lowBand01 = capActive(out.auditory.lowBand01, ceiling);
    out.auditory.midBand01 = capActive(out.auditory.midBand01, ceiling);
    out.auditory.highBand01 = capActive(out.auditory.highBand01, ceiling);
    out.auditory.envelopeAttack01 = capActive(out.auditory.envelopeAttack01, ceiling);
    out.auditory.envelopeSustain01 = capActive(out.auditory.envelopeSustain01, ceiling);
    out.auditory.spatialLeftRight01 = clamp01(out.auditory.spatialLeftRight01);
    out.auditory.spatialFrontBack01 = clamp01(out.auditory.spatialFrontBack01);
    out.auditory.distance01 = clamp01(out.auditory.distance01);
    out.auditory.speechPriority01 = capActive(out.auditory.speechPriority01, ceiling);
    out.auditory.dangerSoundPriority01 = capActive(out.auditory.dangerSoundPriority01, ceiling);
    out.auditory.startleRisk01 = clamp01(out.auditory.startleRisk01);

    out.touch.contactPressure01 = capActive(out.touch.contactPressure01, ceiling);
    out.touch.contactArea01 = capActive(out.touch.contactArea01, ceiling);
    out.touch.vibrationFlutter01 = capActive(out.touch.vibrationFlutter01, ceiling);
    out.touch.vibrationTexture01 = capActive(out.touch.vibrationTexture01, ceiling);
    out.touch.slip01 = capActive(out.touch.slip01, ceiling);
    out.touch.hardness01 = capActive(out.touch.hardness01, ceiling);
    out.touch.softness01 = capActive(out.touch.softness01, ceiling);
    out.touch.warmth01 = capActive(out.touch.warmth01, ceiling);
    out.touch.cold01 = capActive(out.touch.cold01, ceiling);
    out.touch.painBlocked01 = 1.0f;
    out.touch.burnBlocked01 = 1.0f;
    out.touch.crushBlocked01 = 1.0f;
    out.touch.shockBlocked01 = 1.0f;

    out.vestibular.motionSicknessRisk01 = clamp01(out.vestibular.motionSicknessRisk01);
    out.vestibular.visualVestibularMismatch01 = clamp01(out.vestibular.visualVestibularMismatch01);
    out.olfactory.nauseaRisk01 = clamp01(out.olfactory.nauseaRisk01);
    out.olfactory.trigeminalContaminationRisk01 = clamp01(out.olfactory.trigeminalContaminationRisk01);
    out.gustatory.nauseaRisk01 = clamp01(out.gustatory.nauseaRisk01);
    out.gustatory.swallowingConflictRisk01 = clamp01(out.gustatory.swallowingConflictRisk01);

    out.safety.rawSensoryPayloadBlocked = true;
    out.safety.painMappingBlocked = true;
    out.safety.activeCeiling01 = clamp01(ceiling);
    return out;
}

bool PerModalitySafetyEnvelope::validates(const FullDiveChipFrame& frame) const {
    return frame.safety.rawSensoryPayloadBlocked &&
           frame.safety.painMappingBlocked &&
           frame.safety.patientStopSignalAlive &&
           frame.safety.emergencyCutoffAvailable &&
           frame.realityAnchor.required &&
           frame.touch.painBlocked01 >= 1.0f &&
           frame.touch.burnBlocked01 >= 1.0f &&
           frame.touch.crushBlocked01 >= 1.0f &&
           frame.touch.shockBlocked01 >= 1.0f &&
           frame.gustatory.swallowingConflictRisk01 < 0.80f;
}

} // namespace dnxr
