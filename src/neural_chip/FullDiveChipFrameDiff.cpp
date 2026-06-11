#include "neural_chip/FullDiveChipFrameDiff.h"

#include <cmath>
#include <cstring>

namespace dnxr {
namespace {
const char* modalityFromPath(const char* path) {
    if (!path) return "";
#define DNXR_PREFIX(name) if (std::strncmp(path, name ".", sizeof(name)) == 0) return name
    DNXR_PREFIX("visual");
    DNXR_PREFIX("auditory");
    DNXR_PREFIX("touch");
    DNXR_PREFIX("proprioception");
    DNXR_PREFIX("vestibular");
    DNXR_PREFIX("motorIntent");
    DNXR_PREFIX("olfactory");
    DNXR_PREFIX("gustatory");
    DNXR_PREFIX("realityAnchor");
    DNXR_PREFIX("safety");
#undef DNXR_PREFIX
    return "";
}

void add(std::vector<FrameFieldDelta>& deltas, const char* path, float before, float after, float eps) {
    const float delta = after - before;
    if (std::fabs(delta) > eps) deltas.push_back({modalityFromPath(path), path, before, after, delta});
}
} // namespace

std::vector<FrameFieldDelta> FullDiveChipFrameDiff::compare(const FullDiveChipFrame& before, const FullDiveChipFrame& after, float epsilon01) {
    std::vector<FrameFieldDelta> deltas;
#define DNXR_ADD(path, field) add(deltas, path, before.field, after.field, epsilon01)
    DNXR_ADD("visual.globalLuminance01", visual.globalLuminance01); DNXR_ADD("visual.contrast01", visual.contrast01); DNXR_ADD("visual.edgeMap01", visual.edgeMap01); DNXR_ADD("visual.motionFlow01", visual.motionFlow01); DNXR_ADD("visual.depthNear01", visual.depthNear01); DNXR_ADD("visual.depthMid01", visual.depthMid01); DNXR_ADD("visual.depthFar01", visual.depthFar01); DNXR_ADD("visual.objectSaliency01", visual.objectSaliency01); DNXR_ADD("visual.navigationCorridor01", visual.navigationCorridor01); DNXR_ADD("visual.hazardMap01", visual.hazardMap01); DNXR_ADD("visual.avatarBodyVisibility01", visual.avatarBodyVisibility01); DNXR_ADD("visual.textSymbolPriority01", visual.textSymbolPriority01); DNXR_ADD("visual.phospheneDensity01", visual.phospheneDensity01); DNXR_ADD("visual.phospheneStability01", visual.phospheneStability01); DNXR_ADD("visual.flickerRisk01", visual.flickerRisk01); DNXR_ADD("visual.visualFatigueRisk01", visual.visualFatigueRisk01);
    DNXR_ADD("auditory.loudness01", auditory.loudness01); DNXR_ADD("auditory.lowBand01", auditory.lowBand01); DNXR_ADD("auditory.midBand01", auditory.midBand01); DNXR_ADD("auditory.highBand01", auditory.highBand01); DNXR_ADD("auditory.envelopeAttack01", auditory.envelopeAttack01); DNXR_ADD("auditory.envelopeSustain01", auditory.envelopeSustain01); DNXR_ADD("auditory.spatialLeftRight01", auditory.spatialLeftRight01); DNXR_ADD("auditory.spatialFrontBack01", auditory.spatialFrontBack01); DNXR_ADD("auditory.distance01", auditory.distance01); DNXR_ADD("auditory.speechPriority01", auditory.speechPriority01); DNXR_ADD("auditory.dangerSoundPriority01", auditory.dangerSoundPriority01); DNXR_ADD("auditory.startleRisk01", auditory.startleRisk01);
    DNXR_ADD("touch.contactPressure01", touch.contactPressure01); DNXR_ADD("touch.contactArea01", touch.contactArea01); DNXR_ADD("touch.vibrationFlutter01", touch.vibrationFlutter01); DNXR_ADD("touch.vibrationTexture01", touch.vibrationTexture01); DNXR_ADD("touch.slip01", touch.slip01); DNXR_ADD("touch.hardness01", touch.hardness01); DNXR_ADD("touch.softness01", touch.softness01); DNXR_ADD("touch.warmth01", touch.warmth01); DNXR_ADD("touch.cold01", touch.cold01); DNXR_ADD("touch.rightHandContact01", touch.rightHandContact01); DNXR_ADD("touch.leftHandContact01", touch.leftHandContact01); DNXR_ADD("touch.footGroundContact01", touch.footGroundContact01); DNXR_ADD("touch.bodyCollision01", touch.bodyCollision01); DNXR_ADD("touch.painBlocked01", touch.painBlocked01); DNXR_ADD("touch.burnBlocked01", touch.burnBlocked01); DNXR_ADD("touch.crushBlocked01", touch.crushBlocked01); DNXR_ADD("touch.shockBlocked01", touch.shockBlocked01); DNXR_ADD("touch.somatosensoryConfidence01", touch.somatosensoryConfidence01);
    DNXR_ADD("proprioception.headPose01", proprioception.headPose01); DNXR_ADD("proprioception.torsoPose01", proprioception.torsoPose01); DNXR_ADD("proprioception.rightArmPose01", proprioception.rightArmPose01); DNXR_ADD("proprioception.leftArmPose01", proprioception.leftArmPose01); DNXR_ADD("proprioception.rightLegPose01", proprioception.rightLegPose01); DNXR_ADD("proprioception.leftLegPose01", proprioception.leftLegPose01); DNXR_ADD("proprioception.handPoseConfidence01", proprioception.handPoseConfidence01); DNXR_ADD("proprioception.footPoseConfidence01", proprioception.footPoseConfidence01); DNXR_ADD("proprioception.avatarOwnership01", proprioception.avatarOwnership01); DNXR_ADD("proprioception.bodySchemaMismatch01", proprioception.bodySchemaMismatch01);
    DNXR_ADD("vestibular.linearAcceleration01", vestibular.linearAcceleration01); DNXR_ADD("vestibular.angularVelocity01", vestibular.angularVelocity01); DNXR_ADD("vestibular.gravityDirection01", vestibular.gravityDirection01); DNXR_ADD("vestibular.balanceError01", vestibular.balanceError01); DNXR_ADD("vestibular.motionSicknessRisk01", vestibular.motionSicknessRisk01); DNXR_ADD("vestibular.visualVestibularMismatch01", vestibular.visualVestibularMismatch01);
    DNXR_ADD("motorIntent.moveForward01", motorIntent.moveForward01); DNXR_ADD("motorIntent.moveBackward01", motorIntent.moveBackward01); DNXR_ADD("motorIntent.strafeLeft01", motorIntent.strafeLeft01); DNXR_ADD("motorIntent.strafeRight01", motorIntent.strafeRight01); DNXR_ADD("motorIntent.turnYaw01", motorIntent.turnYaw01); DNXR_ADD("motorIntent.lookPitch01", motorIntent.lookPitch01); DNXR_ADD("motorIntent.rightArmReach01", motorIntent.rightArmReach01); DNXR_ADD("motorIntent.leftArmReach01", motorIntent.leftArmReach01); DNXR_ADD("motorIntent.rightHandGrip01", motorIntent.rightHandGrip01); DNXR_ADD("motorIntent.leftHandGrip01", motorIntent.leftHandGrip01); DNXR_ADD("motorIntent.gaitIntent01", motorIntent.gaitIntent01); DNXR_ADD("motorIntent.postureBalance01", motorIntent.postureBalance01); DNXR_ADD("motorIntent.stopIntent01", motorIntent.stopIntent01); DNXR_ADD("motorIntent.confidence01", motorIntent.confidence01); DNXR_ADD("motorIntent.tremorRisk01", motorIntent.tremorRisk01); DNXR_ADD("motorIntent.spasmRisk01", motorIntent.spasmRisk01); DNXR_ADD("motorIntent.fatigueRisk01", motorIntent.fatigueRisk01);
    DNXR_ADD("olfactory.odorPresence01", olfactory.odorPresence01); DNXR_ADD("olfactory.odorIdentityCluster01", olfactory.odorIdentityCluster01); DNXR_ADD("olfactory.pleasantness01", olfactory.pleasantness01); DNXR_ADD("olfactory.freshness01", olfactory.freshness01); DNXR_ADD("olfactory.smokeWarning01", olfactory.smokeWarning01); DNXR_ADD("olfactory.chemicalWarning01", olfactory.chemicalWarning01); DNXR_ADD("olfactory.decay01", olfactory.decay01); DNXR_ADD("olfactory.habituation01", olfactory.habituation01); DNXR_ADD("olfactory.respirationPhase01", olfactory.respirationPhase01); DNXR_ADD("olfactory.nauseaRisk01", olfactory.nauseaRisk01); DNXR_ADD("olfactory.trigeminalContaminationRisk01", olfactory.trigeminalContaminationRisk01);
    DNXR_ADD("gustatory.sweet01", gustatory.sweet01); DNXR_ADD("gustatory.salty01", gustatory.salty01); DNXR_ADD("gustatory.sour01", gustatory.sour01); DNXR_ADD("gustatory.bitter01", gustatory.bitter01); DNXR_ADD("gustatory.umami01", gustatory.umami01); DNXR_ADD("gustatory.metallic01", gustatory.metallic01); DNXR_ADD("gustatory.mouthfeel01", gustatory.mouthfeel01); DNXR_ADD("gustatory.temperature01", gustatory.temperature01); DNXR_ADD("gustatory.pleasantness01", gustatory.pleasantness01); DNXR_ADD("gustatory.nauseaRisk01", gustatory.nauseaRisk01); DNXR_ADD("gustatory.swallowingConflictRisk01", gustatory.swallowingConflictRisk01);
    DNXR_ADD("realityAnchor.orientation01", realityAnchor.orientation01); DNXR_ADD("realityAnchor.identityContinuity01", realityAnchor.identityContinuity01); DNXR_ADD("realityAnchor.sessionBoundary01", realityAnchor.sessionBoundary01); DNXR_ADD("realityAnchor.externalWorldRecall01", realityAnchor.externalWorldRecall01);
    DNXR_ADD("safety.aggregateRisk01", safety.aggregateRisk01); DNXR_ADD("safety.activeCeiling01", safety.activeCeiling01);
#undef DNXR_ADD
    return deltas;
}

} // namespace dnxr
