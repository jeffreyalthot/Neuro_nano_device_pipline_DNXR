#include "full_dive/sync/FullDiveEnvironmentSynchronizationGraph.h"
#include <cassert>
using namespace dnxr;
int main() {
  FullDiveEnvironmentSynchronizationGraph g;
  NeuralChipOnlyWorldFrame w;
  w.frameId = 1;
  w.realityAnchor.required = true;
  auto n = g.buildNode(w);
  assert(!g.requiresHold(n));
  n.globalCoherence01 = 0;
  assert(g.requiresHold(n));
}
