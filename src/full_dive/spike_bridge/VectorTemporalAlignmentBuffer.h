#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class VectorTemporalAlignmentBuffer {
public:
  VectorBundleFrame align(const VectorBundleFrame &input, double runtimeNowMs);
  void reset();
  double lastTimestampMs() const;
  float jitter01() const;

private:
  double lastTs_ = 0;
  float jitter_ = 0;
  VectorBundleFrame lastSafe_{};
  bool has_ = false;
};
} // namespace dnxr
