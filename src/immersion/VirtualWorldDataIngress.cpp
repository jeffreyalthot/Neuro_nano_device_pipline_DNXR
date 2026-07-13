#include "immersion/VirtualWorldDataIngress.h"

#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace dnxr {
namespace {
float c01(float value) { return std::isfinite(value) ? std::clamp(value, 0.0f, 1.0f) : 0.0f; }

bool finite(double value) { return std::isfinite(value); }

NeuralRealityAnchorVector safeAnchor(NeuralRealityAnchorVector anchor) {
  anchor.orientation01 = c01(anchor.orientation01 == 0.0f ? 1.0f : anchor.orientation01);
  anchor.identityContinuity01 = c01(anchor.identityContinuity01 == 0.0f ? 1.0f : anchor.identityContinuity01);
  anchor.sessionBoundary01 = c01(anchor.sessionBoundary01 == 0.0f ? 1.0f : anchor.sessionBoundary01);
  anchor.externalWorldRecall01 = c01(anchor.externalWorldRecall01 == 0.0f ? 1.0f : anchor.externalWorldRecall01);
  return anchor;
}
} // namespace

VirtualWorldDataIngress::VirtualWorldDataIngress(VirtualWorldIngressPolicy policy)
    : policy_(policy) {}

CanonicalVirtualWorldFrame makeCanonicalVirtualWorldFrame(const VirtualWorldIngressFrame& input,
                                                          std::string engineProvenance) {
  CanonicalVirtualWorldFrame frame;
  frame.metadata.sequenceId = input.frameId;
  frame.metadata.engineTimestampMs = input.timestampMs;
  frame.metadata.engineProvenance = std::move(engineProvenance);
  frame.metadata.declaredSizeBytes = sizeof(input);
  frame.modalities.view = input.view;
  frame.modalities.visualField = input.visualField;
  frame.modalities.audio = input.audio;
  frame.modalities.touch = input.touch;
  frame.modalities.body = input.body;
  frame.modalities.vestibular = input.vestibular;
  frame.modalities.chemical = input.chemical;
  frame.modalities.motor = input.motor;
  frame.modalities.realityAnchor = input.realityAnchor;
  frame.modalities.chemicalPresent = input.chemical.odorPresence01 > 0.0f ||
                                      input.chemical.sweet01 > 0.0f ||
                                      input.chemical.salty01 > 0.0f ||
                                      input.chemical.sour01 > 0.0f ||
                                      input.chemical.bitter01 > 0.0f ||
                                      input.chemical.umami01 > 0.0f;
  return frame;
}

bool VirtualWorldDataIngress::authenticated(const CanonicalVirtualWorldFrame& input) const {
  if (!policy_.requireAuthentication) {
    return true;
  }
  return !input.metadata.engineProvenance.empty() &&
         !input.metadata.authenticationTag.empty() &&
         input.metadata.authenticationTag != "unsigned";
}

VirtualWorldIngressResult VirtualWorldDataIngress::rejectWithHold(std::string reason,
                                                                  std::uint64_t frameId,
                                                                  double timestampMs) const {
  VirtualWorldIngressResult result;
  result.accepted = false;
  result.holdFrame = true;
  result.reason = std::move(reason);
  result.world = holdWorld(frameId, timestampMs);
  result.usedLastSafeWorld = hasLastSafeWorld_;
  return result;
}

NeuralChipOnlyWorldFrame VirtualWorldDataIngress::holdWorld(std::uint64_t frameId, double timestampMs) const {
  NeuralChipOnlyWorldFrame hold = hasLastSafeWorld_ ? lastSafeWorld_ : NeuralChipOnlyWorldFrame{};
  hold.frameId = frameId;
  hold.timestampMs = finite(timestampMs) && timestampMs >= 0.0 ? timestampMs : 0.0;
  hold.scene.hazard01 = 0.0f;
  hold.scene.motionFlow01 = 0.0f;
  hold.audio = SpatialAudioFrame{};
  hold.contacts = AvatarContactFrame{};
  hold.motion = AvatarMotionFrame{};
  hold.odor = VirtualOdorField{};
  hold.taste = VirtualTasteFrame{};
  hold.realityAnchor = safeAnchor(hold.realityAnchor);
  return hold;
}

VirtualWorldIngressResult VirtualWorldDataIngress::ingestCanonical(const CanonicalVirtualWorldFrame& input,
                                                                   double cycleTimestampMs) {
  const std::uint64_t frameId = input.metadata.sequenceId;
  const double timestampMs = input.metadata.engineTimestampMs;
  if (input.metadata.schemaVersion != CanonicalVirtualWorldMetadata::kCurrentSchemaVersion) {
    return rejectWithHold("canonical_world_schema_version_unsupported", frameId, cycleTimestampMs);
  }
  if (frameId == 0) {
    return rejectWithHold("canonical_world_sequence_invalid", frameId, cycleTimestampMs);
  }
  if (!finite(timestampMs) || timestampMs < 0.0) {
    return rejectWithHold("canonical_world_timestamp_invalid", frameId, cycleTimestampMs);
  }
  if (input.metadata.declaredSizeBytes > policy_.maxFrameBytes) {
    return rejectWithHold("canonical_world_frame_too_large", frameId, timestampMs);
  }
  if (input.metadata.engineProvenance.empty()) {
    return rejectWithHold("canonical_world_engine_provenance_missing", frameId, timestampMs);
  }
  if (!authenticated(input)) {
    return rejectWithHold("canonical_world_authentication_failed", frameId, timestampMs);
  }
  if (lastSequenceId_ != 0 && frameId <= lastSequenceId_) {
    auto result = rejectWithHold("canonical_world_duplicate_or_replayed_sequence", frameId, timestampMs);
    result.duplicate = true;
    return result;
  }
  if (lastTimestampMs_ >= 0.0 && timestampMs <= lastTimestampMs_) {
    auto result = rejectWithHold("canonical_world_stale_timestamp", frameId, timestampMs);
    result.stale = true;
    return result;
  }
  if (finite(cycleTimestampMs)) {
    if (timestampMs + policy_.maxStaleLagMs < cycleTimestampMs) {
      auto result = rejectWithHold("canonical_world_late_for_bci_cycle", frameId, timestampMs);
      result.stale = true;
      return result;
    }
    if (timestampMs > cycleTimestampMs + policy_.maxFutureLeadMs) {
      auto result = rejectWithHold("canonical_world_ahead_of_bci_cycle", frameId, timestampMs);
      result.future = true;
      return result;
    }
  }
  if (policy_.requireClinicalNeutralScene) {
    const auto clinical = ClinicalNeutralSceneController{}.evaluate(input.clinicalFlags);
    if (!clinical.allowed) {
      return rejectWithHold(clinical.reason, frameId, timestampMs);
    }
  }
  if (!SceneRiskAnnotator{}.allowed(input.sceneRisk)) {
    return rejectWithHold("canonical_world_scene_risk_rejected", frameId, timestampMs);
  }
  if (policy_.requireBodySafety && !input.bodyContract.clinicalSafe()) {
    return rejectWithHold("canonical_world_body_contract_rejected", frameId, timestampMs);
  }
  if (input.modalities.chemicalPresent && !policy_.chemicalChannelsEnabled) {
    return rejectWithHold("canonical_world_chemical_channels_disabled", frameId, timestampMs);
  }

  NeuralChipOnlyWorldFrame world = buildWorld(input);
  const auto sanitized = WorldFrameSanitizer{}.sanitize(world, NeuralChipOnlyPipelineConfig::simulationSafeDefaults());
  if (!sanitized.accepted) {
    return rejectWithHold(sanitized.reason, frameId, timestampMs);
  }

  lastSequenceId_ = frameId;
  lastTimestampMs_ = timestampMs;
  lastSafeWorld_ = sanitized.sanitized;
  hasLastSafeWorld_ = true;

  VirtualWorldIngressResult result;
  result.accepted = true;
  result.reason = "canonical_world_ingress_accepted";
  result.world = sanitized.sanitized;
  return result;
}

NeuralChipOnlyWorldFrame VirtualWorldDataIngress::buildWorld(const CanonicalVirtualWorldFrame& input) const {
  NeuralChipOnlyWorldFrame world;
  world.frameId = input.metadata.sequenceId;
  world.timestampMs = input.metadata.engineTimestampMs;

  if (input.modalities.visualPresent) {
    world.scene.luminance01 = c01(input.modalities.view.exposure);
    world.scene.contrast01 = c01(input.modalities.view.contrast);
    world.scene.edgeDensity01 = c01(1.0f - input.modalities.visualField.peripheralBlur);
    world.scene.motionFlow01 = c01(1.0f - input.modalities.visualField.motionComfort);
    world.scene.nearObstacle01 = c01(input.modalities.visualField.nearDepthLayer);
    world.scene.midDepthStructure01 = c01(input.modalities.visualField.midDepthLayer);
    world.scene.farDepthStructure01 = c01(input.modalities.visualField.farDepthLayer);
    world.scene.salientObject01 = c01(input.modalities.visualField.fovealRadius);
    world.scene.navigationCorridor01 = c01(input.modalities.visualField.peripheralAwareness);
    world.scene.hazard01 = c01(input.modalities.visualField.vignetteStrength);
    world.scene.nearObstacleDensity01 = c01(input.modalities.visualField.nearDepthLayer);
    world.scene.horizonStability01 = c01(input.modalities.visualField.retinalStability);
  }

  if (input.modalities.audioPresent) {
    world.audio.loudness01 = c01(input.modalities.audio.volume01);
    world.audio.lowBand01 = c01(input.modalities.audio.lowFrequency01);
    world.audio.midBand01 = c01(input.modalities.audio.midFrequency01);
    world.audio.highBand01 = c01(input.modalities.audio.highFrequency01);
    world.audio.attack01 = c01(input.modalities.audio.attack01);
    world.audio.sustain01 = c01(input.modalities.audio.sustain01);
    world.audio.leftRight01 = c01(input.modalities.audio.directionLeftRight01);
    world.audio.frontBack01 = c01(input.modalities.audio.directionFrontBack01);
    world.audio.distance01 = c01(input.modalities.audio.sourceDistance01);
    world.audio.speech01 = c01(input.modalities.audio.speechPresence01);
    world.audio.danger01 = c01(input.modalities.audio.dangerPriority01);
  }

  if (input.modalities.touchPresent) {
    world.contacts.pressure01 = c01(input.modalities.touch.pressure01);
    world.contacts.area01 = c01(input.modalities.touch.contactArea01);
    world.contacts.flutter01 = c01(input.modalities.touch.vibration01);
    world.contacts.texture01 = c01(input.modalities.touch.texture01);
    world.contacts.slip01 = c01(input.modalities.touch.slip01);
    world.contacts.hardness01 = c01(input.modalities.touch.hardness01);
    world.contacts.softness01 = c01(input.modalities.touch.softness01);
    world.contacts.warmth01 = c01(input.modalities.touch.warmth01);
    world.contacts.cold01 = c01(input.modalities.touch.cold01);
    world.contacts.rightHand01 = c01(input.modalities.touch.rightHandContact01);
    world.contacts.leftHand01 = c01(input.modalities.touch.leftHandContact01);
    world.contacts.footGround01 = c01(input.modalities.touch.footGroundContact01);
    world.contacts.bodyCollision01 = c01(input.modalities.touch.bodyCollision01);
  }

  if (input.modalities.bodyPresent) {
    world.pose.head01 = c01(input.modalities.body.headPose01);
    world.pose.torso01 = c01(input.modalities.body.torsoPose01);
    world.pose.rightArm01 = c01(input.modalities.body.rightArmPose01);
    world.pose.leftArm01 = c01(input.modalities.body.leftArmPose01);
    world.pose.rightLeg01 = c01(input.modalities.body.rightLegPose01);
    world.pose.leftLeg01 = c01(input.modalities.body.leftLegPose01);
    world.pose.handConfidence01 = c01(input.modalities.body.handConfidence01);
    world.pose.footConfidence01 = c01(input.modalities.body.footConfidence01);
    world.pose.ownership01 = c01(input.modalities.body.avatarOwnership01);
    world.pose.mismatch01 = c01(input.modalities.body.bodyMismatch01);
    world.scene.avatarBody01 = c01(input.modalities.body.avatarOwnership01);
    world.scene.walkableCorridorConfidence01 = c01(input.modalities.body.footConfidence01);
    world.scene.handVisibility01 = c01(input.modalities.body.handConfidence01);
    world.scene.bodyVisibility01 = c01(input.modalities.body.avatarOwnership01);
  }

  if (input.modalities.vestibularPresent) {
    world.motion.linearAcceleration01 = c01(input.modalities.vestibular.linearAcceleration01);
    world.motion.angularVelocity01 = c01(input.modalities.vestibular.angularVelocity01);
    world.motion.gravityDirection01 = c01(input.modalities.vestibular.gravityDirection01);
    world.motion.balanceError01 = c01(input.modalities.vestibular.balanceError01);
    world.motion.sicknessRisk01 = c01(input.modalities.vestibular.motionSicknessRisk01);
    world.motion.visualVestibularMismatch01 = c01(input.modalities.vestibular.visualVestibularMismatch01);
  }

  if (input.modalities.chemicalPresent && policy_.chemicalChannelsEnabled) {
    world.odor.presence01 = c01(input.modalities.chemical.odorPresence01);
    world.odor.identityCluster01 = c01(input.modalities.chemical.odorIdentity01);
    world.odor.pleasantness01 = c01(input.modalities.chemical.odorPleasantness01);
    world.odor.freshness01 = c01(input.modalities.chemical.freshness01);
    world.odor.smoke01 = c01(input.modalities.chemical.smokeWarning01);
    world.odor.chemical01 = c01(input.modalities.chemical.chemicalWarning01);
    world.odor.decay01 = c01(input.modalities.chemical.decay01);
    world.taste.sweet01 = c01(input.modalities.chemical.sweet01);
    world.taste.salty01 = c01(input.modalities.chemical.salty01);
    world.taste.sour01 = c01(input.modalities.chemical.sour01);
    world.taste.bitter01 = c01(input.modalities.chemical.bitter01);
    world.taste.umami01 = c01(input.modalities.chemical.umami01);
    world.taste.mouthfeel01 = c01(input.modalities.chemical.mouthfeel01);
  }

  world.realityAnchor = safeAnchor(input.modalities.realityAnchor);
  world.patientMap = PatientSpecificNeuralMap::simulationDefaults();
  world.latencyBudget.visualLatencyMs = 16.67f;
  world.latencyBudget.auditoryLatencyMs = 16.67f;
  world.latencyBudget.touchLatencyMs = 16.67f;
  world.latencyBudget.proprioceptionLatencyMs = 16.67f;
  world.latencyBudget.vestibularLatencyMs = 16.67f;
  world.latencyBudget.motorLatencyMs = 16.67f;
  return world;
}

NeuralChipOnlyWorldFrame VirtualWorldDataIngress::ingest(const VirtualWorldIngressFrame &input) const {
  VirtualWorldDataIngress mutableIngress(policy_);
  auto canonical = makeCanonicalVirtualWorldFrame(input);
  canonical.modalities.chemicalPresent = true;
  mutableIngress.policy_.chemicalChannelsEnabled = true;
  return mutableIngress.buildWorld(canonical);
}

} // namespace dnxr
