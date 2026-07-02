#include "full_dive/spike_bridge/VectorTemporalAlignmentBuffer.h"
#include <cassert>
using namespace dnxr;
int main() {
  VectorTemporalAlignmentBuffer b;
  VectorBundleFrame f;
  f.timestampMs = 10;
  f.modalityCount = 1;
  auto a = b.align(f, 20);
  assert(a.timestampMs == 20);
  assert(b.jitter01() > 0);
  auto old = b.align(f, 100);
  (void)old;
  assert(b.lastTimestampMs() == 100);
}
