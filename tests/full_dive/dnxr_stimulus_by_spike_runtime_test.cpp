#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"

#include <cassert>
#include <cstring>
#include <string_view>

using namespace dnxr;

int main() {
  StimulusBySpikeInput input;
  input.frameId = 1;
  input.patientMap = PatientSpecificNeuralMap::simulationDefaults();
  input.lastAck.packetAccepted = true;
  input.readFrame.signalQuality01 = 1.0f;
  input.safety.simulationOnly = true;
  input.safety.patientStopSignalAlive = true;
  input.safety.emergencyCutoffAvailable = true;
  input.safety.painMappingBlocked = true;
  input.safety.rawSensoryPayloadBlocked = true;

  const auto result = StimulusBySpikeRuntime{}.process(input);
  assert(result.chipFrame.frameId == 1);
  assert(result.vectorBundle.modalityCount == DNXR_SPIKE_BRIDGE_MODALITY_COUNT);
  assert(result.pppGrammarFrame.frameId == 1);
  assert(result.pppGrammarFrame.modalityCount == DNXR_SPIKE_BRIDGE_MODALITY_COUNT);
  assert(result.pppGrammarFrame.grammarRefreshFrames ==
         DNXR_PPP_GRAMMAR_REFRESH_FRAMES);
  assert(result.pppGrammarFrame.expiresAtFrame == 61);
  assert(result.pppGrammarFrame.feedbackOnly);
  assert(!result.pppGrammarFrame.stimulusCandidateAllowed);
  assert(result.pppGrammarFrame.blockedDangerSymbols);
  assert(result.pppGrammarFrame.rawSensoryPayloadBlocked);
  assert(result.pppGrammarFrame.modalities[0].modalityId == 0x01);
  assert(result.pppGrammarFrame.modalities[0].requiresAck);
  assert(result.pppGrammarFrame.modalities[0].requiresClinicalGate);
  assert(result.pppGrammarFrame.modalities[0].maxActiveIntensity01 <=
         DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
  assert(result.executedStageCount == DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT);
  assert(result.multiThinkingFrame.thinkingGroupCount ==
         DNXR_SPIKE_BRIDGE_MODALITY_COUNT);
  assert(result.multiThinkingFrame.safetyConsensus01 >= 0.0f);
  assert(result.stageCoverageComplete);
  assert(result.safetyPreflightAccepted);
  assert(!result.accepted);
  assert(result.holdRequired);
  assert(std::strcmp(result.reason,
                     "spike_intent_low_global_coherence_hold") == 0);
  for (std::size_t index = 0; index < DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT;
       ++index) {
    assert(std::string_view(result.executedStages[index]) ==
           DNXR_STIMULUS_BY_SPIKE_STAGE_ORDER[index]);
  }

  input.safety.rawSensoryPayloadBlocked = false;
  const auto unsafe = StimulusBySpikeRuntime{}.process(input);
  assert(!unsafe.accepted);
  assert(unsafe.holdRequired);
  assert(!unsafe.safetyPreflightAccepted);
  assert(std::strcmp(unsafe.reason,
                     "stimulus_by_spike_safety_preflight_hold") == 0);

  StimulusBySpikeInput synchronized = input;
  synchronized.safety.rawSensoryPayloadBlocked = true;
  synchronized.lastAck.packetAccepted = true;
  synchronized.lastAck.signalQuality = 1.0f;
  synchronized.sessionState.realityAnchorStability01 = 1.0f;
  synchronized.visual.depthNear01 = 0.82f;
  synchronized.visual.contrast01 = 0.85f;
  synchronized.visual.phospheneStability01 = 0.95f;
  synchronized.touch.contactPressure01 = 0.25f;
  synchronized.touch.somatosensoryConfidence01 = 0.95f;
  synchronized.proprioception.handPoseConfidence01 = 0.95f;
  synchronized.proprioception.footPoseConfidence01 = 0.95f;
  synchronized.proprioception.avatarOwnership01 = 0.95f;
  synchronized.motorIntent.confidence01 = 0.95f;
  synchronized.motorIntent.stopIntent01 = 0.2f;
  const auto unlocked = StimulusBySpikeRuntime{}.process(synchronized);
  assert(!unlocked.pppGrammarFrame.feedbackOnly);
  assert(unlocked.pppGrammarFrame.stimulusCandidateAllowed);
  assert(unlocked.multiThinkingFrame.accepted);
  assert(unlocked.multiThinkingFrame.candidateConfidence01 > 0.0f);

  StimulusFeedbackSynchronizer feedback;
  StimulusBySpikeFrame feedbackFrame;
  feedbackFrame.accepted = true;
  feedbackFrame.allowDispatch = true;
  feedbackFrame.intensityScale01 = 1.0f;
  ChipReadFrame weakRead;
  weakRead.signalQuality01 = 0.10f;
  NeuralChipAck weakAck;
  weakAck.packetAccepted = true;
  weakAck.signalQuality = 0.10f;
  const auto weakFeedback =
      feedback.synchronizeFeedback(feedbackFrame, weakRead, weakAck);
  assert(!weakFeedback.accepted);
  assert(!weakFeedback.allowDispatch);
  assert(feedback.requiresHold());
  assert(weakFeedback.feedbackExpectation01 < 0.20f);
  assert(std::strcmp(weakFeedback.reason,
                     "stimulus_feedback_ack_or_quality_hold") == 0);
}
