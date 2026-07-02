#include "full_dive/spike_bridge/EngineSpikeCore.h"
#include <cassert>
using namespace dnxr;
int main() {
  SpikeEngineIntentFrame i;
  auto a = EngineSpikeCore{}.process(i);
  i.globalLoad01 = .9f;
  auto b = EngineSpikeCore{}.process(i);
  assert(a.frameId == b.frameId);
}
