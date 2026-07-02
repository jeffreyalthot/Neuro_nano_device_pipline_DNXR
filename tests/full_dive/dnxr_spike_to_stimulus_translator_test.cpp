#include "full_dive/spike_bridge/SpikeToStimulusIntentTranslator.h"
#include <cassert>
using namespace dnxr;
int main() {
  EngineSpikeFrame e;
  e.laneCount = 1;
  e.spikeLanes01[0] = .7f;
  e.modalityLoad01[0] = .3f;
  auto s = SpikeToStimulusIntentTranslator{}.translate(e);
  assert(s.boundedChannels01[0] == .7f);
  assert(s.modalityIntensity01[0] == .3f);
  assert(!s.allowDispatch);
}
