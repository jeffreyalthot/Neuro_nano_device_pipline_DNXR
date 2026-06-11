#include "neural_chip/PatientModalityPolicyGate.h"
#include <cassert>
int main(){ dnxr::FullDiveChipFrame f; f.visual.globalLuminance01=1.0f; f.olfactory.odorPresence01=1.0f; auto map=dnxr::PatientSpecificNeuralMap::simulationDefaults(); map.visual.confidence01=1.0f; map.olfactory.enabled=false; auto out=dnxr::PatientModalityPolicyGate{}.apply(f,map); assert(out.visual.globalLuminance01<=0.5f); assert(out.olfactory.odorPresence01==0.0f); }
