#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <cstddef>

namespace dnxr {

struct NeuralVectorFieldView {
    const char* modality = "";
    const char* field = "";
    float value01 = 0.0f;
    bool neutralWhenDisabled = true;
};

class NeuralVectorFieldRegistry {
public:
    template <typename Visitor> static void forEachVisual(const NeuralVisualVector& v, Visitor&& visitor) {
        visitor("visual", "globalLuminance01", v.globalLuminance01, true); visitor("visual", "contrast01", v.contrast01, true); visitor("visual", "edgeMap01", v.edgeMap01, true); visitor("visual", "motionFlow01", v.motionFlow01, true); visitor("visual", "depthNear01", v.depthNear01, true); visitor("visual", "depthMid01", v.depthMid01, true); visitor("visual", "depthFar01", v.depthFar01, true); visitor("visual", "objectSaliency01", v.objectSaliency01, true); visitor("visual", "navigationCorridor01", v.navigationCorridor01, true); visitor("visual", "hazardMap01", v.hazardMap01, true); visitor("visual", "avatarBodyVisibility01", v.avatarBodyVisibility01, true); visitor("visual", "textSymbolPriority01", v.textSymbolPriority01, true); visitor("visual", "phospheneDensity01", v.phospheneDensity01, true); visitor("visual", "phospheneStability01", v.phospheneStability01, true); visitor("visual", "flickerRisk01", v.flickerRisk01, true); visitor("visual", "visualFatigueRisk01", v.visualFatigueRisk01, true);
    }
    template <typename Visitor> static void forEachAuditory(const NeuralAuditoryVector& v, Visitor&& visitor) {
        visitor("auditory", "loudness01", v.loudness01, true); visitor("auditory", "lowBand01", v.lowBand01, true); visitor("auditory", "midBand01", v.midBand01, true); visitor("auditory", "highBand01", v.highBand01, true); visitor("auditory", "envelopeAttack01", v.envelopeAttack01, true); visitor("auditory", "envelopeSustain01", v.envelopeSustain01, true); visitor("auditory", "spatialLeftRight01", v.spatialLeftRight01, false); visitor("auditory", "spatialFrontBack01", v.spatialFrontBack01, false); visitor("auditory", "distance01", v.distance01, true); visitor("auditory", "speechPriority01", v.speechPriority01, true); visitor("auditory", "dangerSoundPriority01", v.dangerSoundPriority01, true); visitor("auditory", "startleRisk01", v.startleRisk01, true);
    }
    template <typename Visitor> static void forEachTouch(const NeuralTouchVector& v, Visitor&& visitor) {
        visitor("touch", "contactPressure01", v.contactPressure01, true); visitor("touch", "contactArea01", v.contactArea01, true); visitor("touch", "vibrationFlutter01", v.vibrationFlutter01, true); visitor("touch", "vibrationTexture01", v.vibrationTexture01, true); visitor("touch", "slip01", v.slip01, true); visitor("touch", "hardness01", v.hardness01, true); visitor("touch", "softness01", v.softness01, true); visitor("touch", "warmth01", v.warmth01, true); visitor("touch", "cold01", v.cold01, true); visitor("touch", "rightHandContact01", v.rightHandContact01, true); visitor("touch", "leftHandContact01", v.leftHandContact01, true); visitor("touch", "footGroundContact01", v.footGroundContact01, true); visitor("touch", "bodyCollision01", v.bodyCollision01, true); visitor("touch", "painBlocked01", v.painBlocked01, false); visitor("touch", "burnBlocked01", v.burnBlocked01, false); visitor("touch", "crushBlocked01", v.crushBlocked01, false); visitor("touch", "shockBlocked01", v.shockBlocked01, false); visitor("touch", "somatosensoryConfidence01", v.somatosensoryConfidence01, true);
    }
    template <typename Visitor> static void forEachProprioception(const NeuralProprioceptionVector& v, Visitor&& visitor) {
        visitor("proprioception", "headPose01", v.headPose01, true); visitor("proprioception", "torsoPose01", v.torsoPose01, true); visitor("proprioception", "rightArmPose01", v.rightArmPose01, true); visitor("proprioception", "leftArmPose01", v.leftArmPose01, true); visitor("proprioception", "rightLegPose01", v.rightLegPose01, true); visitor("proprioception", "leftLegPose01", v.leftLegPose01, true); visitor("proprioception", "handPoseConfidence01", v.handPoseConfidence01, true); visitor("proprioception", "footPoseConfidence01", v.footPoseConfidence01, true); visitor("proprioception", "avatarOwnership01", v.avatarOwnership01, true); visitor("proprioception", "bodySchemaMismatch01", v.bodySchemaMismatch01, true);
    }
    template <typename Visitor> static void forEachVestibular(const NeuralVestibularVector& v, Visitor&& visitor) {
        visitor("vestibular", "linearAcceleration01", v.linearAcceleration01, true); visitor("vestibular", "angularVelocity01", v.angularVelocity01, true); visitor("vestibular", "gravityDirection01", v.gravityDirection01, false); visitor("vestibular", "balanceError01", v.balanceError01, true); visitor("vestibular", "motionSicknessRisk01", v.motionSicknessRisk01, true); visitor("vestibular", "visualVestibularMismatch01", v.visualVestibularMismatch01, true);
    }
    template <typename Visitor> static void forEachOlfactory(const NeuralOlfactoryVector& v, Visitor&& visitor) {
        visitor("olfactory", "odorPresence01", v.odorPresence01, true); visitor("olfactory", "odorIdentityCluster01", v.odorIdentityCluster01, true); visitor("olfactory", "pleasantness01", v.pleasantness01, true); visitor("olfactory", "freshness01", v.freshness01, true); visitor("olfactory", "smokeWarning01", v.smokeWarning01, true); visitor("olfactory", "chemicalWarning01", v.chemicalWarning01, true); visitor("olfactory", "decay01", v.decay01, true); visitor("olfactory", "habituation01", v.habituation01, true); visitor("olfactory", "respirationPhase01", v.respirationPhase01, true); visitor("olfactory", "nauseaRisk01", v.nauseaRisk01, true); visitor("olfactory", "trigeminalContaminationRisk01", v.trigeminalContaminationRisk01, true);
    }
    template <typename Visitor> static void forEachGustatory(const NeuralGustatoryVector& v, Visitor&& visitor) {
        visitor("gustatory", "sweet01", v.sweet01, true); visitor("gustatory", "salty01", v.salty01, true); visitor("gustatory", "sour01", v.sour01, true); visitor("gustatory", "bitter01", v.bitter01, true); visitor("gustatory", "umami01", v.umami01, true); visitor("gustatory", "metallic01", v.metallic01, true); visitor("gustatory", "mouthfeel01", v.mouthfeel01, true); visitor("gustatory", "temperature01", v.temperature01, true); visitor("gustatory", "pleasantness01", v.pleasantness01, true); visitor("gustatory", "nauseaRisk01", v.nauseaRisk01, true); visitor("gustatory", "swallowingConflictRisk01", v.swallowingConflictRisk01, true);
    }

    template <typename Visitor> static void forEachMotorIntent(const NeuralMotorIntentVector& v, Visitor&& visitor) {
        visitor("motorIntent", "moveForward01", v.moveForward01, true); visitor("motorIntent", "moveBackward01", v.moveBackward01, true); visitor("motorIntent", "strafeLeft01", v.strafeLeft01, true); visitor("motorIntent", "strafeRight01", v.strafeRight01, true); visitor("motorIntent", "turnYaw01", v.turnYaw01, true); visitor("motorIntent", "lookPitch01", v.lookPitch01, true); visitor("motorIntent", "rightArmReach01", v.rightArmReach01, true); visitor("motorIntent", "leftArmReach01", v.leftArmReach01, true); visitor("motorIntent", "rightHandGrip01", v.rightHandGrip01, true); visitor("motorIntent", "leftHandGrip01", v.leftHandGrip01, true); visitor("motorIntent", "gaitIntent01", v.gaitIntent01, true); visitor("motorIntent", "postureBalance01", v.postureBalance01, true); visitor("motorIntent", "stopIntent01", v.stopIntent01, false); visitor("motorIntent", "confidence01", v.confidence01, true); visitor("motorIntent", "tremorRisk01", v.tremorRisk01, true); visitor("motorIntent", "spasmRisk01", v.spasmRisk01, true); visitor("motorIntent", "fatigueRisk01", v.fatigueRisk01, true);
    }
    template <typename Visitor> static void forEachRealityAnchor(const NeuralRealityAnchorVector& v, Visitor&& visitor) {
        visitor("realityAnchor", "orientation01", v.orientation01, false); visitor("realityAnchor", "identityContinuity01", v.identityContinuity01, false); visitor("realityAnchor", "sessionBoundary01", v.sessionBoundary01, false); visitor("realityAnchor", "externalWorldRecall01", v.externalWorldRecall01, false);
    }
    template <typename Visitor> static void forEachSafety(const NeuralSafetyVector& v, Visitor&& visitor) {
        visitor("safety", "aggregateRisk01", v.aggregateRisk01, false); visitor("safety", "activeCeiling01", v.activeCeiling01, false);
    }
    template <typename Visitor> static void forEachAllFrameFields(const FullDiveChipFrame& frame, Visitor&& visitor) {
        forEachVisual(frame.visual, visitor); forEachAuditory(frame.auditory, visitor); forEachTouch(frame.touch, visitor);
        forEachProprioception(frame.proprioception, visitor); forEachVestibular(frame.vestibular, visitor); forEachMotorIntent(frame.motorIntent, visitor);
        forEachOlfactory(frame.olfactory, visitor); forEachGustatory(frame.gustatory, visitor); forEachRealityAnchor(frame.realityAnchor, visitor); forEachSafety(frame.safety, visitor);
    }
};

} // namespace dnxr
