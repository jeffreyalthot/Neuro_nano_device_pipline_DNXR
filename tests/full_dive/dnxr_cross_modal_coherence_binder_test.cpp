#include "full_dive/spike_bridge/CrossModalCoherenceBinder.h"
#include <cassert>
#include <cstring>
using namespace dnxr;
int main() {
  VectorBundleFrame f;
  f.modalityCount = 1;
  f.modalities[0].modality = SpikeModality::Vestibular;
  f.modalities[0].risk01 = .95f;
  auto o = CrossModalCoherenceBinder{}.bind(f);
  assert(!o.accepted);
  assert(std::strcmp(o.reason, "visual_vestibular_coherence_hold") == 0);

  VectorBundleFrame chemical;
  chemical.modalityCount = 1;
  chemical.modalities[0].modality = SpikeModality::Olfactory;
  chemical.modalities[0].chemicalLike = true;
  chemical.modalities[0].enabled = true;
  chemical.modalities[0].confidence01 = 1.0f;
  chemical.modalities[0].channelCount = 1;
  chemical.modalities[0].channels01[0] = 0.6f;
  chemical.modalities[0].sceneBinding01 = 0.0f;
  o = CrossModalCoherenceBinder{}.bind(chemical);
  assert(!o.accepted);
  assert(std::strcmp(o.reason, "chemical_scene_binding_hold") == 0);

  VectorBundleFrame impact;
  impact.modalityCount = 1;
  impact.modalities[0].modality = SpikeModality::Touch;
  impact.modalities[0].enabled = true;
  impact.modalities[0].confidence01 = 0.9f;
  impact.modalities[0].channelCount = 1;
  impact.modalities[0].channels01[0] = 0.7f;
  o = CrossModalCoherenceBinder{}.bind(impact);
  assert(o.accepted);
  assert(o.modalities[0].coherenceNeed01 >= 0.5f);
}
