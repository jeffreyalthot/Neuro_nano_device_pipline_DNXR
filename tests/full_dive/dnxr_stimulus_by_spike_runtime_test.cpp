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
  assert(result.executedStageCount == DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT);
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
