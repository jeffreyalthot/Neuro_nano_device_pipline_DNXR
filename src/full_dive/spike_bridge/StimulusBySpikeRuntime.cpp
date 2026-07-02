#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"

#include <algorithm>
#include <string_view>

namespace dnxr {

void StimulusBySpikeRuntime::markStage(StimulusBySpikeResult &result,
                                       const char *stageName) {
  if (result.executedStageCount < result.executedStages.size()) {
    result.executedStages[result.executedStageCount++] = stageName;
  }
}

bool StimulusBySpikeRuntime::stageCoverageComplete(
    const StimulusBySpikeResult &result) {
  if (result.executedStageCount != DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT)
    return false;
  for (std::size_t index = 0; index < DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT;
       ++index) {
    if (std::string_view(result.executedStages[index]) !=
        DNXR_STIMULUS_BY_SPIKE_STAGE_ORDER[index]) {
      return false;
    }
  }
  return true;
}

bool StimulusBySpikeRuntime::safetyPreflightAccepted(
    const StimulusBySpikeInput &input) {
  return input.safety.simulationOnly && input.safety.patientStopSignalAlive &&
         input.safety.emergencyCutoffAvailable &&
         input.safety.painMappingBlocked &&
         input.safety.rawSensoryPayloadBlocked;
}

const char *
StimulusBySpikeRuntime::firstHoldReason(const StimulusBySpikeResult &result) {
  if (!result.vectorBundle.accepted)
    return result.vectorBundle.reason;
  if (result.spikeIntent.holdRequired)
    return result.spikeIntent.holdReason;
  if (!result.engineSpike.accepted || result.engineSpike.holdRequired)
    return result.engineSpike.reason;
  if (!result.stimulusBySpike.accepted)
    return result.stimulusBySpike.reason;
  return "stimulus_by_spike_hold";
}

StimulusBySpikeResult
StimulusBySpikeRuntime::process(const StimulusBySpikeInput &input) {
  StimulusBySpikeResult result;

  result.vectorBundle = normalizer_.normalize(
      input.frameId, input.timestampMs, input.visual, input.auditory,
      input.touch, input.proprioception, input.vestibular, input.motorIntent,
      input.olfactory, input.gustatory, input.realityAnchor);
  markStage(result, "VectorIntentNormalizer");

  result.vectorBundle = aligner_.align(result.vectorBundle, input.timestampMs);
  markStage(result, "VectorTemporalAlignmentBuffer");

  result.vectorBundle =
      patientBinder_.bind(result.vectorBundle, input.patientMap);
  markStage(result, "PatientTargetBindingLayer");

  result.vectorBundle = coherenceBinder_.bind(result.vectorBundle);
  markStage(result, "CrossModalCoherenceBinder");

  result.spikeIntent = intentBuilder_.build(result.vectorBundle);
  markStage(result, "VectorToSpikeEngineFrameBuilder");

  result.engineSpike = engine_.process(result.spikeIntent);
  markStage(result, "EngineSpikeCore");

  result.engineSpike = validator_.validate(result.engineSpike);
  markStage(result, "EngineSpikeOutputValidator");

  result.engineSpike = envelope_.apply(result.engineSpike, input.sessionState);
  markStage(result, "SpikeEnvelopePolicyApplier");

  result.stimulusBySpike = translator_.translate(result.engineSpike);
  markStage(result, "SpikeToStimulusIntentTranslator");

  result.chipFrame = assembler_.assemble(
      result.stimulusBySpike, input.visual, input.auditory, input.touch,
      input.proprioception, input.vestibular, input.motorIntent,
      input.olfactory, input.gustatory, input.realityAnchor, input.safety);
  markStage(result, "StimulusBySpikePacketAssembler");

  result.stimulusBySpike = feedback_.synchronizeFeedback(
      result.stimulusBySpike, input.readFrame, input.lastAck);
  result.chipFrame.safety.aggregateRisk01 =
      spikeClamp01(std::max(result.chipFrame.safety.aggregateRisk01,
                            result.stimulusBySpike.riskScore01));
  result.chipFrame.safety.activeCeiling01 =
      spikeClamp01(result.stimulusBySpike.intensityScale01);
  markStage(result, "StimulusFeedbackSynchronizer");

  result.stageCoverageComplete = stageCoverageComplete(result);
  result.safetyPreflightAccepted = safetyPreflightAccepted(input);
  result.accepted =
      result.vectorBundle.accepted && !result.spikeIntent.holdRequired &&
      result.engineSpike.accepted && !result.engineSpike.holdRequired &&
      result.stimulusBySpike.accepted && !feedback_.requiresHold() &&
      result.stageCoverageComplete && result.safetyPreflightAccepted;
  result.holdRequired = !result.accepted;
  if (!result.safetyPreflightAccepted) {
    result.reason = "stimulus_by_spike_safety_preflight_hold";
  } else if (!result.stageCoverageComplete) {
    result.reason = "stimulus_by_spike_stage_coverage_incomplete";
  } else {
    result.reason = result.accepted ? "accepted" : firstHoldReason(result);
  }
  return result;
}

} // namespace dnxr
