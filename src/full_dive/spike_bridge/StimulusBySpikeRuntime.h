#pragma once

#include "full_dive/spike_bridge/CrossModalCoherenceBinder.h"
#include "full_dive/spike_bridge/EngineSpikeCore.h"
#include "full_dive/spike_bridge/EngineSpikeOutputValidator.h"
#include "full_dive/spike_bridge/MultiLayerSpikeThinkingEngine.h"
#include "full_dive/spike_bridge/PPPGrammarFrameBuilder.h"
#include "full_dive/spike_bridge/PatientTargetBindingLayer.h"
#include "full_dive/spike_bridge/SpikeEnvelopePolicyApplier.h"
#include "full_dive/spike_bridge/SpikeToStimulusIntentTranslator.h"
#include "full_dive/spike_bridge/StimulusBySpikePacketAssembler.h"
#include "full_dive/spike_bridge/StimulusFeedbackSynchronizer.h"
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
#include "full_dive/spike_bridge/VectorTemporalAlignmentBuffer.h"
#include "full_dive/spike_bridge/VectorToSpikeEngineFrameBuilder.h"
#include "neural_chip/NeuralChipSessionState.h"
#include "neural_chip/PatientSpecificNeuralMap.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace dnxr {

constexpr std::size_t DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT = 13;

inline constexpr std::array<std::string_view,
                            DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT>
    DNXR_STIMULUS_BY_SPIKE_STAGE_ORDER = {
        "VectorIntentNormalizer",          "VectorTemporalAlignmentBuffer",
        "PatientTargetBindingLayer",       "CrossModalCoherenceBinder",
        "PPPGrammarFrameBuilder",          "VectorToSpikeEngineFrameBuilder",
        "MultiLayerSpikeThinkingEngine",   "EngineSpikeCore",
        "EngineSpikeOutputValidator",      "SpikeEnvelopePolicyApplier",
        "SpikeToStimulusIntentTranslator",
        "StimulusBySpikePacketAssembler",
        "StimulusFeedbackSynchronizer"};

struct StimulusBySpikeInput {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  NeuralVisualVector visual;
  NeuralAuditoryVector auditory;
  NeuralTouchVector touch;
  NeuralProprioceptionVector proprioception;
  NeuralVestibularVector vestibular;
  NeuralMotorIntentVector motorIntent;
  NeuralOlfactoryVector olfactory;
  NeuralGustatoryVector gustatory;
  NeuralRealityAnchorVector realityAnchor;
  NeuralSafetyVector safety;
  PatientSpecificNeuralMap patientMap;
  NeuralChipSessionState sessionState;
  ChipReadFrame readFrame;
  NeuralChipAck lastAck;
};

struct StimulusBySpikeResult {
  bool accepted = false;
  bool holdRequired = false;
  const char *reason = "not_processed";
  VectorBundleFrame vectorBundle;
  PPPGrammarFrame pppGrammarFrame;
  MultiThinkingFrame multiThinkingFrame;
  SpikeEngineIntentFrame spikeIntent;
  EngineSpikeFrame engineSpike;
  StimulusBySpikeFrame stimulusBySpike;
  FullDiveChipFrame chipFrame;
  std::array<const char *, DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT> executedStages{};
  std::size_t executedStageCount = 0;
  bool stageCoverageComplete = false;
  bool safetyPreflightAccepted = true;
};

class StimulusBySpikeRuntime {
public:
  StimulusBySpikeResult process(const StimulusBySpikeInput &input);

private:
  static void markStage(StimulusBySpikeResult &result, const char *stageName);
  static const char *firstHoldReason(const StimulusBySpikeResult &result);
  static bool stageCoverageComplete(const StimulusBySpikeResult &result);
  static bool safetyPreflightAccepted(const StimulusBySpikeInput &input);

  VectorIntentNormalizer normalizer_{};
  VectorTemporalAlignmentBuffer aligner_{};
  PatientTargetBindingLayer patientBinder_{};
  CrossModalCoherenceBinder coherenceBinder_{};
  PPPGrammarFrameBuilder grammarBuilder_{};
  VectorToSpikeEngineFrameBuilder intentBuilder_{};
  MultiLayerSpikeThinkingEngine thinkingEngine_{};
  EngineSpikeCore engine_{};
  EngineSpikeOutputValidator validator_{};
  SpikeEnvelopePolicyApplier envelope_{};
  SpikeToStimulusIntentTranslator translator_{};
  StimulusBySpikePacketAssembler assembler_{};
  StimulusFeedbackSynchronizer feedback_{};
};

} // namespace dnxr
