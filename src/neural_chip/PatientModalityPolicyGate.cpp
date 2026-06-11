#include "neural_chip/PatientModalityPolicyGate.h"

#include <algorithm>

namespace dnxr {

float PatientModalityPolicyGate::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

float PatientModalityPolicyGate::modalityScale(const PatientModalityProfile& profile) {
    if (!profile.enabled || profile.maturity == NeuralChannelMaturity::Disabled) return 0.0f;
    float ceiling = std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01, clamp01(profile.maxAllowed01));
    if (profile.maturity == NeuralChannelMaturity::Experimental) ceiling = std::min(ceiling, 0.25f);
    if (profile.maturity == NeuralChannelMaturity::ResearchOnly) ceiling = std::min(ceiling, 0.50f);
    return clamp01(ceiling * clamp01(profile.confidence01));
}

#define DNXR_SCALE(field) value.field = clamp01(value.field * gain)
void PatientModalityPolicyGate::scale(NeuralVisualVector& value, float gain) { DNXR_SCALE(globalLuminance01); DNXR_SCALE(contrast01); DNXR_SCALE(edgeMap01); DNXR_SCALE(motionFlow01); DNXR_SCALE(depthNear01); DNXR_SCALE(depthMid01); DNXR_SCALE(depthFar01); DNXR_SCALE(objectSaliency01); DNXR_SCALE(navigationCorridor01); DNXR_SCALE(hazardMap01); DNXR_SCALE(avatarBodyVisibility01); DNXR_SCALE(textSymbolPriority01); DNXR_SCALE(phospheneDensity01); DNXR_SCALE(phospheneStability01); DNXR_SCALE(flickerRisk01); DNXR_SCALE(visualFatigueRisk01); }
void PatientModalityPolicyGate::scale(NeuralAuditoryVector& value, float gain) { DNXR_SCALE(loudness01); DNXR_SCALE(lowBand01); DNXR_SCALE(midBand01); DNXR_SCALE(highBand01); DNXR_SCALE(envelopeAttack01); DNXR_SCALE(envelopeSustain01); value.spatialLeftRight01 = 0.5f + (value.spatialLeftRight01 - 0.5f) * gain; value.spatialFrontBack01 = 0.5f + (value.spatialFrontBack01 - 0.5f) * gain; DNXR_SCALE(distance01); DNXR_SCALE(speechPriority01); DNXR_SCALE(dangerSoundPriority01); DNXR_SCALE(startleRisk01); }
void PatientModalityPolicyGate::scale(NeuralTouchVector& value, float gain) { DNXR_SCALE(contactPressure01); DNXR_SCALE(contactArea01); DNXR_SCALE(vibrationFlutter01); DNXR_SCALE(vibrationTexture01); DNXR_SCALE(slip01); DNXR_SCALE(hardness01); DNXR_SCALE(softness01); DNXR_SCALE(warmth01); DNXR_SCALE(cold01); DNXR_SCALE(rightHandContact01); DNXR_SCALE(leftHandContact01); DNXR_SCALE(footGroundContact01); DNXR_SCALE(bodyCollision01); value.painBlocked01=1.0f; value.burnBlocked01=1.0f; value.crushBlocked01=1.0f; value.shockBlocked01=1.0f; DNXR_SCALE(somatosensoryConfidence01); }
void PatientModalityPolicyGate::scale(NeuralProprioceptionVector& value, float gain) { DNXR_SCALE(headPose01); DNXR_SCALE(torsoPose01); DNXR_SCALE(rightArmPose01); DNXR_SCALE(leftArmPose01); DNXR_SCALE(rightLegPose01); DNXR_SCALE(leftLegPose01); DNXR_SCALE(handPoseConfidence01); DNXR_SCALE(footPoseConfidence01); DNXR_SCALE(avatarOwnership01); DNXR_SCALE(bodySchemaMismatch01); }
void PatientModalityPolicyGate::scale(NeuralVestibularVector& value, float gain) { DNXR_SCALE(linearAcceleration01); DNXR_SCALE(angularVelocity01); value.gravityDirection01 = 0.5f + (value.gravityDirection01 - 0.5f) * gain; DNXR_SCALE(balanceError01); DNXR_SCALE(motionSicknessRisk01); DNXR_SCALE(visualVestibularMismatch01); }
void PatientModalityPolicyGate::scale(NeuralMotorIntentVector& value, float gain) { DNXR_SCALE(moveForward01); DNXR_SCALE(moveBackward01); DNXR_SCALE(strafeLeft01); DNXR_SCALE(strafeRight01); DNXR_SCALE(turnYaw01); DNXR_SCALE(lookPitch01); DNXR_SCALE(rightArmReach01); DNXR_SCALE(leftArmReach01); DNXR_SCALE(rightHandGrip01); DNXR_SCALE(leftHandGrip01); DNXR_SCALE(gaitIntent01); DNXR_SCALE(postureBalance01); value.stopIntent01 = gain > 0.0f ? clamp01(value.stopIntent01) : 1.0f; DNXR_SCALE(confidence01); DNXR_SCALE(tremorRisk01); DNXR_SCALE(spasmRisk01); DNXR_SCALE(fatigueRisk01); }
void PatientModalityPolicyGate::scale(NeuralOlfactoryVector& value, float gain) { DNXR_SCALE(odorPresence01); DNXR_SCALE(odorIdentityCluster01); DNXR_SCALE(pleasantness01); DNXR_SCALE(freshness01); DNXR_SCALE(smokeWarning01); DNXR_SCALE(chemicalWarning01); DNXR_SCALE(decay01); DNXR_SCALE(habituation01); DNXR_SCALE(respirationPhase01); DNXR_SCALE(nauseaRisk01); DNXR_SCALE(trigeminalContaminationRisk01); }
void PatientModalityPolicyGate::scale(NeuralGustatoryVector& value, float gain) { DNXR_SCALE(sweet01); DNXR_SCALE(salty01); DNXR_SCALE(sour01); value.bitter01 = gain <= 0.25f ? 0.0f : clamp01(value.bitter01 * gain); DNXR_SCALE(umami01); DNXR_SCALE(metallic01); DNXR_SCALE(mouthfeel01); DNXR_SCALE(temperature01); DNXR_SCALE(pleasantness01); DNXR_SCALE(nauseaRisk01); DNXR_SCALE(swallowingConflictRisk01); }
#undef DNXR_SCALE

FullDiveChipFrame PatientModalityPolicyGate::apply(const FullDiveChipFrame& frame, const PatientSpecificNeuralMap& map) const {
    FullDiveChipFrame out = frame;
    scale(out.visual, modalityScale(map.visual));
    scale(out.auditory, modalityScale(map.auditory));
    scale(out.touch, modalityScale(map.touch));
    scale(out.proprioception, modalityScale(map.proprioception));
    scale(out.vestibular, modalityScale(map.vestibular));
    scale(out.motorIntent, modalityScale(map.motor));
    scale(out.olfactory, modalityScale(map.olfactory));
    scale(out.gustatory, modalityScale(map.gustatory));
    out.safety.activeCeiling01 = std::min(out.safety.activeCeiling01, DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    return out;
}

} // namespace dnxr
