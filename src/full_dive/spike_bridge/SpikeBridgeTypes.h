#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace dnxr {

constexpr std::size_t DNXR_SPIKE_BRIDGE_MODALITY_COUNT = 9;
constexpr std::size_t DNXR_SPIKE_BRIDGE_VECTOR_CHANNELS = 32;
constexpr std::size_t DNXR_SPIKE_BRIDGE_LANES = 64;

enum class SpikeModality {
  Visual,
  Auditory,
  Touch,
  Proprioception,
  Vestibular,
  Motor,
  Olfactory,
  Gustatory,
  RealityAnchor
};

struct ModalityVectorPacket {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  SpikeModality modality = SpikeModality::Visual;
  std::array<float, DNXR_SPIKE_BRIDGE_VECTOR_CHANNELS> channels01{};
  std::size_t channelCount = 0;
  float confidence01 = 0.0f;
  float risk01 = 0.0f;
  float priority01 = 0.0f;
  float coherenceNeed01 = 0.0f;
  float sceneBinding01 = 0.0f;
  float patientMapConfidence01 = 0.0f;
  float latencyBudgetMs = 0.0f;
  float phaseSlot01 = 0.0f;
  bool enabled = true;
  bool chemicalLike = false;
  bool motorLike = false;
  bool sensoryLike = false;
  const char *sourceEncoder = "unknown";
};

struct VectorBundleFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  std::array<ModalityVectorPacket, DNXR_SPIKE_BRIDGE_MODALITY_COUNT>
      modalities{};
  std::size_t modalityCount = 0;
  float globalCoherence01 = 1.0f;
  float aggregateRisk01 = 0.0f;
  float sceneContinuity01 = 1.0f;
  float bodyOwnership01 = 1.0f;
  float realityAnchor01 = 1.0f;
  bool accepted = true;
  const char *reason = "accepted";
};

struct SpikeEngineIntentFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  std::array<ModalityVectorPacket, DNXR_SPIKE_BRIDGE_MODALITY_COUNT>
      routedModalities{};
  std::size_t routedCount = 0;
  float globalLoad01 = 0.0f;
  float motorPriority01 = 0.0f;
  float sensoryPriority01 = 0.0f;
  float vestibularRisk01 = 0.0f;
  float chemicalRisk01 = 0.0f;
  float phaseLockQuality01 = 1.0f;
  float expectedFeedbackQuality01 = 1.0f;
  bool holdRequired = false;
  const char *holdReason = "none";
};

struct EngineSpikeFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  std::array<float, DNXR_SPIKE_BRIDGE_LANES> spikeLanes01{};
  std::size_t laneCount = 0;
  std::array<float, DNXR_SPIKE_BRIDGE_MODALITY_COUNT> modalityLoad01{};
  float totalSpikeLoad01 = 0.0f;
  float phaseLockQuality01 = 1.0f;
  float predictionConfidence01 = 1.0f;
  float feedbackNeed01 = 0.0f;
  float safetyRisk01 = 0.0f;
  bool accepted = true;
  bool holdRequired = false;
  const char *reason = "accepted";
};

struct StimulusBySpikeFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  std::array<float, DNXR_SPIKE_BRIDGE_LANES> boundedChannels01{};
  std::size_t channelCount = 0;
  std::array<float, DNXR_SPIKE_BRIDGE_MODALITY_COUNT> modalityIntensity01{};
  float aggregateSensoryLoad01 = 0.0f;
  float intensityScale01 = 0.0f;
  float riskScore01 = 0.0f;
  float realityAnchor01 = 1.0f;
  float feedbackExpectation01 = 1.0f;
  bool fiveSenseCoverage = false;
  bool allowDispatch = false;
  bool exposureBudgetOk = true;
  bool accepted = true;
  const char *reason = "accepted";
};

float spikeClamp01(float value);
void sanitize(ModalityVectorPacket &packet);
void sanitize(VectorBundleFrame &frame);
void sanitize(EngineSpikeFrame &frame);
void sanitize(StimulusBySpikeFrame &frame);

} // namespace dnxr
