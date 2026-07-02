#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
#include <cassert>
using namespace dnxr;
int main() {
  VectorIntentNormalizer n;
  NeuralMotorIntentVector m;
  m.confidence01 = .4f;
  auto mp = n.fromMotor(1, 2, m);
  assert(mp.motorLike);
  NeuralOlfactoryVector o;
  auto op = n.fromOlfactory(1, 2, o);
  assert(op.chemicalLike && op.sensoryLike);
  NeuralGustatoryVector g;
  assert(n.fromGustatory(1, 2, g).chemicalLike);
  NeuralVisualVector v;
  auto p = n.fromVisual(1, 2, v);
  assert(p.sensoryLike);
  for (float c : p.channels01)
    assert(c >= 0 && c <= 1);
}
