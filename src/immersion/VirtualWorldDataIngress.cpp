#include "immersion/VirtualWorldDataIngress.h"

#include <algorithm>

namespace dnxr {
namespace {
float c01(float value) { return std::clamp(value, 0.0f, 1.0f); }
} // namespace

NeuralChipOnlyWorldFrame
VirtualWorldDataIngress::ingest(const VirtualWorldIngressFrame &input) const {
  NeuralChipOnlyWorldFrame world;
  world.frameId = input.frameId;
  world.timestampMs = input.timestampMs;

  world.scene.luminance01 = c01(input.view.exposure);
  world.scene.contrast01 = c01(input.view.contrast);
  world.scene.edgeDensity01 = c01(1.0f - input.visualField.peripheralBlur);
  world.scene.motionFlow01 = c01(1.0f - input.visualField.motionComfort);
  world.scene.nearObstacle01 = c01(input.visualField.nearDepthLayer);
  world.scene.midDepthStructure01 = c01(input.visualField.midDepthLayer);
  world.scene.farDepthStructure01 = c01(input.visualField.farDepthLayer);
  world.scene.salientObject01 = c01(input.visualField.fovealRadius);
  world.scene.navigationCorridor01 = c01(input.visualField.peripheralAwareness);
  world.scene.hazard01 = c01(input.visualField.vignetteStrength);
  world.scene.avatarBody01 = c01(input.body.avatarOwnership01);
  world.scene.nearObstacleDensity01 = c01(input.visualField.nearDepthLayer);
  world.scene.walkableCorridorConfidence01 = c01(input.body.footConfidence01);
  world.scene.horizonStability01 = c01(input.visualField.retinalStability);
  world.scene.handVisibility01 = c01(input.body.handConfidence01);
  world.scene.bodyVisibility01 = c01(input.body.avatarOwnership01);

  world.audio.loudness01 = c01(input.audio.volume01);
  world.audio.lowBand01 = c01(input.audio.lowFrequency01);
  world.audio.midBand01 = c01(input.audio.midFrequency01);
  world.audio.highBand01 = c01(input.audio.highFrequency01);
  world.audio.attack01 = c01(input.audio.attack01);
  world.audio.sustain01 = c01(input.audio.sustain01);
  world.audio.leftRight01 = c01(input.audio.directionLeftRight01);
  world.audio.frontBack01 = c01(input.audio.directionFrontBack01);
  world.audio.distance01 = c01(input.audio.sourceDistance01);
  world.audio.speech01 = c01(input.audio.speechPresence01);
  world.audio.danger01 = c01(input.audio.dangerPriority01);

  world.contacts.pressure01 = c01(input.touch.pressure01);
  world.contacts.area01 = c01(input.touch.contactArea01);
  world.contacts.flutter01 = c01(input.touch.vibration01);
  world.contacts.texture01 = c01(input.touch.texture01);
  world.contacts.slip01 = c01(input.touch.slip01);
  world.contacts.hardness01 = c01(input.touch.hardness01);
  world.contacts.softness01 = c01(input.touch.softness01);
  world.contacts.warmth01 = c01(input.touch.warmth01);
  world.contacts.cold01 = c01(input.touch.cold01);
  world.contacts.rightHand01 = c01(input.touch.rightHandContact01);
  world.contacts.leftHand01 = c01(input.touch.leftHandContact01);
  world.contacts.footGround01 = c01(input.touch.footGroundContact01);
  world.contacts.bodyCollision01 = c01(input.touch.bodyCollision01);

  world.pose.head01 = c01(input.body.headPose01);
  world.pose.torso01 = c01(input.body.torsoPose01);
  world.pose.rightArm01 = c01(input.body.rightArmPose01);
  world.pose.leftArm01 = c01(input.body.leftArmPose01);
  world.pose.rightLeg01 = c01(input.body.rightLegPose01);
  world.pose.leftLeg01 = c01(input.body.leftLegPose01);
  world.pose.handConfidence01 = c01(input.body.handConfidence01);
  world.pose.footConfidence01 = c01(input.body.footConfidence01);
  world.pose.ownership01 = c01(input.body.avatarOwnership01);
  world.pose.mismatch01 = c01(input.body.bodyMismatch01);

  world.motion.linearAcceleration01 = c01(input.vestibular.linearAcceleration01);
  world.motion.angularVelocity01 = c01(input.vestibular.angularVelocity01);
  world.motion.gravityDirection01 = c01(input.vestibular.gravityDirection01);
  world.motion.balanceError01 = c01(input.vestibular.balanceError01);
  world.motion.sicknessRisk01 = c01(input.vestibular.motionSicknessRisk01);
  world.motion.visualVestibularMismatch01 =
      c01(input.vestibular.visualVestibularMismatch01);

  world.odor.presence01 = c01(input.chemical.odorPresence01);
  world.odor.identityCluster01 = c01(input.chemical.odorIdentity01);
  world.odor.pleasantness01 = c01(input.chemical.odorPleasantness01);
  world.odor.freshness01 = c01(input.chemical.freshness01);
  world.odor.smoke01 = c01(input.chemical.smokeWarning01);
  world.odor.chemical01 = c01(input.chemical.chemicalWarning01);
  world.odor.decay01 = c01(input.chemical.decay01);

  world.taste.sweet01 = c01(input.chemical.sweet01);
  world.taste.salty01 = c01(input.chemical.salty01);
  world.taste.sour01 = c01(input.chemical.sour01);
  world.taste.bitter01 = c01(input.chemical.bitter01);
  world.taste.umami01 = c01(input.chemical.umami01);
  world.taste.mouthfeel01 = c01(input.chemical.mouthfeel01);

  world.realityAnchor = input.realityAnchor;
  world.realityAnchor.orientation01 = c01(world.realityAnchor.orientation01);
  world.realityAnchor.identityContinuity01 =
      c01(world.realityAnchor.identityContinuity01);
  world.realityAnchor.sessionBoundary01 =
      c01(world.realityAnchor.sessionBoundary01);
  world.realityAnchor.externalWorldRecall01 =
      c01(world.realityAnchor.externalWorldRecall01);

  world.patientMap = PatientSpecificNeuralMap::simulationDefaults();
  world.latencyBudget.visualLatencyMs = 16.67f;
  world.latencyBudget.auditoryLatencyMs = 16.67f;
  world.latencyBudget.touchLatencyMs = 16.67f;
  world.latencyBudget.proprioceptionLatencyMs = 16.67f;
  world.latencyBudget.vestibularLatencyMs = 16.67f;
  world.latencyBudget.motorLatencyMs = 16.67f;
  return world;
}

} // namespace dnxr
