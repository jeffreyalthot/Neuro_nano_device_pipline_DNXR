#include "neural_chip/NeuralVectorValidator.h"

#include <cmath>

namespace dnxr {
namespace {

#define DNXR_CHECK_01(value) \
    do { \
        if (!NeuralVectorValidator::isUnitInterval(value)) { \
            return {false, #value " outside [0,1] or non-finite"}; \
        } \
    } while (false)

} // namespace

bool NeuralVectorValidator::isUnitInterval(float value) {
    return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

NeuralVectorValidationResult NeuralVectorValidator::validate(const FullDiveChipFrame& frame) {
    DNXR_CHECK_01(frame.visual.globalLuminance01);
    DNXR_CHECK_01(frame.visual.contrast01);
    DNXR_CHECK_01(frame.visual.edgeMap01);
    DNXR_CHECK_01(frame.visual.motionFlow01);
    DNXR_CHECK_01(frame.visual.depthNear01);
    DNXR_CHECK_01(frame.visual.depthMid01);
    DNXR_CHECK_01(frame.visual.depthFar01);
    DNXR_CHECK_01(frame.visual.objectSaliency01);
    DNXR_CHECK_01(frame.visual.navigationCorridor01);
    DNXR_CHECK_01(frame.visual.hazardMap01);
    DNXR_CHECK_01(frame.visual.avatarBodyVisibility01);
    DNXR_CHECK_01(frame.visual.textSymbolPriority01);
    DNXR_CHECK_01(frame.visual.phospheneDensity01);
    DNXR_CHECK_01(frame.visual.phospheneStability01);
    DNXR_CHECK_01(frame.visual.flickerRisk01);
    DNXR_CHECK_01(frame.visual.visualFatigueRisk01);

    DNXR_CHECK_01(frame.auditory.loudness01);
    DNXR_CHECK_01(frame.auditory.lowBand01);
    DNXR_CHECK_01(frame.auditory.midBand01);
    DNXR_CHECK_01(frame.auditory.highBand01);
    DNXR_CHECK_01(frame.auditory.envelopeAttack01);
    DNXR_CHECK_01(frame.auditory.envelopeSustain01);
    DNXR_CHECK_01(frame.auditory.spatialLeftRight01);
    DNXR_CHECK_01(frame.auditory.spatialFrontBack01);
    DNXR_CHECK_01(frame.auditory.distance01);
    DNXR_CHECK_01(frame.auditory.speechPriority01);
    DNXR_CHECK_01(frame.auditory.dangerSoundPriority01);
    DNXR_CHECK_01(frame.auditory.startleRisk01);

    DNXR_CHECK_01(frame.touch.contactPressure01);
    DNXR_CHECK_01(frame.touch.contactArea01);
    DNXR_CHECK_01(frame.touch.vibrationFlutter01);
    DNXR_CHECK_01(frame.touch.vibrationTexture01);
    DNXR_CHECK_01(frame.touch.slip01);
    DNXR_CHECK_01(frame.touch.hardness01);
    DNXR_CHECK_01(frame.touch.softness01);
    DNXR_CHECK_01(frame.touch.warmth01);
    DNXR_CHECK_01(frame.touch.cold01);
    DNXR_CHECK_01(frame.touch.rightHandContact01);
    DNXR_CHECK_01(frame.touch.leftHandContact01);
    DNXR_CHECK_01(frame.touch.footGroundContact01);
    DNXR_CHECK_01(frame.touch.bodyCollision01);
    DNXR_CHECK_01(frame.touch.painBlocked01);
    DNXR_CHECK_01(frame.touch.burnBlocked01);
    DNXR_CHECK_01(frame.touch.crushBlocked01);
    DNXR_CHECK_01(frame.touch.shockBlocked01);
    DNXR_CHECK_01(frame.touch.somatosensoryConfidence01);

    DNXR_CHECK_01(frame.proprioception.headPose01);
    DNXR_CHECK_01(frame.proprioception.torsoPose01);
    DNXR_CHECK_01(frame.proprioception.rightArmPose01);
    DNXR_CHECK_01(frame.proprioception.leftArmPose01);
    DNXR_CHECK_01(frame.proprioception.rightLegPose01);
    DNXR_CHECK_01(frame.proprioception.leftLegPose01);
    DNXR_CHECK_01(frame.proprioception.handPoseConfidence01);
    DNXR_CHECK_01(frame.proprioception.footPoseConfidence01);
    DNXR_CHECK_01(frame.proprioception.avatarOwnership01);
    DNXR_CHECK_01(frame.proprioception.bodySchemaMismatch01);

    DNXR_CHECK_01(frame.vestibular.linearAcceleration01);
    DNXR_CHECK_01(frame.vestibular.angularVelocity01);
    DNXR_CHECK_01(frame.vestibular.gravityDirection01);
    DNXR_CHECK_01(frame.vestibular.balanceError01);
    DNXR_CHECK_01(frame.vestibular.motionSicknessRisk01);
    DNXR_CHECK_01(frame.vestibular.visualVestibularMismatch01);

    DNXR_CHECK_01(frame.motorIntent.moveForward01);
    DNXR_CHECK_01(frame.motorIntent.moveBackward01);
    DNXR_CHECK_01(frame.motorIntent.strafeLeft01);
    DNXR_CHECK_01(frame.motorIntent.strafeRight01);
    DNXR_CHECK_01(frame.motorIntent.turnYaw01);
    DNXR_CHECK_01(frame.motorIntent.lookPitch01);
    DNXR_CHECK_01(frame.motorIntent.rightArmReach01);
    DNXR_CHECK_01(frame.motorIntent.leftArmReach01);
    DNXR_CHECK_01(frame.motorIntent.rightHandGrip01);
    DNXR_CHECK_01(frame.motorIntent.leftHandGrip01);
    DNXR_CHECK_01(frame.motorIntent.gaitIntent01);
    DNXR_CHECK_01(frame.motorIntent.postureBalance01);
    DNXR_CHECK_01(frame.motorIntent.stopIntent01);
    DNXR_CHECK_01(frame.motorIntent.confidence01);
    DNXR_CHECK_01(frame.motorIntent.tremorRisk01);
    DNXR_CHECK_01(frame.motorIntent.spasmRisk01);
    DNXR_CHECK_01(frame.motorIntent.fatigueRisk01);

    DNXR_CHECK_01(frame.olfactory.odorPresence01);
    DNXR_CHECK_01(frame.olfactory.odorIdentityCluster01);
    DNXR_CHECK_01(frame.olfactory.pleasantness01);
    DNXR_CHECK_01(frame.olfactory.freshness01);
    DNXR_CHECK_01(frame.olfactory.smokeWarning01);
    DNXR_CHECK_01(frame.olfactory.chemicalWarning01);
    DNXR_CHECK_01(frame.olfactory.decay01);
    DNXR_CHECK_01(frame.olfactory.habituation01);
    DNXR_CHECK_01(frame.olfactory.respirationPhase01);
    DNXR_CHECK_01(frame.olfactory.nauseaRisk01);
    DNXR_CHECK_01(frame.olfactory.trigeminalContaminationRisk01);

    DNXR_CHECK_01(frame.gustatory.sweet01);
    DNXR_CHECK_01(frame.gustatory.salty01);
    DNXR_CHECK_01(frame.gustatory.sour01);
    DNXR_CHECK_01(frame.gustatory.bitter01);
    DNXR_CHECK_01(frame.gustatory.umami01);
    DNXR_CHECK_01(frame.gustatory.metallic01);
    DNXR_CHECK_01(frame.gustatory.mouthfeel01);
    DNXR_CHECK_01(frame.gustatory.temperature01);
    DNXR_CHECK_01(frame.gustatory.pleasantness01);
    DNXR_CHECK_01(frame.gustatory.nauseaRisk01);
    DNXR_CHECK_01(frame.gustatory.swallowingConflictRisk01);

    DNXR_CHECK_01(frame.realityAnchor.orientation01);
    DNXR_CHECK_01(frame.realityAnchor.identityContinuity01);
    DNXR_CHECK_01(frame.realityAnchor.sessionBoundary01);
    DNXR_CHECK_01(frame.realityAnchor.externalWorldRecall01);
    DNXR_CHECK_01(frame.safety.aggregateRisk01);
    DNXR_CHECK_01(frame.safety.activeCeiling01);

    if (frame.safety.activeCeiling01 > DNXR_NEURAL_CHIP_ACTIVE_CEILING_01) {
        return {false, "activeCeiling01 exceeds neural chip ceiling"};
    }
    if (!frame.safety.rawSensoryPayloadBlocked) return {false, "raw sensory payload is not blocked"};
    if (!frame.safety.simulationOnly) return {false, "frame is not simulation-only"};
    if (!frame.safety.painMappingBlocked) return {false, "pain mapping is not blocked"};

    return {true, "accepted"};
}

} // namespace dnxr
