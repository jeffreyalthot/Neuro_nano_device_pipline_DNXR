#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
#include <algorithm>
namespace dnxr {
static ModalityVectorPacket pkt(std::uint64_t id, double ts, SpikeModality m,
                                const char *s) {
  ModalityVectorPacket p;
  p.frameId = id;
  p.timestampMs = ts;
  p.modality = m;
  p.sourceEncoder = s;
  p.enabled = true;
  p.confidence01 = 1;
  p.sceneBinding01 = 1;
  p.patientMapConfidence01 = 1;
  p.latencyBudgetMs = 16.0f;
  return p;
}
static void fill(ModalityVectorPacket &p, std::initializer_list<float> xs) {
  std::size_t i = 0;
  for (float x : xs) {
    if (i < p.channels01.size())
      p.channels01[i++] = spikeClamp01(x);
  }
  p.channelCount = i;
  sanitize(p);
}
ModalityVectorPacket
VectorIntentNormalizer::fromVisual(std::uint64_t id, double ts,
                                   const NeuralVisualVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Visual, "VisualCorticalWorldEncoder");
  p.sensoryLike = true;
  p.risk01 = std::max(v.flickerRisk01, v.visualFatigueRisk01);
  p.priority01 = std::max(v.objectSaliency01, v.hazardMap01);
  fill(p, {v.globalLuminance01, v.contrast01, v.edgeMap01, v.motionFlow01,
           v.depthNear01, v.depthMid01, v.depthFar01, v.objectSaliency01,
           v.navigationCorridor01, v.hazardMap01, v.avatarBodyVisibility01,
           v.textSymbolPriority01, v.phospheneDensity01, v.phospheneStability01,
           v.flickerRisk01, v.visualFatigueRisk01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromAuditory(std::uint64_t id, double ts,
                                     const NeuralAuditoryVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Auditory, "AuditoryTonotopicEncoder");
  p.sensoryLike = true;
  p.risk01 = v.startleRisk01;
  p.priority01 = std::max(v.speechPriority01, v.dangerSoundPriority01);
  fill(p, {v.loudness01, v.lowBand01, v.midBand01, v.highBand01,
           v.envelopeAttack01, v.envelopeSustain01, v.spatialLeftRight01,
           v.spatialFrontBack01, v.distance01, v.speechPriority01,
           v.dangerSoundPriority01, v.startleRisk01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromTouch(std::uint64_t id, double ts,
                                  const NeuralTouchVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Touch, "NeuralVirtualEpidermisEncoder");
  p.sensoryLike = true;
  const bool noContact = std::max({v.contactPressure01, v.contactArea01,
                                   v.vibrationFlutter01, v.vibrationTexture01,
                                   v.rightHandContact01, v.leftHandContact01,
                                   v.footGroundContact01, v.bodyCollision01}) <=
                         0.0f;
  const bool blockersSafe = std::min({v.painBlocked01, v.burnBlocked01,
                                      v.crushBlocked01, v.shockBlocked01}) >=
                            1.0f;
  p.confidence01 =
      noContact && blockersSafe ? 1.0f : v.somatosensoryConfidence01;
  p.risk01 = 1.0f - std::min({v.painBlocked01, v.burnBlocked01,
                              v.crushBlocked01, v.shockBlocked01});
  fill(p, {v.contactPressure01, v.contactArea01, v.vibrationFlutter01,
           v.vibrationTexture01, v.slip01, v.hardness01, v.softness01,
           v.warmth01, v.cold01, v.rightHandContact01, v.leftHandContact01,
           v.footGroundContact01, v.bodyCollision01, v.painBlocked01,
           v.burnBlocked01, v.crushBlocked01, v.shockBlocked01,
           v.somatosensoryConfidence01});
  return p;
}
ModalityVectorPacket VectorIntentNormalizer::fromProprioception(
    std::uint64_t id, double ts, const NeuralProprioceptionVector &v) const {
  auto p =
      pkt(id, ts, SpikeModality::Proprioception, "NeuralProprioceptionEncoder");
  p.sensoryLike = true;
  const bool neutralPose =
      std::max({v.headPose01, v.torsoPose01, v.rightArmPose01, v.leftArmPose01,
                v.rightLegPose01, v.leftLegPose01, v.bodySchemaMismatch01}) <=
      0.0f;
  p.confidence01 = neutralPose
                       ? 1.0f
                       : (v.handPoseConfidence01 + v.footPoseConfidence01) *
                             0.5f;
  p.risk01 = v.bodySchemaMismatch01;
  p.sceneBinding01 = v.avatarOwnership01;
  fill(p,
       {v.headPose01, v.torsoPose01, v.rightArmPose01, v.leftArmPose01,
        v.rightLegPose01, v.leftLegPose01, v.handPoseConfidence01,
        v.footPoseConfidence01, v.avatarOwnership01, v.bodySchemaMismatch01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromVestibular(std::uint64_t id, double ts,
                                       const NeuralVestibularVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Vestibular, "NeuralVestibularEncoder");
  p.sensoryLike = true;
  p.risk01 = std::max(v.motionSicknessRisk01, v.visualVestibularMismatch01);
  fill(p, {v.linearAcceleration01, v.angularVelocity01, v.gravityDirection01,
           v.balanceError01, v.motionSicknessRisk01,
           v.visualVestibularMismatch01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromMotor(std::uint64_t id, double ts,
                                  const NeuralMotorIntentVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Motor, "MotorIntentDecoder");
  p.motorLike = true;
  const bool onlyStopIntent =
      std::max({v.moveForward01, v.moveBackward01, v.strafeLeft01,
                v.strafeRight01, v.turnYaw01, v.lookPitch01,
                v.rightArmReach01, v.leftArmReach01, v.rightHandGrip01,
                v.leftHandGrip01, v.gaitIntent01, v.postureBalance01,
                v.tremorRisk01, v.spasmRisk01, v.fatigueRisk01}) <= 0.0f &&
      v.stopIntent01 >= 1.0f;
  p.confidence01 = onlyStopIntent ? 1.0f : v.confidence01;
  p.risk01 = std::max({v.tremorRisk01, v.spasmRisk01, v.fatigueRisk01});
  p.priority01 = v.stopIntent01;
  fill(p, {v.moveForward01, v.moveBackward01, v.strafeLeft01, v.strafeRight01,
           v.turnYaw01, v.lookPitch01, v.rightArmReach01, v.leftArmReach01,
           v.rightHandGrip01, v.leftHandGrip01, v.gaitIntent01,
           v.postureBalance01, v.stopIntent01, v.confidence01, v.tremorRisk01,
           v.spasmRisk01, v.fatigueRisk01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromOlfactory(std::uint64_t id, double ts,
                                      const NeuralOlfactoryVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Olfactory, "NeuralOlfactoryEncoder");
  p.sensoryLike = true;
  p.chemicalLike = true;
  p.risk01 = std::max(
      {v.nauseaRisk01, v.trigeminalContaminationRisk01, v.chemicalWarning01});
  fill(p, {v.odorPresence01, v.odorIdentityCluster01, v.pleasantness01,
           v.freshness01, v.smokeWarning01, v.chemicalWarning01, v.decay01,
           v.habituation01, v.respirationPhase01, v.nauseaRisk01,
           v.trigeminalContaminationRisk01});
  return p;
}
ModalityVectorPacket
VectorIntentNormalizer::fromGustatory(std::uint64_t id, double ts,
                                      const NeuralGustatoryVector &v) const {
  auto p = pkt(id, ts, SpikeModality::Gustatory, "NeuralGustatoryEncoder");
  p.sensoryLike = true;
  p.chemicalLike = true;
  p.risk01 = std::max(v.nauseaRisk01, v.swallowingConflictRisk01);
  fill(p, {v.sweet01, v.salty01, v.sour01, v.bitter01, v.umami01, v.metallic01,
           v.mouthfeel01, v.temperature01, v.pleasantness01, v.nauseaRisk01,
           v.swallowingConflictRisk01});
  return p;
}
ModalityVectorPacket VectorIntentNormalizer::fromRealityAnchor(
    std::uint64_t id, double ts, const NeuralRealityAnchorVector &v) const {
  auto p =
      pkt(id, ts, SpikeModality::RealityAnchor, "NeuralRealityAnchorVector");
  p.confidence01 = std::min({v.orientation01, v.identityContinuity01,
                             v.sessionBoundary01, v.externalWorldRecall01});
  p.sceneBinding01 = p.confidence01;
  p.priority01 = 1;
  fill(p, {v.orientation01, v.identityContinuity01, v.sessionBoundary01,
           v.externalWorldRecall01, v.required ? 1.0f : 0.0f});
  return p;
}
VectorBundleFrame VectorIntentNormalizer::normalize(
    std::uint64_t id, double ts, const NeuralVisualVector &a,
    const NeuralAuditoryVector &b, const NeuralTouchVector &c,
    const NeuralProprioceptionVector &d, const NeuralVestibularVector &e,
    const NeuralMotorIntentVector &f, const NeuralOlfactoryVector &g,
    const NeuralGustatoryVector &h, const NeuralRealityAnchorVector &r) const {
  VectorBundleFrame out;
  out.frameId = id;
  out.timestampMs = ts;
  out.modalityCount = 9;
  out.modalities = {fromVisual(id, ts, a),       fromAuditory(id, ts, b),
                    fromTouch(id, ts, c),        fromProprioception(id, ts, d),
                    fromVestibular(id, ts, e),   fromMotor(id, ts, f),
                    fromOlfactory(id, ts, g),    fromGustatory(id, ts, h),
                    fromRealityAnchor(id, ts, r)};
  float risk = 0, coh = 1;
  for (const auto &p : out.modalities) {
    risk = std::max(risk, p.risk01);
    coh = std::min(coh, p.confidence01);
  }
  out.aggregateRisk01 = risk;
  out.globalCoherence01 = coh;
  out.realityAnchor01 = out.modalities[8].confidence01;
  sanitize(out);
  return out;
}
} // namespace dnxr
