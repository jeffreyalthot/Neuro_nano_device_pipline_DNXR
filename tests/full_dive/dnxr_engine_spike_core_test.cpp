#include "full_dive/spike_bridge/EngineSpikeCore.h"
#include "full_dive/spike_bridge/EngineSpikeOutputValidator.h"
#include <cassert>
#include <cstring>
#include <limits>
using namespace dnxr;
int main() {
  SpikeEngineIntentFrame i;
  i.routedCount = 1;
  i.routedModalities[0].channelCount = 1;
  i.routedModalities[0].channels01[0] = .5f;
  i.routedModalities[0].confidence01 = 1;
  auto a = EngineSpikeCore{}.process(i);
  auto b = EngineSpikeCore{}.process(i);
  assert(a.spikeLanes01[0] == b.spikeLanes01[0]);
  assert(a.spikeLanes01[0] >= 0 && a.spikeLanes01[0] <= 1);
  i.holdRequired = true;
  assert(EngineSpikeCore{}.process(i).holdRequired);

  EngineSpikeFrame invalid;
  invalid.laneCount = invalid.spikeLanes01.size() + 1;
  auto checked = EngineSpikeOutputValidator{}.validate(invalid);
  assert(!checked.accepted);
  assert(std::strcmp(checked.reason,
                     "spike_lane_count_exceeds_fixed_capacity") == 0);

  invalid = EngineSpikeFrame{};
  invalid.spikeLanes01[0] = std::numeric_limits<float>::quiet_NaN();
  checked = EngineSpikeOutputValidator{}.validate(invalid);
  assert(!checked.accepted);
  assert(std::strcmp(checked.reason, "non_finite_spike_lane") == 0);

  invalid = EngineSpikeFrame{};
  invalid.totalSpikeLoad01 = 1.2f;
  checked = EngineSpikeOutputValidator{}.validate(invalid);
  assert(!checked.accepted);
  assert(std::strcmp(checked.reason, "total_spike_load_out_of_range") == 0);
}
