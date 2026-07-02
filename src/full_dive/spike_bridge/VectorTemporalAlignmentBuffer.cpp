#include "full_dive/spike_bridge/VectorTemporalAlignmentBuffer.h"
#include <cmath>
namespace dnxr {
VectorBundleFrame
VectorTemporalAlignmentBuffer::align(const VectorBundleFrame &in, double now) {
  auto o = in;
  double j = std::fabs(now - in.timestampMs);
  jitter_ = spikeClamp01(static_cast<float>(j / 100.0));
  if (has_ && in.timestampMs + 50.0 < lastTs_) {
    o = lastSafe_;
    o.accepted = false;
    o.reason = "old_vector_frame_held_on_last_safe_state";
  }
  o.timestampMs = now;
  for (auto &p : o.modalities) {
    p.timestampMs = now;
    if (jitter_ > 0.5f)
      p.confidence01 *= 0.8f;
  }
  if (jitter_ > 0.2f)
    o.globalCoherence01 *= 0.8f;
  sanitize(o);
  lastTs_ = now;
  lastSafe_ = o;
  has_ = true;
  return o;
}
void VectorTemporalAlignmentBuffer::reset() {
  lastTs_ = 0;
  jitter_ = 0;
  has_ = false;
  lastSafe_ = {};
}
double VectorTemporalAlignmentBuffer::lastTimestampMs() const {
  return lastTs_;
}
float VectorTemporalAlignmentBuffer::jitter01() const { return jitter_; }
} // namespace dnxr
