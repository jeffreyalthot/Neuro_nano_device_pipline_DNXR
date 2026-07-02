#include "full_dive/spike_bridge/PatientTargetBindingLayer.h"

#include <algorithm>

namespace dnxr {
namespace {
const PatientModalityProfile &profileFor(const PatientSpecificNeuralMap &map,
                                         SpikeModality modality) {
  switch (modality) {
  case SpikeModality::Visual:
    return map.visual;
  case SpikeModality::Auditory:
    return map.auditory;
  case SpikeModality::Touch:
    return map.touch;
  case SpikeModality::Proprioception:
    return map.proprioception;
  case SpikeModality::Vestibular:
    return map.vestibular;
  case SpikeModality::Motor:
    return map.motor;
  case SpikeModality::Olfactory:
    return map.olfactory;
  case SpikeModality::Gustatory:
    return map.gustatory;
  case SpikeModality::RealityAnchor:
    return map.proprioception;
  }
  return map.proprioception;
}

float maturityRiskBias(NeuralChannelMaturity maturity) {
  switch (maturity) {
  case NeuralChannelMaturity::SupportedSimulation:
    return 0.0f;
  case NeuralChannelMaturity::ResearchOnly:
    return 0.05f;
  case NeuralChannelMaturity::Experimental:
    return 0.10f;
  case NeuralChannelMaturity::Disabled:
    return 1.0f;
  }
  return 1.0f;
}
} // namespace

VectorBundleFrame
PatientTargetBindingLayer::bind(const VectorBundleFrame &input,
                                const PatientSpecificNeuralMap &map) const {
  auto output = input;
  const auto count = std::min(output.modalityCount, output.modalities.size());

  float aggregateRisk = 0.0f;
  float globalCoherence = 1.0f;
  for (std::size_t index = 0; index < count; ++index) {
    auto &packet = output.modalities[index];
    if (packet.modality == SpikeModality::RealityAnchor) {
      aggregateRisk = std::max(aggregateRisk, packet.risk01);
      globalCoherence = std::min(globalCoherence, packet.confidence01);
      continue;
    }

    const auto &profile = profileFor(map, packet.modality);
    packet.patientMapConfidence01 = spikeClamp01(profile.confidence01);
    if (!profile.enabled ||
        profile.maturity == NeuralChannelMaturity::Disabled) {
      packet.enabled = false;
      packet.confidence01 = 0.0f;
      packet.risk01 = 0.0f;
      for (auto &channel : packet.channels01) {
        channel = 0.0f;
      }
    } else {
      const float sensitivity = spikeClamp01(profile.sensitivity01);
      const float threshold = spikeClamp01(profile.threshold01);
      const float ceiling = spikeClamp01(profile.maxAllowed01);
      for (std::size_t channelIndex = 0;
           channelIndex < packet.channelCount &&
           channelIndex < packet.channels01.size();
           ++channelIndex) {
        float bounded =
            spikeClamp01(packet.channels01[channelIndex] * sensitivity);
        bounded = bounded < threshold ? 0.0f : std::min(bounded, ceiling);
        packet.channels01[channelIndex] = bounded;
      }
      if (profile.confidence01 < 0.5f) {
        packet.confidence01 *= packet.patientMapConfidence01;
      }
      packet.risk01 = spikeClamp01(packet.risk01 +
                                   maturityRiskBias(profile.maturity) *
                                       (1.0f - packet.patientMapConfidence01));
    }

    aggregateRisk = std::max(aggregateRisk, packet.risk01);
    globalCoherence =
        std::min(globalCoherence, packet.enabled ? packet.confidence01 : 1.0f);
  }

  output.aggregateRisk01 = aggregateRisk;
  output.globalCoherence01 = globalCoherence;
  sanitize(output);
  return output;
}
} // namespace dnxr
