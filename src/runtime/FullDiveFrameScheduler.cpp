#include "FullDiveFrameScheduler.h"

namespace dnxr {

FullDiveFrameScheduler::FullDiveFrameScheduler(float targetHz)
    : targetHz_(targetHz) {}

FullDiveFrameScheduleDecision
FullDiveFrameScheduler::plan(bool safetyReady, bool hardRealtimeReady) const {
  FullDiveFrameScheduleDecision decision;
  decision.stageOrder = {"acquisition",
                         "signal_quality",
                         "artifact",
                         "baseline",
                         "intent",
                         "calibration",
                         "immersion",
                         "fulldive_frame",
                         "environment_sync",
                         "five_sense",
                         "body_schema",
                         "vector_intent_normalize",
                         "vector_temporal_align",
                         "patient_target_bind",
                         "cross_modal_coherence",
                         "vector_to_spike_intent",
                         "engine_spike",
                         "spike_output_validate",
                         "spike_envelope_policy",
                         "spike_to_stimulus_intent",
                         "stimulus_packet_assemble",
                         "stimulus_feedback_sync",
                         "safety",
                         "dose",
                         "latency",
                         "render",
                         "translate",
                         "hal",
                         "ack",
                         "feedback",
                         "correction",
                         "audit",
                         "finalizer"};
  if (!safetyReady || !hardRealtimeReady) {
    decision.runFrame = false;
    decision.reason = "safety_or_hard_realtime_not_ready";
  }
  return decision;
}

float FullDiveFrameScheduler::targetHz() const { return targetHz_; }

} // namespace dnxr
