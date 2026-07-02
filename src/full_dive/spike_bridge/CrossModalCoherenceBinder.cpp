#include "full_dive/spike_bridge/CrossModalCoherenceBinder.h"

#include <algorithm>

namespace dnxr {
VectorBundleFrame
CrossModalCoherenceBinder::bind(const VectorBundleFrame &input) const {
  auto output = input;
  const auto count = std::min(output.modalityCount, output.modalities.size());

  float risk = output.aggregateRisk01;
  float confidence = output.globalCoherence01;
  bool vestibularConflict = false;
  bool chemicalSceneConflict = false;
  bool multiSenseEvent = false;
  float visualMotion01 = 0.0f;
  float vestibularLoad01 = 0.0f;

  for (std::size_t index = 0; index < count; ++index) {
    auto &packet = output.modalities[index];
    risk = std::max(risk, packet.risk01);
    confidence =
        std::min(confidence, packet.enabled ? packet.confidence01 : 1.0f);

    if (packet.channelCount > 0 && packet.channels01[0] > 0.2f) {
      packet.coherenceNeed01 = std::max(packet.coherenceNeed01, 0.5f);
      multiSenseEvent = true;
    }

    if (packet.chemicalLike && packet.channelCount > 0 &&
        packet.channels01[0] > 0.1f && packet.sceneBinding01 < 0.2f) {
      chemicalSceneConflict = true;
    }

    if (packet.modality == SpikeModality::Visual && packet.channelCount > 3) {
      visualMotion01 = packet.channels01[3];
    }
    if (packet.modality == SpikeModality::Vestibular) {
      vestibularLoad01 = packet.channelCount > 1 ? packet.channels01[1] : 0.0f;
      if (packet.risk01 > 0.85f) {
        vestibularConflict = true;
      }
    }
  }

  if (vestibularLoad01 > 0.45f && visualMotion01 < 0.10f) {
    vestibularConflict = true;
    risk = std::max(risk, 0.85f);
  }

  output.aggregateRisk01 = risk;
  output.globalCoherence01 = std::min(output.globalCoherence01, confidence);
  if (multiSenseEvent) {
    output.sceneContinuity01 =
        std::min(output.sceneContinuity01, output.globalCoherence01 + 0.25f);
  }

  if (vestibularConflict || chemicalSceneConflict) {
    output.accepted = false;
    output.reason = vestibularConflict ? "visual_vestibular_coherence_hold"
                                       : "chemical_scene_binding_hold";
    output.globalCoherence01 = std::min(output.globalCoherence01, 0.2f);
  }

  sanitize(output);
  return output;
}
} // namespace dnxr
