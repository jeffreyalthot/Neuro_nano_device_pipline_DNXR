#include "full_dive/spike_bridge/SpikeBridgeTypes.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

float spikeClamp01(float value) {
  if (!std::isfinite(value))
    return 0.0f;
  return std::max(0.0f, std::min(1.0f, value));
}

void sanitize(ModalityVectorPacket &packet) {
  for (auto &channel : packet.channels01)
    channel = spikeClamp01(channel);
  packet.channelCount = std::min(packet.channelCount, packet.channels01.size());
  packet.confidence01 = spikeClamp01(packet.confidence01);
  packet.risk01 = spikeClamp01(packet.risk01);
  packet.priority01 = spikeClamp01(packet.priority01);
  packet.coherenceNeed01 = spikeClamp01(packet.coherenceNeed01);
  packet.sceneBinding01 = spikeClamp01(packet.sceneBinding01);
  packet.patientMapConfidence01 = spikeClamp01(packet.patientMapConfidence01);
  packet.phaseSlot01 = spikeClamp01(packet.phaseSlot01);

  if (!packet.enabled) {
    packet.confidence01 = 0.0f;
    packet.risk01 = 0.0f;
  }
}

void sanitize(VectorBundleFrame &frame) {
  frame.modalityCount = std::min(frame.modalityCount, frame.modalities.size());
  for (auto &packet : frame.modalities)
    sanitize(packet);
  frame.globalCoherence01 = spikeClamp01(frame.globalCoherence01);
  frame.aggregateRisk01 = spikeClamp01(frame.aggregateRisk01);
  frame.sceneContinuity01 = spikeClamp01(frame.sceneContinuity01);
  frame.bodyOwnership01 = spikeClamp01(frame.bodyOwnership01);
  frame.realityAnchor01 = spikeClamp01(frame.realityAnchor01);
}

void sanitize(EngineSpikeFrame &frame) {
  frame.laneCount = std::min(frame.laneCount, frame.spikeLanes01.size());
  for (auto &lane : frame.spikeLanes01)
    lane = spikeClamp01(lane);
  for (auto &load : frame.modalityLoad01)
    load = spikeClamp01(load);
  frame.totalSpikeLoad01 = spikeClamp01(frame.totalSpikeLoad01);
  frame.phaseLockQuality01 = spikeClamp01(frame.phaseLockQuality01);
  frame.predictionConfidence01 = spikeClamp01(frame.predictionConfidence01);
  frame.feedbackNeed01 = spikeClamp01(frame.feedbackNeed01);
  frame.safetyRisk01 = spikeClamp01(frame.safetyRisk01);
}

void sanitize(StimulusBySpikeFrame &frame) {
  frame.channelCount =
      std::min(frame.channelCount, frame.boundedChannels01.size());
  for (auto &channel : frame.boundedChannels01)
    channel = spikeClamp01(channel);
  for (auto &intensity : frame.modalityIntensity01)
    intensity = spikeClamp01(intensity);
  frame.aggregateSensoryLoad01 = spikeClamp01(frame.aggregateSensoryLoad01);
  frame.intensityScale01 = spikeClamp01(frame.intensityScale01);
  frame.riskScore01 = spikeClamp01(frame.riskScore01);
  frame.realityAnchor01 = spikeClamp01(frame.realityAnchor01);
  frame.feedbackExpectation01 = spikeClamp01(frame.feedbackExpectation01);
}

} // namespace dnxr
