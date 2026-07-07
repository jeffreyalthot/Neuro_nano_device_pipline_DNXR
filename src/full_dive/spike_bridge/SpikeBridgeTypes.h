#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace dnxr {

constexpr std::size_t DNXR_SPIKE_BRIDGE_MODALITY_COUNT = 9;
constexpr std::size_t DNXR_SPIKE_BRIDGE_VECTOR_CHANNELS = 32;
constexpr std::size_t DNXR_SPIKE_BRIDGE_LANES = 64;
constexpr std::size_t DNXR_PPP_GRAMMAR_REFRESH_FRAMES = 60;
constexpr float DNXR_PPP_STIMULUS_SYNC_THRESHOLD_01 = 0.80f;

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

struct PPPGrammarModalityEntry {
  std::uint16_t modalityId = 0;
  std::uint16_t channelId = 0;
  const char *modalityName = "UNKNOWN";
  const char *semanticPath = "unknown";
  float channelConfidence01 = 0.0f;
  float maxActiveIntensity01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
  bool requiresAck = true;
  bool requiresClinicalGate = true;
  bool feedbackExpected = true;
  bool stimulusUnlocked = false;
};

struct PPPGrammarFrame {
  const char *pppVersion = "0.7.0";
  const char *grammarId = "DNXR_SPIKE_GRAMMAR_MAIN";
  std::uint64_t frameId = 0;
  std::uint64_t validFromFrame = 0;
  std::uint64_t expiresAtFrame = DNXR_PPP_GRAMMAR_REFRESH_FRAMES;
  std::size_t grammarRefreshFrames = DNXR_PPP_GRAMMAR_REFRESH_FRAMES;
  float globalSync01 = 0.0f;
  float activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
  bool clinicalGated = true;
  bool feedbackOnly = true;
  bool stimulusCandidateAllowed = false;
  bool ackRequired = true;
  bool ackReceived = false;
  bool emergencyLatched = false;
  bool blockedDangerSymbols = true;
  bool rawSensoryPayloadBlocked = true;
  std::size_t hardwareElectrodeCount = 0;
  std::size_t hardwareWireCount = 0;
  std::size_t hardwareStimulusChannelLimit = 0;
  std::array<PPPGrammarModalityEntry, DNXR_SPIKE_BRIDGE_MODALITY_COUNT>
      modalities{};
  std::size_t modalityCount = 0;
};

struct ThinkingGroupSummary {
  SpikeModality modality = SpikeModality::Visual;
  float featureLayer01 = 0.0f;
  float temporalLayer01 = 0.0f;
  float safetyLayer01 = 0.0f;
  float idealSpikeLayer01 = 0.0f;
  float internalSpikeCoherence01 = 0.0f;
};

struct MultiThinkingFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  std::array<ThinkingGroupSummary, DNXR_SPIKE_BRIDGE_MODALITY_COUNT>
      thinkingGroups{};
  std::size_t thinkingGroupCount = 0;
  float crossGroupCoherence01 = 0.0f;
  float candidateConfidence01 = 0.0f;
  float safetyConsensus01 = 0.0f;
  bool accepted = true;
  const char *reason = "accepted";
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
