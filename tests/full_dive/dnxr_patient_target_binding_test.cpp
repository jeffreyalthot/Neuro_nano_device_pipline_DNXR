#include "full_dive/spike_bridge/PatientTargetBindingLayer.h"
#include <cassert>
using namespace dnxr;
int main() {
  VectorBundleFrame f;
  f.modalityCount = 1;
  f.modalities[0].modality = SpikeModality::Visual;
  f.modalities[0].enabled = true;
  f.modalities[0].confidence01 = 1;
  PatientSpecificNeuralMap m = PatientSpecificNeuralMap::simulationDefaults();
  m.visual.enabled = false;
  auto o = PatientTargetBindingLayer{}.bind(f, m);
  assert(!o.modalities[0].enabled && o.modalities[0].confidence01 == 0);

  f.modalities[0].enabled = true;
  f.modalities[0].confidence01 = 1.0f;
  f.modalities[0].risk01 = 0.0f;
  f.modalities[0].channelCount = 2;
  f.modalities[0].channels01[0] = 0.05f;
  f.modalities[0].channels01[1] = 1.0f;
  m.visual.enabled = true;
  m.visual.confidence01 = 0.40f;
  m.visual.sensitivity01 = 1.0f;
  m.visual.threshold01 = 0.10f;
  m.visual.maxAllowed01 = 0.60f;
  m.visual.maturity = NeuralChannelMaturity::Experimental;
  o = PatientTargetBindingLayer{}.bind(f, m);
  assert(o.modalities[0].enabled);
  assert(o.modalities[0].channels01[0] == 0.0f);
  assert(o.modalities[0].channels01[1] == 0.60f);
  assert(o.modalities[0].confidence01 == 0.40f);
  assert(o.modalities[0].risk01 > 0.0f);
}
